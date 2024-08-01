#ifndef muehle_h
#define muehle_h

#include "debug.h"

/* for the gameserver
#define GAMEKINDNAME "NMMorris"
#define PORTNUMMER 1357
#define HOSTNAME "sysprak.priv.lab.nm.ifi.lmu.de" */

// some useful includes. less typing
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// struct for Players
typedef struct {
  char Spielername[64];
  int Spielernummer;
  int Spielerbereit; // 0=notready ; 1=ready
} Player;

// struct for sharedMemory
typedef struct {
  char Spielname[64];
  char Spielername[64]; // ours
  int Spielernummer;    // ours
  int Spieleranzahl;
  pid_t Thinker;
  pid_t Connector;
  int endGame;
  int shmIdSpielerliste;
  int shmIdSpielfeld;
  int shmIdSteinliste;
  int ongoingGame;
  char move[16];
} Spieldaten;

typedef struct {
  char position[4];
  int owner;
  int stoneNumber;
} Stein;

typedef struct {
  int maximaleZugzeit;
  int zuSchlagendeSteine;
  int anzahlMitspieler;
  int anzahlSteine;
  // Stein* Steinliste;
} Spielfeld; // size: sizeof(Spielfeld) + anzahlMitspieler * anzahlSteine *
             // sizeof(Stein)

// struct for .config
typedef struct {
  int Portnummer;
  char Hostname[50];
  char Gamekind[50];
} CONFIG;

// main.c
void parseArguments(char **, CONFIG *, int, char **);
void help(); // prints help for I/O

// functionality.c
void connectorFunctionality(unsigned int);
void thinkerFunctionality(void);
void startSocket(char *, CONFIG *);

// socket.h
int createSocket(char *, int);

// performConnection.h
int getServerMessage(int, char *, int, ssize_t);
int formatierteAusgabe(char *, int);
int init_performConnect(int, char *);
// Call this function from ouside of performConnection.c
int performConnect(int, char *, char *);

// gameConnection.c
int printServerMessage(char *, int);
int performGameConnection(int);
int waitForSequenz();
void sendServer(char *);

// shm.c
/* returns shmID
 * needs Pointer to shmThinkerConnector */
int shmCreate(int);
void shmAttach(Spieldaten *);
void shmDetatch(int);
void shmDetatchAll(int, Spieldaten *);
void shmDestroy(int);
void shmDestroyAll(int, Spieldaten *);

// sync.c
/* needs shmID and Pointer to shmThinkerConnector and gameID */
void createThreads(int);
/*
 * These functions are for internal use in sync.c.
 */
void switchBlocking(int, int);
void synchronize(int);
void *provideSignal(int, void *);
void signalSync();
void waitNow();
void connectorToThinker(pid_t);
void thinkerToConnector(pid_t);

// config.c
/* needs filename and pointer to CONFIG struct */
void parseConfig(char *, CONFIG *);
/* needs pointer to the file name
 * returns FILE * to the content of the given file */
FILE *openFile(char *);
/* needs pointer to a char *
 * returns pointer to the given line, free of space and tab */
void trimBlanks(char *);
/* needs a pair of name, value and a CONFIG * parameters
 * places them in the parameters struct*/
void placeValues(char *, char *, CONFIG *);
/* needs pointer to CONFIG struct */
void checkValidity(CONFIG *);

// Thinker.c
void think();
void writeSHMDatainFeld(int *, Stein *, Spielfeld *);
void printField(int *, Spielfeld *);
char positioner(int *, Spielfeld *, int);

// CPU.c
/* returns neighbour of a position as an array
 * returns NULL if position is invalid */
void findNeighbour(int, int *);

/* needs a board and a position
 * returns count of possible mill at given pos in current board
 * returns -1 if position is invalid */
int fastMuehle(int *, int);

/* transforms position back into String
 * returns NULL if position is invalid */
char *posString(int);

int *legalMovesOphase(int *);

int legalMoveCounter();

int *legalMovesMEphase(int *, int, int);
#endif // muehle_h
