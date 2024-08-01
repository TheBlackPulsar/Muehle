//#define DEBUG
#include "muehle.h"

extern void* shmThinkerConnector;
extern void* shmSpielerliste;

extern int player;

Spieldaten spieledatenForSHM;
Player ourplayer;
Player otherPlayer; //not a specific Player
Player* listOfPlayers; //ourPlayer is always at Postition 0
int PlayerlistPosition = 1;

int performConnect(int sock_stream, char* gameID, char* Gamekind){
    
    spieledatenForSHM.Thinker = getpid();
    spieledatenForSHM.Connector = getppid();

    DEBUG_PRINT("%s", Gamekind);
    int init = init_performConnect(sock_stream, gameID);
    if (init < 0 ) {
        perror("init_performConnect failed");
        exit(EXIT_FAILURE);
    }

    //write Structs in SHM

    //create shm for Spielerliste and store shmid in shmThinkerConnector
    spieledatenForSHM.shmIdSpielerliste = shmCreate(spieledatenForSHM.Spieleranzahl * sizeof(Player));
    shmSpielerliste = shmat(spieledatenForSHM.shmIdSpielerliste, NULL, 0);
    memcpy(shmSpielerliste, listOfPlayers, spieledatenForSHM.Spieleranzahl * sizeof(Player)); //Spielerliste added to shm
    memcpy(shmThinkerConnector, &spieledatenForSHM, sizeof(Spieldaten)); //Spieledaten added to shm

    //The following lines Can only be used together ore shmPlayerlistPTR has no use
    //Player* shmPlayerlistPTR = (Player*) shmSpielerliste;
    //DEBUG_PRINT("SHMINFO: %s", shmPlayerlistPTR[0].Spielername);

    return 0;
}

int init_performConnect(int sock_stream, char *gameID){

    int BUF = 100;
    char *buffer = malloc( sizeof(char) * BUF );
    ssize_t size = 0;

    //Spieldaten *shmThinkerConnectorPTR = (Spieldaten *)shmThinkerConnector;
    
    //Kommunikation:
    //S: + MNM Gameserver <<Gameserver Version>> accepting connections
    getServerMessage(sock_stream,buffer,BUF,size);
    formatierteAusgabe(buffer, 1);

    //S: + Happy New Year!
    getServerMessage(sock_stream,buffer,BUF,size);
    formatierteAusgabe(buffer, 2);
    
    
    //C: VERSION <<Client Version>>
    send(sock_stream,"VERSION 3.12\n",strlen("VERSION 3.12\n"),0);


    //S: + Client version accepted - please send Game-ID to join
    //S: - Client Version does not match server Version
    getServerMessage(sock_stream,buffer,BUF,size);
    if( (formatierteAusgabe(buffer, 3)) < 0){
        printf("case 2 Fehler\n");
        exit(EXIT_FAILURE);
    }


    //C: ID <<Game-ID>>  
    char destination[17] = "ID ";
    strcat(destination,gameID);
    strcat(destination,"\n");    
    send(sock_stream,destination,strlen(destination),0); 


    //S: + PLAYING  Gamekind-Name 
    //S: - Did not get the expected ID command
    //S: - Not a valid game ID
    getServerMessage(sock_stream,buffer,BUF,size);
    if(formatierteAusgabe(buffer, 4) == -1) {
        printf("case 3 Fehler\n");
        exit(EXIT_FAILURE);
    }


    //S: + << Game-Name >> // + Spielname
    getServerMessage(sock_stream,buffer,BUF,size);
    if(formatierteAusgabe(buffer, 5) == -1) {
        printf("case 4 Fehler\n");
        exit(EXIT_FAILURE);
    }


    //C: PLAYER [[ Gewünschte Spielernummer ]]
    char helper[2];
    //player = shmThinkerConnectorPTR->Spielernummer;
    char playermessage[20] = "PLAYER ";
    sprintf(helper,"%d", player-1);
    strcat(playermessage, helper);
    strcat(playermessage,"\n");
    DEBUG_PRINT("We send server: %s", playermessage);
    send(sock_stream,playermessage,strlen("PLAYER 0\n"),0);


    //S: + YOU << Spielernummer >> << Spielername >> // + YOU (0 oder 1) 
    getServerMessage(sock_stream,buffer,BUF,size);
    if(formatierteAusgabe(buffer, 6) == -1) {
        printf("case 5 Fehler\n");
        exit(EXIT_FAILURE);
    }


    //S: + TOTAL << Spieleranzahl >>
    getServerMessage(sock_stream,buffer,BUF,size);
    if(formatierteAusgabe(buffer, 7) == -1) {
        printf("case 6 Fehler\n");
        exit(EXIT_FAILURE);
    }
    
    
    //Nun kommt fur jeden der anderen Spieler die Zeile:
    //S: + << Spielernummer >> << Spielername >> << Bereit >>
    getServerMessage(sock_stream,buffer,BUF,size);
    if(formatierteAusgabe(buffer, 8) == -1) {
        printf("case 7 Fehler\n");
        exit(EXIT_FAILURE);
    }


    //S: + ENDPLAYER
    getServerMessage(sock_stream,buffer,BUF,size);
    if(formatierteAusgabe(buffer, 9) == -1) {
        printf("case 8 Fehler\n");
        exit(EXIT_FAILURE);
    }

    free(buffer);

    return 0;
}

