#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


char* saveToFile(const char* fileName, const char* content) {
    char fullPath[256];
    snprintf(fullPath, sizeof(fullPath), "./%s", fileName);

    FILE* file = fopen(fullPath, "w");
    if (file != NULL) {
        fprintf(file, "%s", content);
        fclose(file);
        printf("File '%s' saved successfully.\n", fullPath);
        return strdup(fullPath);
    } else {
        perror("Error saving file");
        return NULL;
    }
}

int main(int argc, char* argv[]) {
    int sockfd, newsockfd;
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    char buffer[256];

    if (argc < 3) {
        fprintf(stderr, "usage %s port filename\n", argv[0]);
        return 1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
        return 1;
    }

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error binding socket address");
        return 2;
    }

    listen(sockfd, SOMAXCONN);

    printf("Waiting for a connection...\n");

    cli_len = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &cli_len);
    if (newsockfd < 0)
    {
        perror("ERROR on accept");
        return 3;
    }

    printf("Connection accepted\n");


    memset(buffer, 0, sizeof(buffer));
    n = recv(newsockfd, buffer, sizeof(buffer) - 1, 0);

    char currentDir[256];
    if (getcwd(currentDir, sizeof(currentDir)) != NULL) {
        printf("Current working directory: %s\n", currentDir);
    } else {
        perror("Error getting current working directory");
    }

    if (n <= 0) {
        perror("Error receiving file content");
    } else {
        char* filePath = saveToFile(argv[2], buffer);
        if (filePath != NULL) {
            printf("File saved at: %s\n", filePath);
            free(filePath);
        }
    }

    close(newsockfd);
    close(sockfd);
    return 0;
}
