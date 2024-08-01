//#define DEBUG
#include "muehle.h"

extern int ongoingGame;

extern int sock;

extern char *gameID[14]; // GetOpt kommandozeile einlesen
extern CONFIG *config;   // for infos from +.config

void connectorFunctionality(unsigned int start) {
  /* Ongoing connectorFunctionality place ongoingGame = 0 behind an appropriate
   * if to end the loop after you are done */
  DEBUG_PRINT("Connector here!! Pid:%d", getpid());
  if (start)
    startSocket(*gameID, config);

  performGameConnection(sock);
  DEBUG_PRINT("connector funct. ends here");
  //ongoingGame = 0;
}

void thinkerFunctionality(void) {
  DEBUG_PRINT("Thinking from now on!");
  think();
  /* Ongoing thinkerFunctionality place ongoingGame = 0 behind an appropriate
   * if to end the loop after you are done */
}

void startSocket(char *gameID, CONFIG *config) {
  // Filedeskritpor von Sockets
  int sock_stream = createSocket(config->Hostname, config->Portnummer); //gefahr
  sock = sock_stream;
  if (sock_stream < 0) {
    perror("sock_stream < 0");
    exit(EXIT_FAILURE);
  }
  if ((performConnect(sock_stream, gameID, config->Gamekind)) < 0) {
    perror("Programm wird vorzeitig beendet\n");
    exit(EXIT_FAILURE);
  }
}
