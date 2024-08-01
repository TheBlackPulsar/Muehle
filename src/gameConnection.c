//#define DEBUG
#include "muehle.h"

extern void* shmThinkerConnector;
extern void* shmSpielerliste; //shm detatch noch überall
extern void* shmSpielfeld;
extern void* shmSteinliste;

extern int sock;

extern int ongoingGame;

Spielfeld SpielfeldForSHM;
Stein* listOfStones;
Stein tmpStein;
int stoneListPos = 0;
int gameIsRunning = 1;
int firsttime1 =1;
int firsttime2 = 1;

int performGameConnection(int sock){
    
    int BUF = 100;
    char buffer[BUF];
    ssize_t size = 0;
    int sequenz;

    Spieldaten* shmThinkerConnectorPTR = (Spieldaten*) shmThinkerConnector;
    if(!firsttime2){sendServer(shmThinkerConnectorPTR->move);}
    if(firsttime2){
    shmThinkerConnectorPTR->shmIdSpielfeld = shmCreate(sizeof(Spielfeld));
    firsttime2 = 0;
    }

while(gameIsRunning){

    sequenz = waitForSequenz(); //RETURN: (0: Timeout) (1: MOVE) (2: WAIT) (3: GAMEOVER) (4: Invalid Move) (-1: error)

	//Spielfeld *shmSpielfeldPTR;
	//Player *shmSpielerlistePTR; 
	//Stein *shmSteinlistePTR;

    switch(sequenz){
        case -1: 
            DEBUG_PRINT("Case -1: This is not good!");
            getServerMessage(sock, buffer, BUF, size);
            printServerMessage(buffer,0);
            gameIsRunning = 0;
            DEBUG_PRINT("Game stopped");
            ongoingGame = 0;
            exit(EXIT_FAILURE);
            break;
            
        case 0:
            DEBUG_PRINT("-----TIMEOUT-----");
            getServerMessage(sock, buffer, BUF, size);
            printServerMessage(buffer,0);
            gameIsRunning = 0;
            DEBUG_PRINT("Game stopped");
            ongoingGame = 0;
            exit(EXIT_FAILURE);
            break;

        case 1: //MOVE
            //S: + CAPTURE <<Anzahl zu schlagender Steine>>
            getServerMessage(sock, buffer, BUF, size);
            printServerMessage(buffer,1);

            //S: + PIECELIST <<Anzahl Mitspieler>>,<<Anzahl Steine>>
            getServerMessage(sock, buffer, BUF, size);
            printServerMessage(buffer,2);

            if(firsttime1){
                //Create SHM for Steinliste
                shmThinkerConnectorPTR->shmIdSteinliste = shmCreate(SpielfeldForSHM.anzahlMitspieler * SpielfeldForSHM.anzahlSteine * sizeof(Stein));
                firsttime1=0;
            }
            
            // Spielfeld SHM
            shmSpielfeld = shmat(shmThinkerConnectorPTR->shmIdSpielfeld, NULL, 0);
            //Spielfeld *shmSpielfeldPTR = (Spielfeld *)shmSpielfeld;

            // Spielerliste SHM
            shmSpielerliste = shmat(shmThinkerConnectorPTR->shmIdSpielerliste, NULL, 0);
            //Player *shmSpielerlistePTR = (Player *)shmSpielerliste;

            // Steinliste SHM
            shmSteinliste = shmat(shmThinkerConnectorPTR->shmIdSteinliste, NULL, 0);
            //Stein *shmSteinlistePTR = (Stein *)shmSteinliste;

            //S: + PIECE<<Mitspielernummer>>.<<Steinnummer>> <<Position>>
            for(int i=0; i< SpielfeldForSHM.anzahlMitspieler * SpielfeldForSHM.anzahlSteine; i++){
                getServerMessage(sock, buffer, BUF, size);
                printServerMessage(buffer,3);
            }

            stoneListPos = 0;

            //S: + ENDPIECELIST
            getServerMessage(sock, buffer, BUF, size);
            printServerMessage(buffer,0);

            //C: THINKING
            sendServer("THINKING");
            DEBUG_PRINT("CLIENT: + THINKING\n");

            memcpy(shmSpielfeld, &SpielfeldForSHM, sizeof(Spielfeld));
            memcpy(shmSteinliste, listOfStones, SpielfeldForSHM.anzahlMitspieler * SpielfeldForSHM.anzahlSteine * sizeof(Stein));

            //S: + OKTHINK
            getServerMessage(sock, buffer, BUF, size);
            printServerMessage(buffer,0);

            //Starting Thinker

            //remove after testing
            for (int i = 0; i < 18; i++) {
            DEBUG_PRINT("SHMINFO %d: %s", i, shmSteinlistePTR[i].position);
            }
            DEBUG_PRINT("SHMINFO: %d", shmSpielfeldPTR->anzahlSteine);
            DEBUG_PRINT("Spielernummer: %i", shmSpielerlistePTR[0].Spielernummer);
            DEBUG_PRINT("SHM-ID Spielfeld main: %d", shmThinkerConnectorPTR->shmIdSpielfeld);
            DEBUG_PRINT("Hello :) Gameconnect here Pid:%d", getpid());
            DEBUG_PRINT("-----------------------------------------------");
            //endremove
            free(listOfStones);
            shmDetatch(shmThinkerConnectorPTR->shmIdSpielfeld);
            shmDetatch(shmThinkerConnectorPTR->shmIdSpielerliste);
            shmDetatch(shmThinkerConnectorPTR->shmIdSteinliste);
            return 0;

            //PLAYGROUND 
            /*
            int testing=1; //remove after Testing Playground
            if(testing==1){DEBUG_PRINT("--MOVE 1 played--"); sendServer("PLAY A0");}
            if(testing==2){DEBUG_PRINT("MOVE 2"); sendServer("PLAY A1");}
            if(testing==3){DEBUG_PRINT("MOVE 3"); sendServer("PLAY A2");}
            if(testing==4){DEBUG_PRINT("MOVE 4"); sendServer("PLAY B0");}
            if(testing==5){DEBUG_PRINT("MOVE 5"); sendServer("PLAY B2");}
            testing++;
            //ENDPLAYGROUND 
            */
            break;

        case 2: //WAIT
            sendServer("OKWAIT");
            DEBUG_PRINT("OKWAIT has been sent");
            break;

        case 3: //GAMEOVER
            getServerMessage(sock, buffer, BUF, size);
            printServerMessage(buffer,0);
            DEBUG_PRINT("GAMEOVER has been triggered, this is not yet fully implemented");
            gameIsRunning = 0;
            DEBUG_PRINT("Game stopped");
            ongoingGame = 0;
            return 0;

        case 4:
            DEBUG_PRINT("----INVALID MOVE-------\n        NOT yet fully implemented!!\n");
            getServerMessage(sock, buffer, BUF, size);
            printServerMessage(buffer,0);
            gameIsRunning = 0;
            DEBUG_PRINT("Game stopped");
            ongoingGame = 0;
            break;

        case 5:
            break;

        case 6:
            DEBUG_PRINT("- Did not get the expected PLAY command");
            shmThinkerConnectorPTR->ongoingGame = 0;
            return 0;
            //exit(EXIT_FAILURE);
            break;

        default:
            DEBUG_PRINT("Unexpected Message | DEFAULT: This should not have happened");
            shmThinkerConnectorPTR->ongoingGame = 0;
            exit(EXIT_FAILURE);
            break;
    }
}

    free(listOfStones);
    shmDetatch(shmThinkerConnectorPTR->shmIdSpielfeld);
    shmDetatch(shmThinkerConnectorPTR->shmIdSpielerliste);
    shmDetatch(shmThinkerConnectorPTR->shmIdSteinliste);
    return 0;
}

