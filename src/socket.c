//#define DEBUG

#include "muehle.h"

#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

pid_t child_pid = -1 ; //Global

void kill_child(int sig)
{
    printf("Die Verbindung konnte nicht hergestellt werden. Bitte sicherstellen, dass Sie im Uni-Netzwerk sind. (VPN)\n");
    kill(child_pid,SIGKILL);
}

int createSocket(char* HOST, int PORT) {

        DEBUG_PRINT("%s", HOST);

        //Erzeugen des Socket - Verbindung über TCP
        int network_socket;
        network_socket = socket(AF_INET, SOCK_STREAM, 0);
            if (network_socket < 0) {
                //Fehler beim Erzeugen des Sockets
                printf("Es ist ein Fehler beim Erzeugen des Sockets aufgetreten\n");
                exit(EXIT_FAILURE);
            } else {
                printf("Der Socket zum Verbindungsaufbau wurde erfolgreich initialisiert. Versuche Verbindungsaufbau zum Server...\n");
            } 
    
        //Auflösen des Hostname in eine IPv4 Addresse
        struct addrinfo hints;
        struct addrinfo *res, *tmp;
        char host[256];

        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_INET;

        int ret = getaddrinfo(HOST, NULL, &hints, &res);

        DEBUG_PRINT("%s", HOST);

        if (ret != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
            printf("Beim Auflösen der Addresse ist ein Fehler aufgetreten");
            exit(EXIT_FAILURE);
        }

            for (tmp = res; tmp != NULL; tmp = tmp->ai_next) {
                getnameinfo(tmp->ai_addr, tmp->ai_addrlen, host, sizeof(host), NULL, 0, NI_NUMERICHOST);
            }  

        freeaddrinfo(res);

        //Spezifikation der Adresse für den Socket
        struct sockaddr_in server_address;
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(PORT);
        //if (inet_addr(host) ==  inet_addr("10.155.92.198")) {
            server_address.sin_addr.s_addr = inet_addr(host); //Vorsicht inet_addr(host) und inet_addr(10.155.92.198)
        /*} else {
        //    exit(EXIT_FAILURE);
        }*/

        //Herstellen einer Verbindung mit dem Server
        int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
        
            if (connection_status < 0){
                //Fehler beim herstellen einer Verbindung
                printf("Ein Fehler bei der Verbindung mit dem Server ist aufgetreten\n");
                exit(EXIT_FAILURE);
            } else {
                printf("Die Verbindung zum Server wurde erfolgreich hergestellt\n");
            }
        
        return network_socket;

    exit(EXIT_FAILURE);

}
