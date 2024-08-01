//#define DEBUG
#include <stdio.h>
#include "muehle.h"

int counter = 0;

void findNeighbour(int pos, int *neigh){


        /* switch (pos) {
            case 0:
                return (int[]) {1,7,-1,-1};
            case 1:
                return (int[]) {0,2,8,-1};
            case 2:
                return (int[]) {1,3,-1,-1};
            case 3:
                return (int[]) {2,4,11,-1};
            case 4:
                return (int[]) {3,5,-1,-1};
            case 5:
                return (int[]) {4,6,13,-1};
            case 6:
                return (int[]) {5,7,-1,-1};
            case 7:
                return (int[]) {0,6,15,-1};
            case 8:
                return (int[]) {9,15,-1,-1};
            case 9:
                return (int[]) {1,8,10,17};
            case 10:
                return (int[]) {9,11,-1,-1};
            case 11:
                return (int[]) {3,10,13,19};
            case 12:
                return (int[]) {11,13,-1,-1};
            case 13:
                return (int[]) {5,12,14,21};
            case 14:
                return (int[]) {13,15,-1,-1};
            case 15:
                return (int[]) {7,8,14,23};
            case 16:
                return (int[]) {17,23,-1,-1};
            case 17:
                return (int[]) {9,16,18,-1};
            case 18:
                return (int[]) {17,19,-1,-1};
            case 19:
                return (int[]) {11,18,20,-1};
            case 20:
                return (int[]) {19,21,-1,-1};
            case 21:
                return (int[]) {13,20,22,-1};
            case 22:
                return (int[]) {21,23,-1,-1};
            case 23:
                return (int[]) {15,16,22,-1};
            default:
                return NULL;
        }*/

        if( pos > 23 || pos < 0) return;

        int square = pos/8;



        if(pos%2 == 0){
            neigh[0] = (pos+7)%8 + 8*square;
            neigh[1] = pos + 1;
            neigh[2] = -1;
            neigh[3] = -1;
        }
        if(pos%2 == 1){

            neigh[0] = pos-1;
            neigh[1] = (pos+1)%8 + 8*square;
            switch(square){
                case 0:
                    neigh[2] = pos + 8;
                    neigh[3] = -1;
                    break;
                case 1:
                    neigh[2] = pos + 8;
                    neigh[3] = pos - 8;
                    break;
                case 2:
                    neigh[2] = pos - 8;
                    neigh[3] = -1;
                    break;
                default:
                DEBUG_PRINT("Error at findNeighbour");
            }
        }

    }

int fastMuehle(int* feld, int pos) {

    int millcount = 0;
    DEBUG_PRINT("fastMuhle");
    switch (pos) {
        case 0:
            if (feld[0] == feld[1] && feld[0] == feld[2]) millcount++;
            if (feld[0] == feld[6] && feld[0] == feld[7]) millcount++;
            return millcount;
        case 1:
            if (feld[1] == feld[0] && feld[1] == feld[2])  millcount++;
            if (feld[1] == feld[9] && feld[1] == feld[17]) millcount++;
            return millcount;
        case 2:
            if (feld[2] == feld[0] && feld[2] == feld[1]) millcount++;
            if (feld[2] == feld[3] && feld[2] == feld[4]) millcount++;
            return millcount;
        case 3:
            if (feld[3] == feld[2] && feld[3] == feld[4]) millcount++;
            if (feld[3] == feld[11] && feld[3] == feld[19]) millcount++;
            return millcount;
        case 4:
            if (feld[4] == feld[2] && feld[4] == feld[3]) millcount++;
            if (feld[4] == feld[5] && feld[4] == feld[6]) millcount++;
            return millcount;
        case 5:
            if (feld[5] == feld[4] && feld[5] == feld[6]) millcount++;
            if (feld[5] == feld[13] && feld[5] == feld[21]) millcount++;
            return millcount;
        case 6:
            if (feld[6] == feld[0] && feld[6] == feld[7]) millcount++;
            if (feld[6] == feld[4] && feld[6] == feld[5]) millcount++;
            return millcount;
        case 7:
            if (feld[7] == feld[0] && feld[7] == feld[6]) millcount++;
            if (feld[7] == feld[15] && feld[7] == feld[23]) millcount++;
            return millcount;
        case 8:
            if (feld[8] == feld[9] && feld[8] == feld[10]) millcount++;
            if (feld[8] == feld[14] && feld[8] == feld[15]) millcount++;
            return millcount;
        case 9:
            if (feld[9] == feld[8] && feld[9] == feld[10]) millcount++;
            if (feld[9] == feld[1] && feld[9] == feld[17]) millcount++;
            return millcount;
        case 10:
            if (feld[10] == feld[8] && feld[10] == feld[9]) millcount++;
            if (feld[10] == feld[11] && feld[10] == feld[12]) millcount++;
            return millcount;
        case 11:
            if (feld[11] == feld[10] && feld[11] == feld[2]) millcount++;
            if (feld[11] == feld[3] && feld[11] == feld[19]) millcount++;
            return millcount;
        case 12:
            if (feld[12] == feld[10] && feld[12] == feld[11]) millcount++;
            if (feld[12] == feld[13] && feld[12] == feld[14]) millcount++;
            return millcount;
        case 13:
            if (feld[13] == feld[12] && feld[11] == feld[14]) millcount++;
            if (feld[13] == feld[5] && feld[13] == feld[21]) millcount++;
            return millcount;
        case 14:
            if (feld[14] == feld[12] && feld[14] == feld[13]) millcount++;
            if (feld[14] == feld[8] && feld[14] == feld[15]) millcount++;
            return millcount;
        case 15:
            if (feld[15] == feld[8] && feld[15] == feld[14]) millcount++;
            if (feld[15] == feld[7] && feld[15] == feld[23]) millcount++;
            return millcount;
        case 16:
            if (feld[16] == feld[17] && feld[16] == feld[18]) millcount++;
            if (feld[16] == feld[22] && feld[16] == feld[23]) millcount++;
            return millcount;
        case 17:
            if (feld[17] == feld[16] && feld[17] == feld[18]) millcount++;
            if (feld[17] == feld[1] && feld[17] == feld[9]) millcount++;
            return millcount;
        case 18:
            if (feld[18] == feld[16] && feld[18] == feld[17]) millcount++;
            if (feld[18] == feld[19] && feld[18] == feld[20]) millcount++;
            return millcount;
        case 19:
            if (feld[19] == feld[18] && feld[19] == feld[20]) millcount++;
            if (feld[19] == feld[3] && feld[19] == feld[11]) millcount++;
            return millcount;
        case 20:
            if (feld[20] == feld[18] && feld[20] == feld[19]) millcount++;
            if (feld[20] == feld[21] && feld[20] == feld[22]) millcount++;
            return millcount;
        case 21:
            if (feld[21] == feld[20] && feld[21] == feld[22]) millcount++;
            if (feld[21] == feld[5] && feld[21] == feld[13]) millcount++;
            return millcount;
        case 22:
            if (feld[22] == feld[20] && feld[22] == feld[21]) millcount++;
            if (feld[22] == feld[16] && feld[22] == feld[23]) millcount++;
            return millcount;
        case 23:
            if (feld[23] == feld[16] && feld[23] == feld[22]) millcount++;
            if (feld[23] == feld[7] && feld[23] == feld[15]) millcount++;
            return millcount;
        default:
            return -1;
    }

}