int printServerMessage(char *buffer, int messageNumber){

    //Server Nachricht in einzelne Worte trennen, auf die wir zugreifen koennen:
    char words[20][20]; 
  
    //strtok extrahiert strings bis zu einem bestimmten char in einem token
    char * token = strtok(buffer, " "); //ACHTUNG: MANCHE WORTE HABEN \n AM ENDE
    int token_anzahl = 0;
    while( token != NULL ) {
        //printf( " %s\n", token ); //print alle token
        strcpy(words[token_anzahl], token);
        token_anzahl++;
        token = strtok(NULL, " ");
    }
    
    for(int i=0; i<token_anzahl; i++) {
        printf("%s\n", words[i]);
    }

    char tmp[20][20];
    switch (messageNumber){//SHM handling
    case 0: break; //only print

    case 1:
        SpielfeldForSHM.zuSchlagendeSteine = atoi(words[2]);
        break;

    case 2:
        token = strtok(words[2], ",");
        token_anzahl = 0;
        while( token != NULL ) {
            strcpy(tmp[token_anzahl], token);
            token_anzahl++;
            token = strtok(NULL, ",");
        }
        SpielfeldForSHM.anzahlMitspieler = atoi(tmp[0]);
        SpielfeldForSHM.anzahlSteine = atoi(tmp[1]);
        listOfStones = malloc(SpielfeldForSHM.anzahlMitspieler * SpielfeldForSHM.anzahlSteine*sizeof(Stein));
        break;

    case 3:
        token = strtok(words[1]+5, ".");
        token_anzahl = 0;
        while( token != NULL ) {
            strcpy(tmp[token_anzahl], token);
            token_anzahl++;
            token = strtok(NULL, ",");
        }
        tmpStein.owner = atoi(tmp[0]);
        tmpStein.stoneNumber = atoi(tmp[1]);
        strncpy(tmpStein.position, words[2], 2); //ohne /n am ende
        listOfStones[stoneListPos] = tmpStein;
        stoneListPos++;
        break; 

    case 4: 
        break;
    }

    return 0;
}

