// #define DEBUG

#include "muehle.h"
#include <time.h>

/* End the loops in Thinker and Connector with 0 */
int ongoingGame = 1;
void *shmThinkerConnector; // gamedata is stored here
void *shmSpielerliste;     // Players are stored here
void *shmSpielfeld;  // maximaleZugzeit, zuSchlagendeSteine, anzahlMitspieler,
                     // anzahlSteine
void *shmSteinliste; // Stone- position, number and owner is stored here
int sock;

char *gameID[14]; // GetOpt kommandozeile einlesen
int player;       // GetOpt kommandozeile einlesen
CONFIG *config;   // for infos from +.config

int main(int argc, char *argv[]) {
  srand(time(NULL));

  config = (CONFIG *)malloc(sizeof(CONFIG));
  parseArguments(gameID, config, argc, argv);

  int shmID_ThinkerConnector = shmCreate(sizeof(Spieldaten));

  createThreads(shmID_ThinkerConnector);

  free(config);

  return EXIT_SUCCESS;
}

void parseArguments(char **gameID, CONFIG *config, int argc, char **argv) {
  int opt, g = 0, p = 0, c = 0;
  while ((opt = getopt(argc, argv, ":g:p:c:h")) != -1) {
    switch (opt) {
    case 'g':
      *gameID = optarg;
      DEBUG_PRINT("GameID Input: %s\n", *gameID);
      g = 1;
      break;
    case 'p':
      player = atoi(optarg);
      if ((player - 1) == 0 || (player - 1) == 1) {
        DEBUG_PRINT("Gewünschter Spieler Input: %d\n", player);
        // shmThinkerConnectorPTR->Spielernummer = player;
        p = 1;
        break;
      } else {
        exit(EXIT_FAILURE);
      }
    case 'c':
      parseConfig(optarg, config);
      c = 1;
      break;
    case 'h':
      help();
      exit(EXIT_FAILURE);
    default:
      help();
      DEBUG_PRINT("Default");
      exit(EXIT_FAILURE);
    }
  }
  if (!g) { // makes g mandatory
    printf("Bitte Game-ID angeben.\n");
    help();
    exit(EXIT_FAILURE);
  }
  if (!p) { // set 1 as default value
    player = 1;
  }
  if (!c) { // set client.conf as default value
    parseConfig("client.conf", config);
    DEBUG_PRINT("client.conf");
  }
}

void help() {
  printf("Die folgenden Optionen koennen uebergeben werden:\n");
  printf("-g\tNotwendig\tGame-ID um ein Spiel zu starten\n");
  printf("-p\tOptional\t Gewünschte Spielernummer angeben\n");
  printf("-c\tOptional\t Namen der .conf angeben\n");
  printf("-h\tDiese Hilfe drucken und verlassen\n");
}
