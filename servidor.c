#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h> //pel INADDR_ANY
#include <sys/socket.h> //per la creació de sockets

typedef struct {
    char result;
    int player_score;
    int server_score;
} GameHistory;

GameHistory gameHistory[100];
int playerIndex = 0;

int play_blackjack(int socket) {
    int player_total = 0, server_total = 0, card;
    char buffer[1024] = {0};
    
    for (int i = 0; i < 2; i++) {
        card = rand() % 10 + 1;
        player_total += card;
        card = rand() % 10 + 1;
        server_total += card;
    }
    
    while (1) {
        memset(buffer, 0, sizeof(buffer));  
        snprintf(buffer, sizeof(buffer), "Puntuació actual: %d", player_total);
        send(socket, buffer, strlen(buffer), 0);
        
        memset(buffer, 0, sizeof(buffer));
        read(socket, buffer, 1024);
        
        if (strcmp(buffer, "S") == 0) {
            card = rand() % 10 + 1;
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

    // Genera un número aleatori entre 15 i 20 per decidir quan el servidor es
    int server_ia = rand() % 6 + 15;

    while (server_total < server_ia) {
        card = rand() % 10 + 1;
        server_total += card;
    }
    
    char result_message[1024];
    if (server_total > 21 || player_total > server_total) {
        snprintf(result_message, sizeof(result_message), "Victoria! La teva puntuació ha estat de:%d i la del servidor de:%d", player_total, server_total);
        gameHistory[playerIndex].result = 'V';
    } else {
        snprintf(buffer, sizeof(buffer), "Derrota. La teva puntuació ha estat de:%d i la del servidor de:%d", player_total, server_total);
        gameHistory[playerIndex].result = 'D';
    }
    gameHistory[playerIndex].player_score = player_total;
    gameHistory[playerIndex].server_score = server_total;
    playerIndex++;
    send(socket, result_message, strlen(result_message), 0);
    return 0;
}

void send_history(int socket) {
    int wins = 0, losses = 0;
    for (int i = 0; i < playerIndex; i++) {
        if (gameHistory[i].result == 'V') {
            wins++;
        } else {
            losses++;
        }
    }
    char buffer[1024] = {0};
    snprintf(buffer, sizeof(buffer), "Victories: %d, Derrotes: %d\n", wins, losses);
    send(socket, buffer, strlen(buffer), 0);
    return 0;
}


int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket < 0) {
            perror("Server accept failed");
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            close(server_fd);

            while (1) {
                memset(buffer, 0, sizeof(buffer));
                read(new_socket, buffer, 1024);

                if (strcmp(buffer, "test") == 0) {
                    send(new_socket, "OK", 2, 0);
                } else if (strcmp(buffer, "BLACKJACK") == 0) {
                    play_blackjack(new_socket);
                } else if (strcmp(buffer, "GET_HISTORY") == 0) {
                    send_history(new_socket);
                }
            }

            exit(0);
        } else {
            close(new_socket);
        }
    }

    return 0;
}
