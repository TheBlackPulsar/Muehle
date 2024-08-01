#include "muehle.h"

// gamedata is stored here
extern void *shmThinkerConnector;
// Players are stored here
extern void *shmSpielerliste;
// maximaleZugzeit, zuSchlagendeSteine,anzahlMitspieler, anzahlSteine
extern void *shmSpielfeld;
// Stone- position, number and owner is stored here
extern void *shmSteinliste;

int shmCreate(int size) {
  int shmID = shmget(IPC_PRIVATE, size, IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
  if (shmID == -1) {
    perror("Fehler beim kreiern des shm.\n");
    printf("Mit key %d und Groesse %d.\n", IPC_PRIVATE, size);
    exit(EXIT_FAILURE);
  }

  DEBUG_PRINT("SHM-ID: %i\n", shmID); // debug print
  return shmID;
}

void shmAttach(Spieldaten *shmThinkerConnectorPTR) {
  shmSpielerliste = shmat(shmThinkerConnectorPTR->shmIdSpielerliste, NULL, 0);
  shmSpielfeld = shmat(shmThinkerConnectorPTR->shmIdSpielfeld, NULL, 0);
  shmSteinliste = shmat(shmThinkerConnectorPTR->shmIdSteinliste, NULL, 0);
}

void shmDetatch(int shmID) {
  void *IDPTR = &shmID;
  shmdt(IDPTR);
}

void shmDetatchAll(int shmID_ThinkerConnector,
                   Spieldaten *shmThinkerConnectorPTR) {
  shmDetatch(shmThinkerConnectorPTR->shmIdSpielerliste);
  shmDetatch(shmThinkerConnectorPTR->shmIdSpielfeld);
  shmDetatch(shmThinkerConnectorPTR->shmIdSteinliste);
  shmDetatch(shmID_ThinkerConnector);
}

void shmDestroy(int shmID) { shmctl(shmID, IPC_RMID, NULL); }

void shmDestroyAll(int shmID_ThinkerConnector,
                   Spieldaten *shmThinkerConnectorPTR) {
  shmDestroy(shmThinkerConnectorPTR->shmIdSpielerliste);
  shmDestroy(shmThinkerConnectorPTR->shmIdSpielfeld);
  shmDestroy(shmThinkerConnectorPTR->shmIdSteinliste);
  shmDestroy(shmID_ThinkerConnector);
}
