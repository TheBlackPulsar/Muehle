//#define DEBUG
#include <stdio.h>
#include "muehle.h"
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>

extern void* shmThinkerConnector; //gamedata is stored here
extern void* shmSpielerliste; //Players are stored here
extern void* shmSpielfeld; //maximaleZugzeit, zuSchlagendeSteine, anzahlMitspieler, anzahlSteine
extern void* shmSteinliste; //Stone- position, number and owner is stored here

int run = 0;
int scounter = 0;

void think(){
    // ThinkerConnector SHM is already attatched
    Spieldaten *shmThinkerConnectorPTR = (Spieldaten *)shmThinkerConnector;

    // Spielfeld SHM
    Spielfeld *shmSpielfeldPTR = (Spielfeld *)shmSpielfeld;

    // Spielerliste SHM
    //shmSpielerliste = shmat(shmThinkerConnectorPTR->shmIdSpielerliste, NULL, 0);
    //Player *shmSpielerlistePTR = (Player *)shmSpielerliste;

    // Steinliste SHM
    Stein *shmSteinlistePTR = (Stein *)shmSteinliste;

    int feld[24]; //0:A0 1:A1 ... 8:B0 9:B1 ...
    writeSHMDatainFeld(feld, shmSteinlistePTR, shmSpielfeldPTR); //0 is empty | 1 is us | 2 is the enemy
    //DEBUG_PRINT("%i", feld[1]);
    //DEBUG_PRINT("%i", feld[2]);

    //this is for testing only remove pls
    //for (int i = 0; i < 18; i++) {DEBUG_PRINT("SHMINFO %d: %s", i, shmSteinlistePTR[i].position);}
    DEBUG_PRINT("\n");
    DEBUG_PRINT("SHMAnzahl_zuschlagender_Steine: %d", shmSpielfeldPTR->zuSchlagendeSteine);
    //DEBUG_PRINT("Spielernummer: %i", shmSpielerlistePTR[0].Spielernummer);
    //DEBUG_PRINT("SHM-ID Spielfeld: %d", shmThinkerConnectorPTR->shmIdSpielfeld);
    //DEBUG_PRINT("Hello :) Thinker here Pid:%d", getpid());
    //end of tests

    char move[16];
    int rStones = shmSpielfeldPTR->zuSchlagendeSteine;
    int player =shmThinkerConnectorPTR->Spielernummer;
    int opponent = 2;
    if (player != 1) opponent = 1;
    int anzSteine = shmSpielfeldPTR->anzahlSteine;

    int opcount = 0;
    int opstonelist[9];

     int r;
    strcpy(move, "PLAY ");



    if (rStones > 0) {
        DEBUG_PRINT("Here");
        /*strcat(move, shmSteinlistePTR[9*(opponent-1)+scounter].position);
            //DEBUG_PRINT("(%s) _____________________________", move);
           scounter++;*/
        for(int i=opponent*9-9; i<opponent*9; i++){
            if((int)(shmSteinlistePTR[i].position[1])-'0'>=0) {
                opstonelist[opcount] = i;
                opcount++;
            }
            }

        int pos = rand() % opcount;
        DEBUG_PRINT("%i", pos);
        DEBUG_PRINT("%i", opcount);
        strcat(move, shmSteinlistePTR[opstonelist[pos]].position);
        DEBUG_PRINT("%s", move);
            }

  else {
       //OPhase
        if (run < 9) {
            //Switsch zum testen vn Caputuren, später löschen
            /*switch (run) {
                case 0:
                    strcat(move, "A7");
                    break;
                case 1:
                    strcat(move, "A1");
                    break;
                case 2:
                    strcat(move, "A2");
                    break;
                case 3:
                    strcat(move, "A6");
                    break;
                case 4:
                    strcat(move, "A0");
                    break;
            }*/
             int* movelist = legalMovesOphase(feld);
             int leftloop = 0;

             for(int i = 0; i < legalMoveCounter(); i++ ){
                 feld[movelist[i]] = opponent;
                 if(fastMuehle(feld,movelist[i]) > 0){
                  strcat(move, posString(movelist[i]));
                feld[movelist[i]] = 0;
                leftloop = 1;
                  break;
            }
              feld[movelist[i]] = player;
            if(fastMuehle(feld,movelist[i]) > 0){
                strcat(move, posString(movelist[i]));
                feld[movelist[i]] = 0;
                leftloop = 1;
                break;
            }
             }
             if(!leftloop){
                 r = rand() % legalMoveCounter();
                 DEBUG_PRINT("%i", legalMoveCounter());
                 strcat(move, posString(movelist[r]));
             }
        run++;
        free(movelist);

        }
        else {
            DEBUG_PRINT("MEPhase");
            int* movelist = legalMovesMEphase(feld,player,anzSteine);
            int leftloop = 0;

            if(legalMoveCounter() == 0) {
                DEBUG_PRINT("No more Moves");
                 return;
            }
            DEBUG_PRINT("MEPhase1 %i", legalMoveCounter());
            for(int i = 0; i < legalMoveCounter(); i++ ){
                feld[movelist[i]%100] = player;
                if(fastMuehle(feld,movelist[i]%100) > 0){
                    strcat(move, posString(movelist[i]/100));
                    strcat(move, ":");
                    strcat(move, posString(movelist[i]%100));
                    feld[movelist[i]%100] = 0;
                    leftloop = 1;
                    DEBUG_PRINT("MEPhase2");
                    break;
                }
                feld[movelist[i]%100] = opponent;
                if(fastMuehle(feld,movelist[i]) > 0){
                    strcat(move, posString(movelist[i]/100));
                    strcat(move, ":");
                    strcat(move, posString(movelist[i]%100));
                    feld[movelist[i]%100] = 0;
                    leftloop = 1;
                    DEBUG_PRINT("MEPhase3");
                    break;
                }
            }
            if(!leftloop){
                DEBUG_PRINT("MEPhase4");
                DEBUG_PRINT("%i", legalMoveCounter());
                r = rand() % legalMoveCounter();
                strcat(move, posString(movelist[r]/100));
                strcat(move, ":");
                strcat(move, posString(movelist[r]%100));
                DEBUG_PRINT("%s", move);
            }
            free(movelist);
            DEBUG_PRINT("MEPhase5");
        }
}


    strcpy(shmThinkerConnectorPTR->move, move);
    //shmDetatch(shmThinkerConnectorPTR->shmIdSpielfeld);
    //shmDetatch(shmThinkerConnectorPTR->shmIdSteinliste);
    //shmDetatch(shmThinkerConnectorPTR->shmIdSteinliste);
    DEBUG_PRINT("Think() ende\n\n");
}

