#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include "RatThread.h"
#include <time.h>
#include <semaphore.h>
#include <string>


/* rat thread project */
using namespace std;


/* NOTE: all globals and prototypes in header file */
int main (int argc, char* argv[]){

	/* thread of rat arrays */
	pthread_t Rats[MAXRATS];


	/* setting time */
	func_start = (long int) time(NULL);

	/* create the mutex */
	if (pthread_mutex_init(&mutex, NULL) < 0) {
		perror("pthread_mutex_init");
		exit(1);
    } 

	/* function info - will be set globally */
	int num_of_rooms = 0;
	int num_of_rats = 0;
	char algorithim;


	/* read rooms file, getting  */
	ifstream rooms ("rooms");
	if (rooms.is_open()){
		string line;
		int j = 0;
		while (getline (rooms, line)){

			if (line.length() <= 2){
				continue; /* skip empty lines */
			}

			/* parse file */
  			istringstream sin( line );
    	   	int i, z = 0;
        	while( sin >> i || sin.good()) {
        		if (sin.fail()){
        			cerr << "PARSING 'rooms' FAILED" << endl << cout;
        			exit(-1);
        		}
            	RoomInfo[j][z] = i;
            	z++;
       		}
       	    j++;
		}

		/* setting the number of rooms for ease later */
		num_of_rooms = j;
	} else {
		/* rooms file missing */
		cerr << "FILE 'rooms' DNE" << endl;
		exit(-1);
	}
	rooms.close();

	/* read args */
	if (argc != 3){
		print_usage();
		exit(-1);
	} else {
		/* set command line arg vals */
		num_of_rats = atoi(argv[1]);
		algorithim = argv[2][0];

		/* checking which algo */
		if (!(algorithim == 'i' || algorithim == 'd' || algorithim == 'n' || algorithim == 'N' || algorithim == 'D' || algorithim == 'I')){
			cout << "ALGORITHIM DNE" << endl;
			exit(-1);
		}

		/* checks number of rats */
		if (num_of_rats > MAXRATS){
			cout << "OI! TOO MANY RATS!" << endl;
			exit(-1);
		}
	}

	/* creating/initializing structs */
	init_structs();

	/* set the global variables */
	NUM_ROOMS = num_of_rooms;
	NUM_RATS = num_of_rats;
	ALGORITHIM = algorithim;

	/* init the sems */
	for (int i = 0; i < NUM_ROOMS; i++){
		if (sem_init(&Rooms[i], 0, RoomInfo[i][0]) < 0) {
        	perror("sem_init");
        exit(1);
   		}
	}

	/* init the threads (rats) */
	for (int i = 0; i < NUM_RATS; i++){
		/* create the rat */
		if(pthread_create(&Rats[i], NULL, Rat, (void *)i) != 0){
			perror("pthread_create");
       	    exit(1);
		}
	}

	/* wait for all Rats (threads) to finish */
	for (int i = 0; i < NUM_RATS; i++){
		(void) pthread_join(Rats[i], NULL);
	}
	
	/* print stats */
	print_stats();

	/* destroy the mutex */
    (void)pthread_mutex_destroy(&mutex);
	return 0;
}

/* print the RoomVB stats */
void print_stats(){
	/* print stats */
	for (int i = 0; i < NUM_ROOMS; i++){
		printf("Room %d [%d %d]:", i, RoomInfo[i][0], RoomInfo[i][1]);
		for (int j = 0; j < NUM_RATS; j++){
			printf(" %d %d %d;", RoomVB[i][j].iRat, RoomVB[i][j].iEntry, RoomVB[i][j].tDep);
		}
		cout << endl;
	}

	/* get ideal_time */
	int ideal_time = 0;
	for (int i = 0; i < NUM_ROOMS; i++){
		ideal_time += RoomInfo[i][1] * NUM_RATS;
	}

	printf("Total traversal time: %d seconds, compared to ideal time: %d seconds\n", total_time, ideal_time);
}

/* usage */
void print_usage(){
	cout << "###############" << " USAGE " << "################" << endl;
	cout << "proper format: ./maze [NUM] [ALGORITHIM]" << endl;
	cout << "0 <= NUM <= 5" << endl;
	cout << "ALGORITHIM == i || d || n" << endl;
	cout << "######################################" << endl;
}

/* enter room */
int EnterRoom(int iRat, int iRoom){
	/* enters room */
    sem_wait(&Rooms[iRoom]);
    int iEnter = RELATIVE_TIME;
	sleep(RoomInfo[iRoom][1]);
	return iEnter;
}

/* checks if it is possible to enter room */
int TryToEnterRoom(int iRat, int iRoom){
	/* try to enter room */
	int sval;
    sem_getvalue(&Rooms[iRoom], &sval);
 	if (sval > 0){ /* if room has space return 0 */
 		return 0;
 	}
 	return -1;
}