int formatierteAusgabe(char *buffer, int messageNumber){
    //messageNumber fuer jede moegliche Server antwort

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

    //Server: - TIMEOUT Be faster next time
    switch (messageNumber){ //verschiedene messageNumbers für jeden Fall
    case 1: 
        //S: + MNM Gameserver <<Gameserver Version>> accepting connections
        printf("Aktuelle Gameserver Version: %s\n", words[3]);
        break;

    case 2: 
        //S: + Happy New Year!
        printf("%s ", words[1]);
        printf("%s ", words[2]);
        printf("%s", words[3]);
        break;

    case 3: 
        //S: + Client version accepted - please send Game-ID to join
        //S: - Client Version does not match server Version
        if(buffer[0] == '-') {
            if (strcmp(words[1],"Client") == 0){
                perror("Client Version nicht akzeptiert\n");
            } else if (strcmp(words[1],"TIMEOUT") == 0){
                perror("Client Version Time Out\n");
            } else {
                perror("Case 2 unknown Server Message\n");
            }
            exit(EXIT_FAILURE);
        }
        if(buffer[0] == '+') {
            if (strcmp(words[1],"Client") == 0){
                printf("Client Version akzeptiert\n");
            } else perror("Case 2 unknown Server Message\n");
        }
        break; 

    case 4: 
        //S: + PLAYING  Gamekind-Name 
        //S: - Did not get the expected ID command
        //S: - Not a valid game ID
        if(buffer[0] == '-') {
            if (strcmp(words[5],"expected") == 0){
                perror("Game ID Command falsch\n");
            } else if (strcmp(words[1],"TIMEOUT") == 0){
                perror("Game-ID Time Out\n");
            } else if(strcmp(words[1],"Not") == 0) {
                perror("Game-ID not valid\n");
            } else {
                perror("Case 3 unknown Server Message\n");
            }
            exit(EXIT_FAILURE);
        }
        
        if(buffer[0] == '+') {
          
            if (strcmp(words[2], "NMMorris\n") != 0){
                perror("Falsches Game Kind\n");
                exit(EXIT_FAILURE);
            } else if (strcmp(words[2],"NMMorris\n") == 0){
                printf("Aktuelles Spiel: %s", words[2]);
            } else {
                perror("Case 3 unknown Server Message\n");
            }
        }
        break;

    case 5: //S: + << Game-Name >> // + Spielname
        if(buffer[0] == '-') {
            if (strcmp(words[1],"TIMEOUT") == 0){
                perror("Case 4 Time Out\n");
            } else {
                perror("Case 4 unknown Server Message\n");
            }
            exit(EXIT_FAILURE);
        }
    
        if(buffer[0] == '+') {
            char tmp[100];
            memset(tmp, 0, sizeof(tmp));
            for (int j = 1; j < token_anzahl ; j++ ){
                strcat(tmp, words[j]);
                if(j != token_anzahl - 1) strcat(tmp, " ");
            }
            printf("Die Spiel Lobby heißt: %s", tmp);
            strcpy(spieledatenForSHM.Spielname, tmp);
        }

        break;

    case 6: 
        //S: + YOU << Spielernummer >> << Spielername >> // + YOU (0 oder 1) Player1
        //S: - No free player
        if(buffer[0] == '-') {
            if (strcmp(words[1],"TIMEOUT") == 0){
                perror("Case 5 Time Out\n");
            } else if(strcmp(words[2],"free") == 0){
                perror("Kein freier Spieler\n");
            }
            else {
                perror("Case 5 unknown Server Message\n");
            }
            exit(EXIT_FAILURE);
        }
       
        if(buffer[0] == '+') {
            if (strcmp(words[1],"YOU") == 0){
                char tmp[30];
                memset(tmp, 0, sizeof(tmp));
                for (int j = 3; j < token_anzahl; j++ ){
                    strcat(tmp, words[j]);
                    if(j != token_anzahl - 1) strcat(tmp, " ");
                }
                printf("Deine Spielernummer ist %i und dein Spielername lautet %s",atoi(words[2])+1, tmp);
                spieledatenForSHM.Spielernummer = atoi(words[2])+1;
                ourplayer.Spielernummer = spieledatenForSHM.Spielernummer;
                strcpy(spieledatenForSHM.Spielername, tmp);
                strcpy(ourplayer.Spielername, spieledatenForSHM.Spielername);
                ourplayer.Spielerbereit = 1;
            } else perror("Case 5 unknown Server Message\n");
        }
        break;

    case 7: //S: + TOTAL << Spieleranzahl >>
        if(buffer[0] == '-') {
            if (strcmp(words[1],"TIMEOUT") == 0){
                perror("Case 6 Time Out\n");
            } else {
                perror("Case 6 unknown Server Message\n");
            }
            exit(EXIT_FAILURE);
        }
      
        if(buffer[0] == '+') {
            if (strcmp(words[1],"TOTAL") == 0){
                printf("Anzahl der Spieler: %i\n", atoi(words[2]));
                spieledatenForSHM.Spieleranzahl = atoi(words[2]);
                listOfPlayers = malloc(spieledatenForSHM.Spieleranzahl * sizeof(Player));
                listOfPlayers[0] = ourplayer;
            } else perror("Case 6 unknown Server Message\n");
        }
        break;

    case 8: //S: + << Spielernummer >> << Spielername >> << Bereit >>
        if(buffer[0] == '-') {
            if (strcmp(words[1],"TIMEOUT") == 0){
                perror("Case 7 Time Out\n");
            } else {
                perror("Case 7 unknown Server Message\n");
            }
            exit(EXIT_FAILURE);
        }


        char tmp[30];
        memset(tmp, 0, sizeof(tmp));
        for (int j = 2; j < token_anzahl - 1; j++ ){
            strcat(tmp, words[j]);
            if(j != token_anzahl - 2) strcat(tmp, " ");
        }
    
        if (atoi(words[token_anzahl-1]) == 0){ // words == 0\n oder auch 1\n maybe
            printf("Spieler %i (%s) ist noch nicht bereit\n",atoi(words[1])+1,tmp);

        } else {
            printf("Spieler %i (%s) ist bereit\n",atoi(words[1])+1,tmp);

        }

        strcpy(otherPlayer.Spielername, tmp);
        otherPlayer.Spielernummer = atoi(words[1]+1);
        otherPlayer.Spielerbereit = atoi(words[token_anzahl-1]);

        listOfPlayers[PlayerlistPosition] = otherPlayer;
        PlayerlistPosition++;

        break;

    case 9: //S: + ENDPLAYERS
        if(buffer[0] == '-') {
            if (strcmp(words[1],"TIMEOUT") == 0){
                perror("Case 8 Time Out\n");
            } else {
                perror("Case 8- unknown Server Message\n");
            }
            exit(EXIT_FAILURE);
        }

        if(buffer[0] == '+') {
            if (strcmp(words[1],"ENDPLAYERS\n") == 0){
                printf("Lasset das Spiel beginnen\n\n\n");
            } else perror("Case 8+ unknown Server Message\n");
        }
        break;

    default:
        break;
    }

    return 0;
}

int getServerMessage(int sock_stream, char *buffer, int charanzahl, ssize_t size){

    int i= 0;
    char *var = malloc(sizeof(char));
    
    
    memset(buffer, 0, sizeof(*buffer));

    if ( (size = recv(sock_stream, var, 1, 0)) == -1 ) exit(EXIT_FAILURE);

    buffer[i] = *var;

    for(i= 1; i < charanzahl; i++){
        size = recv(sock_stream, var, 1, 0);
        buffer[i] = *var;
        if (buffer[i] == '\n') break;
    }

    i++;
    for (int j = i; j<charanzahl; j++){
        buffer[j] = '\0';
    }

    
    free(var);
    return 0;
}
