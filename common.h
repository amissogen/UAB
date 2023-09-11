#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h> //pel INADDR_ANY
#include <sys/socket.h> //per la creació de sockets
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define MIN_SERVER_SCORE 15
#define MAX_SERVER_SCORE 20
#define MAX_CARD_VALUE 10
#define DEFAULT_SERVER_IP "172.26.0.1"
#define DEFAULT_SERVER_PORT 8080

typedef struct
{
    char result;
    int player_score;
    int server_score;
} GameHistory;