char* posString(int pos) {

    switch (pos) {
        case 0:
            return "A0";
        case 1:
            return "A1";
        case 2:
            return "A2";
        case 3:
            return "A3";
        case 4:
            return "A4";
        case 5:
            return "A5";
        case 6:
            return "A6";
        case 7:
            return "A7";
        case 8:
            return "B0";
        case 9:
            return "B1";
        case 10:
            return "B2";
        case 11:
            return "B3";
        case 12:
            return "B4";
        case 13:
            return "B5";
        case 14:
            return "B6";
        case 15:
            return "B7";
        case 16:
            return "C0";
        case 17:
            return "C1";
        case 18:
            return "C2";
        case 19:
            return "C3";
        case 20:
            return "C4";
        case 21:
            return "C5";
        case 22:
            return "C6";
        case 23:
            return "C7";
        default:
            return NULL;
    }
}

int* legalMovesOphase(int* field) {

    int* moves = malloc(24*sizeof(int));
    counter = 0;

    if(!moves) return NULL;

    for(int i = 0; i < 24; i++){
        if (field[i] == 0) {
            moves[counter] = i;
            counter++;
        }

    }
    return moves;
}

int legalMoveCounter(){

    return counter;
}

int* legalMovesMEphase(int* field, int player, int stonecount){


    counter = 0;
    int* moves = malloc(96*sizeof(int));
    int neighbour[5] = {0};
    DEBUG_PRINT("LM");
    if(stonecount > 3){
        for(int i = 0; i < 24; i++){
            if (field[i] == 0) {
                findNeighbour(i, neighbour);
                DEBUG_PRINT("%i",i);
                for(int j = 0; j < 4; j++){
                    DEBUG_PRINT("lmx %i", neighbour[j]);
                    if(neighbour[j]>-1 && field[neighbour[j]] == player){
                        moves[counter] = 100*neighbour[j]+i;
                        DEBUG_PRINT("lm1 %i", moves[counter]);
                        counter++;
                    }
                }
            }
        }
    }
    else{
        int stone[3] = {-1,-1,-1};
        int count = 0;

        for(int i = 0; i < 24; i++){
            if (field[i] == player) {
                stone[count] = i;
                count++;
            }

        }
        for(int i = 0; i < 24; i++){
            if (field[i] == 0) {
                for(int j; j < 3; j++){
                    if(stone[j]>-1){
                        moves[counter] = 100*stone[j]+i;
                        counter++;
                        DEBUG_PRINT("lm2");
                    }
                }
            }
        }

    }

    return moves;
}






