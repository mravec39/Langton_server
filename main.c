#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ws2tcpip.h>
#include <winsock2.h>

// Funkcie pre odosielanie suboru klientovi
void sendFileToClient(int clientSocket, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char buffer[256];
    size_t bytesRead;
// nacitanie súboru do buffra a odosielanie klientovi
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (send(clientSocket, buffer, bytesRead, 0) < 0) {
            perror("Error sending file to client");
            fclose(file);
            return;
        }
        break;
    }

    fclose(file);
}

// Funkcia pre prijatie suboru od klienta
void receiveFileFromClient(const char* content, const char* fileName) {
    char fullPath[256];
    snprintf(fullPath, sizeof(fullPath), "./%s", fileName);

    FILE* file = fopen(fullPath, "w");
    if (file != NULL) {
        fprintf(file, "%s", content);
        fclose(file);
        printf("File '%s' saved successfully.\\n", fullPath);
    } else {
        perror("Error saving file");
    }
}

int main(int argc, char* argv[]) {

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Error initializing Winsock");
        return 1;
    }

    int sockfd, newsockfd;
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    char buffer[256];

    if (argc < 3) {
        fprintf(stderr, "usage %s port filename\n", argv[0]);
        return 1;
    }

    // Inicializácia štruktúry adresy servera
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));

    //Vytvorenie socketu
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        return 1;
    }

    // Priradenie socketu k adrese
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error binding socket address");
        return 2;
    }


    listen(sockfd, SOMAXCONN);

    printf("Waiting for a connection...\n");

    while (1) {
        cli_len = sizeof(cli_addr);

        // Prijatie spojenia
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &cli_len);
        if (newsockfd < 0) {
            perror("ERROR on accept");
            continue;
        }

        printf("Connection accepted\n");

        memset(buffer, 0, sizeof(buffer));
        // Prijatie príkazu od klienta
        n = recv(newsockfd, buffer, sizeof(buffer) - 1, 0);

        if (n <= 0) {
            perror("Error receiving command from client");
        } else {
            char command = buffer[0];
            printf("%c - ", command);

            if (command == 's' || command == 'S') {
                printf("Client requested to load the world from the server.\n");

                // Odoslanie souboru klientovi
                sendFileToClient(newsockfd, "received_file.txt");
            } else if (command == 'u' || command == 'U') {
                printf("Client requested to upload a file to the server.\n");

                // Prijatie souboru od klienta
                memset(buffer, 0, sizeof(buffer));
                recv(newsockfd, buffer, sizeof(buffer) - 1, 0);
                receiveFileFromClient(buffer, "received_file.txt");
            } else {
                printf("Unknown command received from the client.\n");
            }
        }
    }
    WSACleanup();
    return 0;
}