void writeSHMDatainFeld(int* feld, Stein* shmSteinlistePTR, Spielfeld* shmSpielfeldPTR){

    for(int i=0; i<24; i++){
        feld[i]=0;
    }

    for(int y=1; y<3; y++){
        for(int i=y*9-9; i<y*9; i++){
            if((int)(shmSteinlistePTR[i].position[1])-'0'>=0){
                switch (shmSteinlistePTR[i].position[0]){
                    case 'A':
                        feld[(int)(shmSteinlistePTR[i].position[1])-'0' + 0] = y;
                        break;
                    case 'B':
                        feld[(int)(shmSteinlistePTR[i].position[1])-'0' + 8] = y;
                        break;
                    case 'C':
                        feld[(int)(shmSteinlistePTR[i].position[1])-'0' + 16] = y;
                        break;
                    default: DEBUG_PRINT("broke in writeSHMDATEFELD"); break;
                }
            }
        }
    }
    printField (feld, shmSpielfeldPTR);
}

void printField(int* feld, Spielfeld* shmSpielfeldPTR){
        printf("Capture %i piece\n", shmSpielfeldPTR->zuSchlagendeSteine);
        for(int i=0; i<24; i++){
            printf("%i:%i  ",i,feld[i]);
        }
        printf("\n");
        //Zeile 1
        printf("%c",positioner(feld, shmSpielfeldPTR, 0));
        printf("----------");
        printf("%c",positioner(feld, shmSpielfeldPTR, 1));
        printf("----------");
        printf("%c\n",positioner(feld, shmSpielfeldPTR, 2));
        //Zeile 2
        printf("|          |          |\n");
        //Zeile 3
        printf("|   %c", positioner(feld, shmSpielfeldPTR, 8));
        printf("------%c", positioner(feld, shmSpielfeldPTR, 9));
        printf("------%c   |\n", positioner(feld, shmSpielfeldPTR, 10));
        //Zeile 4
        printf("|   |      |      |   |\n");
        //Zeile 5
        printf("|   |   %c", positioner(feld, shmSpielfeldPTR, 16));
        printf("--%c", positioner(feld, shmSpielfeldPTR, 17));
        printf("--%c   |   |\n", positioner(feld, shmSpielfeldPTR, 18));
        //Zeile 6
        printf("|   |   |     |   |   |\n");
        //Zeile 7
        printf("%c---", positioner(feld, shmSpielfeldPTR, 7));
        printf("%c---", positioner(feld, shmSpielfeldPTR, 15));
        printf("%c", positioner(feld, shmSpielfeldPTR, 23));
        printf("     %c---", positioner(feld, shmSpielfeldPTR, 19));
        printf("%c---", positioner(feld, shmSpielfeldPTR, 11));
        printf("%c\n", positioner(feld, shmSpielfeldPTR, 3));
        //Zeile 8
        printf("|   |   |     |   |   |\n");
        //Zeile 9
        printf("|   |   %c", positioner(feld, shmSpielfeldPTR, 22));
        printf("--%c", positioner(feld, shmSpielfeldPTR, 21));
        printf("--%c   |   |\n", positioner(feld, shmSpielfeldPTR, 20));
        //Zeile 10
        printf("|   |      |      |   |\n");
        //Zeile 11
        printf("|   %c", positioner(feld, shmSpielfeldPTR, 14));
        printf("------%c", positioner(feld, shmSpielfeldPTR, 13));
        printf("------%c   |\n", positioner(feld, shmSpielfeldPTR, 12));
        //Zeile 12
        printf("|          |          |\n");
        //Zeile 13
        printf("%c",positioner(feld, shmSpielfeldPTR, 6));
        printf("----------");
        printf("%c",positioner(feld, shmSpielfeldPTR, 5));
        printf("----------");
        printf("%c\n",positioner(feld, shmSpielfeldPTR, 4));

}

char positioner(int* feld, Spielfeld* shmSpielfeldPTR, int pos){
    int fieldinfo = feld[pos];
    switch (fieldinfo){
        case 0:
            return '+';
        case 1:
            return '#';
        case 2:
            return '*';
        default:
            DEBUG_PRINT("NOT EXAPTABLE SOURCE: positioner");
            return '!';

    }
}

