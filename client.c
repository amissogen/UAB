#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>



int main()
{
    struct sockaddr_in address;
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    char *message;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    int option;
    while (1)
    {
        printf("Menú principal:\n");
        printf("1. Test conexión\n");
        printf("2. Jugar a 21 Blackjack\n");
        printf("3. Salir\n");
        printf("Opción: ");
        scanf("%d", &option);

        switch (option)
        {
        case 1:
            message = "test";
            send(sock, message, strlen(message), 0);
            read(sock, buffer, 1024);
            printf("Recibido: %s\n", buffer);
            break;
        case 2:
            message = "BLACKJACK";
            send(sock, message, strlen(message), 0);
            int playing = 1;
            char action[2];
            while (playing)
            {
                memset(buffer, 0, sizeof(buffer)); // Limpia el buffer
                read(sock, buffer, 1024);
                printf("%s\n", buffer);

                if (buffer[0] == 'W' || buffer[0] == 'L')
                {
                    char *token = strtok(buffer, ":");
                    
               
                    playing = 0;
                    continue;
                }

                printf("¿Quieres otra carta? (H/S): ");
                scanf("%s", action);

                // Traduce las opciones del menú a las palabras que el servidor entiende
                if (action[0] == 'H' || action[0] == 'h')
                {
                    send(sock, "H", 3, 0);
                }
                else if (action[0] == 'S' || action[0] == 's')
                {
                    send(sock, "S", 5, 0);
                }
            }

            break;
        case 3:
            close(sock);
            return 0;
        default:
            printf("Opción inválida\n");
            break;
        }
    }

    close(sock);
    return 0;
}
