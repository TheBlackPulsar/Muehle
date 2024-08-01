//#define DEBUG
#include "muehle.h"
#include <fcntl.h>

static volatile sig_atomic_t flag = 0;
static sigset_t newMask, oldMask, nullMask;
extern int ongoingGame;

// gamedata is stored here
extern void *shmThinkerConnector;

void createThreads(int shmID_ThinkerConnector) {
  pid_t pid;
  int gameOver[2];
  //char message[2];

  int pipeError = pipe(gameOver);
  if (pipeError) {
    perror("Fehler beim erstellen der Pipe");
    exit(EXIT_FAILURE);
  }

  signalSync();

  if ((pid = fork()) < 0) {
    perror("Fehler beim erzeugen der Prozesse.\n");
    exit(EXIT_FAILURE);
  } else if (!pid) {
    shmThinkerConnector = shmat(shmID_ThinkerConnector, NULL, 0);
    if (shmThinkerConnector == (void *)-1) {
      perror("Fehler bei shmat() fuer Connector\n");
      exit(EXIT_FAILURE);
    }
    Spieldaten *shmThinkerConnectorPTR = (Spieldaten *)shmThinkerConnector;
    shmThinkerConnectorPTR->Connector = getpid();
 	shmThinkerConnectorPTR->endGame = 0;

    unsigned int start = 1;
    while (ongoingGame){
      waitNow();
      DEBUG_PRINT("Connector");
      connectorFunctionality(start);
      if (start)
        start--;
      connectorToThinker(getppid());
    }
    DEBUG_PRINT("Trigger END");

    //close(gameOver[0]);
    //DEBUG_PRINT("Write End");
    //message[0] = 'D';
    //write(gameOver[1], message, 1);
    //close(gameOver[1]);
    
    shmThinkerConnectorPTR->endGame = 1;
    DEBUG_PRINT("Set Flag");

    shmDetatchAll(shmID_ThinkerConnector, shmThinkerConnectorPTR);
    DEBUG_PRINT("Set Flag");

    return;
  }

  shmThinkerConnector = shmat(shmID_ThinkerConnector, NULL, 0);
  if (shmThinkerConnector == (void *)-1) {
    perror("Fehler bei shmat() fuer Thinker\n");
    exit(EXIT_FAILURE);
  }
  Spieldaten *shmThinkerConnectorPTR = (Spieldaten *)shmThinkerConnector;
  shmThinkerConnectorPTR->Thinker = getpid();

  unsigned int skip = 1;
  unsigned int attach = 1;
  ssize_t endOfGame = 0;
  while (1) {
    if (skip)
      ;
    else if (attach) {
      shmAttach(shmThinkerConnectorPTR);
      thinkerFunctionality();
      attach--;
    } else
      thinkerFunctionality();

    DEBUG_PRINT("Thinker");

    //close(gameOver[1]);
      DEBUG_PRINT("Thinker1");
    if (skip)
      skip--;
    else {
        DEBUG_PRINT("Thinker2");
	DEBUG_PRINT("Flag: %i", shmThinkerConnectorPTR->endGame);
	if(shmThinkerConnectorPTR->endGame == 1) {
      		//endOfGame = read(gameOver[0], message, PIPE_BUF);
		endOfGame = 1;
        DEBUG_PRINT("Thinker3");}
      //close(gameOver[0]);
        DEBUG_PRINT("Thinker4");
      if (endOfGame){
        shmDetatchAll(shmID_ThinkerConnector, shmThinkerConnectorPTR);
	shmDestroyAll(shmID_ThinkerConnector, shmThinkerConnectorPTR);
        return;}
    }
    thinkerToConnector(pid);
    waitNow();
  }
  return;
}

void switchBlocking(int fd, int block){
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		exit(EXIT_FAILURE);
	if(block)
		flags &= ~O_NONBLOCK;
	else
		flags |= ~O_NONBLOCK;
	fcntl(fd, F_SETFL, flags);
}

void synchronize(int sigNumber) {
  signalSync();
  flag = 1;
}

void signalSync() {
  provideSignal(SIGUSR1, synchronize);
  provideSignal(SIGUSR2, synchronize);
  sigemptyset(&nullMask);
  sigemptyset(&newMask);
  sigaddset(&newMask, SIGUSR1);
  sigaddset(&newMask, SIGUSR2);
  if (sigprocmask(SIG_BLOCK, &newMask, NULL) < 0) {
    perror("Fehler bei sigprocmask() in signalSync\n");
    exit(EXIT_FAILURE);
  }
}

void *provideSignal(int sigNumber, void *sigHandler) {
  struct sigaction newSig;
  newSig.sa_handler = sigHandler;
  sigemptyset(&newSig.sa_mask);
  newSig.sa_flags = SA_RESTART;
  if (sigaction(sigNumber, &newSig, NULL) < 0)
    return SIG_ERR;
  return newSig.sa_handler;
}

void connectorToThinker(pid_t processID) { kill(processID, SIGUSR1); }
void thinkerToConnector(pid_t processID) { kill(processID, SIGUSR2); }

void waitNow() {
  while (!flag)
    sigsuspend(&nullMask);
  flag = 0;
  if (sigprocmask(SIG_BLOCK, &oldMask, NULL) < 0) {
    perror("Fehler bei sigprocmask() in waitNow\n");
    exit(EXIT_FAILURE);
  }
}
