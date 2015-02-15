/*
 * Author: Marc Pfeifer
 * Based on code from the book: "C von A bis Z", Author: Jürgen Wolf, Rheinwerk Computing, http://openbook.rheinwerk-verlag.de/c_von_a_bis_z/025_c_netzwerkprogrammierung_006.htm
 */

// Includes:
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

// Defines:
#define PORT 1234
#define RCVBUFSIZE 8192  // Receivebuffer size.

/* 
 * Prints current error and closes the application.
 */
static void error_exit(char *errorMessage) {
    fprintf(stderr, "%s: %s\n", errorMessage, strerror(errno));
    exit(EXIT_FAILURE);
}

/*
 * The main method.
 */
int main( int argc, char *argv[]) {
    
    // Variables:
    struct sockaddr_in server;
    struct hostent *host_info;
    unsigned long addr;
    int sock;
    char *echo_string;
    int echo_len;

    // Check the calling arguments.
    if (argc < 3)
        error_exit("usage: client server-ip echo_word\n");

    // Create a socket.
    sock = socket( AF_INET, SOCK_STREAM, 0 );

    // Check if the creation of the socket was successful.
    if (sock < 0)
        error_exit( "Fehler beim Anlegen eines Sockets");

    // Create the socket address for the server (IP and Port):
    memset( &server, 0, sizeof (server));
    // Check the ip from the calling arguments.
    if ((addr = inet_addr( argv[1])) != INADDR_NONE) {
        // Set the IP address.
        memcpy( (char *)&server.sin_addr, &addr, sizeof(addr));
    }
    else {
        // Conver the server name (e.g. localhost) into an IP if necessary.
        host_info = gethostbyname(argv[1]);
        if (NULL == host_info)
            error_exit("Unbekannter Server");
        // Set the IP address.
        memcpy( (char *)&server.sin_addr,
                host_info->h_addr, host_info->h_length );
    }
    // Set the connection type to IPv4.
    server.sin_family = AF_INET;
    // Set the port.
    server.sin_port = htons( PORT );

    // Connect to the server.
    if(connect(sock,(struct sockaddr*)&server,sizeof(server)) <0)
        error_exit("Kann keine Verbindung zum Server herstellen");

    // Read the echo string.
    echo_string = argv[2];
    // Extract the length.
    echo_len = strlen(echo_string);

    // Send the sting including a newline to the server.
    if (send(sock, echo_string, echo_len, 0) != echo_len)
        error_exit("send() hat eine andere Anzahl von Bytes versendet als erwartet !!!!");           
    if (send(sock, "\n", 2, 0) != 2)
        error_exit("send() hat eine andere Anzahl von Bytes versendet als erwartet !!!!");

        
    // 3 Methods to receive the answer from the server:
    
    // 1. Method
    // Receives the complete answer and stays in an infinite loop afterwards.
    /* int rec_value;
    char buf[RCVBUFSIZE];
    while (rec_value = read(sock,buf,RCVBUFSIZE)){
        if(rec_value < 0){
            perror("reading stream message");
            exit(1);
        } else
            write(1,buf,rec_value);
    }*/
    
    // 2. Method
    // Receives the one letter of the answer than stops the programm.
    /* char echo_buffer[RCVBUFSIZE];
    int recv_size = 0;
    if((recv_size = recv(sock, echo_buffer, RCVBUFSIZE, 0)) < 0) {
        error_exit("Fehler bei recv()");
    }
    echo_buffer[recv_size] = '\0';
    printf("Nachrichten vom Client : %s ", echo_buffer);*/
     
    // 3. Method
    // Receives the first letter of the answer then the rest and stays in an infinite loop afterwards.
    /*char buffer[RCVBUFSIZE];
    int recv_count;
    memset(buffer, 0, sizeof(buffer));
    while ((recv_count = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        printf("Nachrichten vom Client : %s ",buffer);
    }*/

    // Close the socket.
    close(sock);

    // Close the programm.
    return EXIT_SUCCESS;
}