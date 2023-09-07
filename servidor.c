#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int play_blackjack(int socket) {
    int player_total = 0, server_total = 0, card;
    char buffer[1024] = {0};
    printf("partida nova");
    
    for (int i = 0; i < 2; i++) {
        card = rand() % 10 + 1;
        player_total += card;
        card = rand() % 10 + 1;
        server_total += card;
    }
    
    while (1) {
        memset(buffer, 0, sizeof(buffer));  // Limpia el buffer
        snprintf(buffer, sizeof(buffer), "Tu puntuación: %d", player_total);
        send(socket, buffer, strlen(buffer), 0);
        
        memset(buffer, 0, sizeof(buffer));
        read(socket, buffer, 1024);
        if (strcmp(buffer, "H") == 0) {
            card = rand() % 10 + 1;
            player_total += card;
            if (player_total > 21) {
                send(socket, "L", 11, 0);
                return 0;
            }
        } else if (strcmp(buffer, "S") == 0) {
            break;
        }
    }
    
    while (server_total < 17) {
        card = rand() % 10 + 1;
        server_total += card;
    }
    
    if (server_total > 21 || player_total > server_total) {
        send(socket, "W", 10, 0);
    } else {
        send(socket, "L", 11, 0);
    }
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
    
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    
    while(1) {
        read(new_socket, buffer, 1024);
        
        if (strcmp(buffer, "A") == 0) {
            send(new_socket, "Hola", 4, 0);
        } else if (strcmp(buffer, "B") == 0) {
            send(new_socket, "Adios", 5, 0);
        } else if (strcmp(buffer, "test") == 0) {
            send(new_socket, "OK", 2, 0);
        } else if (strcmp(buffer, "BLACKJACK") == 0) {
            play_blackjack(new_socket);
        }
    }
    
    return 0;
}