/* leave room */
int LeaveRoom(int iRat, int iRoom, int iEnter){
	/* we're out! */
    sem_post(&Rooms[iRoom]);

	/* logging information */
	BeginRegion();
	int index = VisitorCount[iRoom];
	RoomVB[iRoom][index].iRat = iRat;
	RoomVB[iRoom][index].iEntry = iEnter;
	RoomVB[iRoom][index].tDep = RELATIVE_TIME;
	VisitorCount[iRoom]++;
	EndRegion();

	return index; /* returns the index of the room last left */
}

/* Thread Rat */
void *Rat(void *iRat_void){
	/* iRat = rat index/id */
	int iRat = (long) iRat_void;

	/* cross initilization error fix: */
	int index = -1;
	int i = 0;
	int rooms_entered = 0;
	int room_ready = 2;
	int location = -1;
	bool entered[MAXROOMS];
	for (int i = 0; i < NUM_ROOMS; i++){
		entered[i] = false;
	}

	switch (ALGORITHIM){
		case 'I':
		case 'i':
			/* go through all rooms in order 0 -> N */
			for (i = 0; i < NUM_ROOMS; i++){
				int iEnter = EnterRoom(iRat, i);
				location = LeaveRoom(iRat, i, iEnter);
			}
			/* log completion time! */
			printf("Rat %d completed maze in %d seconds\n", iRat, RoomVB[NUM_ROOMS - 1][location].tDep);
			BeginRegion();
			total_time += RoomVB[NUM_ROOMS - 1][location].tDep;
			EndRegion();
			break;
		case 'D':
		case 'd':
			/* go through all room in order iRat -> N -> iRat -1 */
			i = 0;

			/* go through everyroom - and go into room that corresponds to the iRat. (wrap around). */
			while (i < NUM_ROOMS){
				index = (iRat + i) % (NUM_ROOMS);
				int iEnter = EnterRoom(iRat, index);
				location = LeaveRoom(iRat, index, iEnter);
				i++;
			}

			if (index != - 1){
				printf("Rat %d completed maze in %d seconds\n", iRat, RoomVB[index][location].tDep);
				BeginRegion();
				total_time += RoomVB[index][location].tDep;
				EndRegion();
			}


			break;
		case 'N':
		case 'n':
			
			/* go through all room in order iRat -> N -> iRat - 1 */
			i = 0;
			while (i < NUM_ROOMS){ /* while it hasn't visited all the rooms */
					index = (iRat + i) % (NUM_ROOMS); 
												  		 /* set index: starting off with an index based off of the d algo
												 		  * so that by default the rats start off in different rooms, so as to
													      * interfere less with each other. 
														  */

				/* find the next empty room - working off the index */
				int j = 0;
				while (j < NUM_ROOMS){ /* until we find a room that is both empty and that we haven't been to */
					int enterVal = TryToEnterRoom(iRat, index); /* is room empty */
					if ((enterVal == 0) && (!entered[index])){
						/* a good room has been found! */
						/* exit loop */
						break;
					} else {
						/* check next room */
						/* increment index */
						index = (index + 1) % NUM_ROOMS;
					}
					j++; /* no room found, keep looking */
				}

				/* no empty room found */
				if (j == NUM_ROOMS){
					index = (iRat + 1) % NUM_ROOMS; /* if all rooms not empty, default to behavior similar to the d algorithim. */
				}

				/* find the first room that wasn't entered already - by this rat */
				while (entered[index] == true){
					index = (index + 1) % NUM_ROOMS; /* increment, checking the next room */
				}
				entered[index] = true; /* record that this room has been entered by this rat. */
				int iEnter = EnterRoom(iRat, index); /* enter room */
				location = LeaveRoom(iRat, index, iEnter); /* leave room */
				i++; /* next room */
			}

			/* get largest */
			if (index != - 1){
				printf("Rat %d completed maze in %d seconds\n", iRat, RoomVB[index][location].tDep);
				BeginRegion();
				total_time += RoomVB[index][location].tDep;
				EndRegion();
			}
			break;
		}
}


/* init logs */
void init_structs(){


	/* init the RoomVB structure */
	for (int i = 0; i < NUM_ROOMS; i++){
		for (int j = 0; j < NUM_RATS; j++){
			vbentry* entry = new vbentry;
			entry -> iRat = -1;
			entry -> iEntry = -1;
			entry -> tDep = -1;
			RoomVB[i][j]= *entry;
		}
	}

	/* used so that the RoomVB struct logs rats in order of entering a room */
	for (int i = 0; i < NUM_ROOMS; i++){
		VisitorCount[i] = 0;
	}
}

/* mutex functions */
void BeginRegion()
{
    pthread_mutex_lock(&mutex);
}

void EndRegion()
{
    pthread_mutex_unlock(&mutex);
}


