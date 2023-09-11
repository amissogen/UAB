#include "common.h"

GameHistory gameHistory[100];
int playerIndex = 0;

int play_blackjack(int socket) {
    int player_total = 0, server_total = 0, card;
    char buffer[BUFFER_SIZE] = {0};

    // Generem la primera tirada
    card = rand() % MAX_CARD_VALUE + 1;
    player_total += card;

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), "Puntuació actual: %d", player_total);
        send(socket, buffer, strlen(buffer), 0);

        memset(buffer, 0, sizeof(buffer));
        read(socket, buffer, BUFFER_SIZE);

        if (strcmp(buffer, "S") == 0) {
            card = rand() % MAX_CARD_VALUE + 1;
            player_total += card;
            if (player_total > 21) {
                snprintf(buffer, sizeof(buffer), "Derrota. La teva puntuació ha estat de:%d i la del servidor de:%d", player_total, server_total);
                send(socket, buffer, strlen(buffer), 0);
                gameHistory[playerIndex].result = 'D';
                gameHistory[playerIndex].player_score = player_total;
                gameHistory[playerIndex].server_score = server_total;
                playerIndex++;
                return 0;
            }
        } else if (strcmp(buffer, "N") == 0) {
            break;
        }
    }

    // Genera un número aleatori entre dos constants definides per decidir quan el servidor es retira de treure cartes
    int server_ia = rand() % (MAX_SERVER_SCORE - MIN_SERVER_SCORE + 1) + MIN_SERVER_SCORE;

    while (server_total < server_ia && server_total < player_total) {
        card = rand() % MAX_CARD_VALUE + 1;
        server_total += card;
    }

    char result_message[BUFFER_SIZE];
    if (server_total > 21 || player_total > server_total) {
        snprintf(result_message, sizeof(result_message), "Victoria! La teva puntuació ha estat de:%d i la del servidor de:%d", player_total, server_total);
        gameHistory[playerIndex].result = 'V';
    } else {
        snprintf(result_message, sizeof(result_message), "Derrota. La teva puntuació ha estat de:%d i la del servidor de:%d", player_total, server_total);
        gameHistory[playerIndex].result = 'D';
    }
    gameHistory[playerIndex].player_score = player_total;
    gameHistory[playerIndex].server_score = server_total;
    playerIndex++;
    send(socket, result_message, strlen(result_message), 0);
    return 0;
}


void send_history(int socket)
{
    int wins = 0, losses = 0;
    for (int i = 0; i < playerIndex; i++)
    {
        if (gameHistory[i].result == 'V')
        {
            wins++;
        }
        else
        {
            losses++;
        }
    }
    char buffer[BUFFER_SIZE] = {0};
    snprintf(buffer, sizeof(buffer), "Victories: %d, Derrotes: %d\n", wins, losses);
    send(socket, buffer, strlen(buffer), 0);
  
}


int main(int argc, char *argv[])
{
    int server_port = DEFAULT_SERVER_PORT;
    int pid;
    int server_fd, new_socket;
    struct sockaddr_in address;   
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    if (argc > 2)
    {
        fprintf(stderr, "Ús: %s <PORT_SERVIDOR>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    else if (argc == 2)
    {
        server_port = atoi(argv[1]);
    }
  
    

    server_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    address.sin_family = AF_INET;
    address.sin_port = htons(server_port);
    address.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    listen(server_fd, 3);

    while (1)
    {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
      
         if ((pid = fork()) ==0) //child process
        {
            close(server_fd);

            while (1)
            {
                memset(buffer, 0, sizeof(buffer));
                read(new_socket, buffer, BUFFER_SIZE);

                if (strcmp(buffer, "test") == 0)
                {
                    send(new_socket, "OK", 2, 0);
                }
                else if (strcmp(buffer, "BLACKJACK") == 0)
                {
                    play_blackjack(new_socket);
                }
                else if (strcmp(buffer, "GET_HISTORY") == 0)
                {
                    send_history(new_socket);
                }
            }

            exit(0);
        }
        else
        {
            close(new_socket);
        }
    }

    return 0;
}
