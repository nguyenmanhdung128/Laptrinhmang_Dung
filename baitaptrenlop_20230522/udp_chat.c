#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024

void error(const char *message) {
    perror(message);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <server_ip> <server_port> <client_port>\n", argv[0]);
        exit(1);
    }

    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t server_len = sizeof(server_addr);
    char buffer[MAX_BUFFER_SIZE];

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("Error opening socket");

    // Clear server address structure
    memset(&server_addr, 0, sizeof(server_addr));

    // Set server address parameters
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    // Clear client address structure
    memset(&client_addr, 0, sizeof(client_addr));

    // Set client address parameters
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = INADDR_ANY;
    client_addr.sin_port = htons(atoi(argv[3]));

    // Bind socket to client address
    if (bind(sockfd, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0)
        error("Error binding");

    while (1) {
        printf("Client: ");
        fgets(buffer, MAX_BUFFER_SIZE, stdin);

        // Send message to server
        if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, server_len) < 0)
            error("Error sending to server");

        // Receive response from server
        int received_bytes = recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &server_len);
        if (received_bytes < 0)
            error("Error receiving from server");

        buffer[received_bytes] = '\0';
        printf("Server: %s\n", buffer);
    }

    // Close socket
    close(sockfd);

    return 0;
}