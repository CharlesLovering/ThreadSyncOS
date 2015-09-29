/*
 Header file containing structs and function prototypes.
 Author: Charlie Lovering
 */

/* imports */
 #include <semaphore.h>

/* constants */
#define MAXRATS 5
#define MAXROOMS 8

/* time macro for relative time - NOTE: I am not really sure why its time - func_start, kinda interesting though */
#define RELATIVE_TIME (time(NULL) - func_start)

/* structs */
struct vbentry {
	int iRat;   /* rat indentifier */
	int iEntry; /* time of entry */
	int tDep;   /* time of departure from the room */
} ;


/* global vars */
long int func_start; 						/* time() at function start) */
struct vbentry RoomVB[MAXROOMS][MAXRATS];	/* array of room vistor logs */	
int VisitorCount[MAXROOMS];					/* keeps track of how many rats entered a room */
int total_time;								/* the total time of rats going through the maze */
sem_t Rooms[MAXROOMS];						/* the semaphores which implement the room's capacity */
int NUM_ROOMS;								/* number of rooms read from file */
int NUM_RATS;								/* number of rats entered by command line arg */
int RoomInfo[MAXROOMS][2];					/* info on rooms read from file */
char ALGORITHIM;							/* algorithim read from command line arg */
pthread_mutex_t mutex;						/* semaphore/mutex used to protect regions */


/* function prototypes */
void print_usage();
int EnterRoom(int iRat, int iRoom);
int LeaveRoom(int iRat, int iRoom, int iEnter);
void init_structs();
void *Rat(void *iRat);
void print_stats();
void BeginRegion();
void EndRegion();




