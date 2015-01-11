/* client.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* Headerfiles für UNIX/Linux */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <string>

#define PORT 1234
#define RCVBUFSIZE 8192

/* Funktion gibt aufgetretenen Fehler aus und
 * beendet die Anwendung. */
static void error_exit(std::string errorMessage) {
    fprintf(stderr, "%s: %s\n", errorMessage.c_str(), strerror(errno));
    exit(EXIT_FAILURE);
}

int main( int argc, char *argv[]) {
    struct sockaddr_in server;
    struct hostent *host_info;
    unsigned long addr;
    int sock;
    char *echo_string;
    int echo_len;

 /* Sind die erforderlichen Kommandozeilenargumente vorhanden? */
    if (argc < 3)
        error_exit("usage: client server-ip echo_word\n");

    /* Erzeuge das Socket. */
    sock = socket( AF_INET, SOCK_STREAM, 0 );


    if (sock < 0)
        error_exit( "Fehler beim Anlegen eines Sockets");

    /* Erzeuge die Socketadresse des Servers.
     * Sie besteht aus Typ, IP-Adresse und Portnummer. */
    memset( &server, 0, sizeof (server));
    if ((addr = inet_addr( argv[1])) != INADDR_NONE) {
        /* argv[1] ist eine numerische IP-Adresse. */
        memcpy( (char *)&server.sin_addr, &addr, sizeof(addr));
    }
    else {
        /* Für den Fall der Fälle: Wandle den
         * Servernamen bspw. "localhost" in eine IP-Adresse um. */
        host_info = gethostbyname(argv[1]);
        if (NULL == host_info)
            error_exit("Unbekannter Server");
        /* Server-IP-Adresse */
        memcpy( (char *)&server.sin_addr,
                host_info->h_addr, host_info->h_length );
    }
    /* IPv4-Verbindung */
    server.sin_family = AF_INET;
    /* Portnummer */
    server.sin_port = htons( PORT );

    /* Baue die Verbindung zum Server auf. */
    if(connect(sock,(struct sockaddr*)&server,sizeof(server)) <0)
        error_exit("Kann keine Verbindung zum "
                   "Server herstellen");

    /* Zweites Argument wird als "echo" beim Server verwendet. */
    echo_string = argv[2];
    /* Länge der Eingabe */
    echo_len = strlen(echo_string);

    /* den String inkl. Nullterminator an den Server senden */
    if (send(sock, echo_string, echo_len, 0) != echo_len)
        error_exit("send() hat eine andere Anzahl"
                   " von Bytes versendet als erwartet !!!!");
                   
    if (send(sock, "\n", 2, 0) != 2)
        error_exit("send() hat eine andere Anzahl"
                   " von Bytes versendet als erwartet !!!!");
    

    // printf("test");
    
    // Gibt die komplette Antwort aus und bleibt dann in Endlosschleife.
    int rec_value;
    char buf[RCVBUFSIZE];
    while((rec_value = read(sock, buf, RCVBUFSIZE))){
        if(rec_value < 0){
            perror("reading stream message");
            exit(1);
        } else
            write(1,buf,rec_value);
    }
    
    // Gibt den ersten Antwortbuchstaben aus, dann Ende.
    /* char echo_buffer[RCVBUFSIZE];
    int recv_size = 0;
    if((recv_size = recv(sock, echo_buffer, RCVBUFSIZE, 0)) < 0) {
        error_exit("Fehler bei recv()");
    }
    echo_buffer[recv_size] = '\0';
    printf("Nachrichten vom Client : %s ", echo_buffer);*/
     
    // Gibt erst den ersten Antwortbuchstaben aus, dann die restliche Antwort und bleibt dann in Endlosschleife.
    /*char buffer[RCVBUFSIZE];
    int recv_count;
    memset(buffer, 0, sizeof(buffer));
    while ((recv_count = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        printf("Nachrichten vom Client : %s ",buffer);
    }*/

   close(sock);

    return EXIT_SUCCESS;
}