int waitForSequenz(char* buffer){
    char words[20][20];
    do{
        int BUF = 100;
        char *buffer = malloc( sizeof(char) * BUF );
        ssize_t size = 0;
        
        getServerMessage(sock, buffer, BUF, size);

        char words[20][20]; 

        //strtok extrahiert strings bis zu einem bestimmten char in einem token
        char * token = strtok(buffer, " "); //ACHTUNG: MANCHE WORTE HABEN \n AM ENDE
        int token_anzahl = 0;
        while( token != NULL ) {
            //printf( " %s\n", token );
            strcpy(words[token_anzahl], token);
            token_anzahl++;
            token = strtok(NULL, " ");
        }
        free(buffer);

        DEBUG_PRINT("CASE: %s has been detected\n", words[1]);
        
        /* Print out Server Message
        for(int i=0; i<token_anzahl; i++) {
            printf("%s\n", words[i]);
        }
        */

        if(strcmp(words[1], "MOVE") == 0){
            //TODO Here Action to Process MOVETIME!!
            //S: + MOVE <<Maximale Zugzeit>>
            SpielfeldForSHM.maximaleZugzeit = atoi(words[2]);
            for(int i=0; i<token_anzahl; i++) {
                printf("%s\n", words[i]);
            }
            return 1;
        }
        
        if(strcmp(words[1], "WAIT\n")== 0)return 2;

        if(strncmp(words[1], "GAMEOVER", 8)== 0) return 3;

        if (strcmp(words[1], "TIMEOUT")== 0) return 0;

        if (strcmp(words[1], "Invalid")== 0) return 4;

        if(strcmp(words[1], "MOVEOK\n")== 0)return 5;

        if(strcmp(words[1], "Did")== 0)return 6;
        
        else return -1;

    }
    while(!strcmp(words[1],"MOVE") || !strcmp(words[1],"WAIT") || !strcmp(words[1],"GAMEOVER"));
}

void sendServer(char *message){
    char dest[50];
    strcpy(dest, message);
    strcat(dest, "\n");
    //DEBUG_PRINT("MSG: %s", dest);
    send(sock, dest, strlen(dest), 0);
}
