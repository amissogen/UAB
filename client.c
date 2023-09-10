#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h> //pel INADDR_ANY
#include <sys/socket.h> //per la creació de sockets

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
        printf("1. Test conexió\n");
        printf("2. Jugar a 21 Blackjack\n");
        printf("3. Consultar historial de partides\n");
        printf("4. Sortir\n");
        printf("Opció: ");
        scanf("%d", &option);

        switch (option)
        {
        case 1:
            message = "test";
            send(sock, message, strlen(message), 0);
            read(sock, buffer, 1024);
            printf("Rebut des del servidor: %s\n", buffer);
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

                if (buffer[0] == 'V' || buffer[0] == 'D')
                {
                    char *token = strtok(buffer, ":");
                    // Aquí podrías parsear y mostrar los detalles del resultado
                    playing = 0;
                    continue;
                }

                printf("Vols una altre carta? (S/N): ");
                scanf("%s", action);

                // Traduce las opciones del menú a las palabras que el servidor entiende
                if (action[0] == 'S' || action[0] == 's')
                {
                    send(sock, "S", 1, 0);
                }
                else if (action[0] == 'N' || action[0] == 'n')
                {
                    send(sock, "N", 1, 0);
                }
            }
            break;

        case 3:

            message = "GET_HISTORY";
            send(sock, message, strlen(message), 0);
      
                memset(buffer, 0, sizeof(buffer));
                read(sock, buffer, 1024);
                if (strcmp(buffer, "END_OF_HISTORY\n") == 0)
                {
                    break; // Sal del bucle cuando recibas el mensaje especial
                }
                printf("%s", buffer); // Imprime cada entrada del historial
       
            break;

        case 4:
            close(sock);
            return 0;
        default:
            printf("Opció invàlida\n");
            break;
        }
    }

    close(sock);
    return 0;
}
