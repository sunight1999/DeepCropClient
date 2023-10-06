#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>

#define HOME getenv("HOME")
#define SERVER "172.23.203.129"
#define PORT 8080

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Failed to create socket");
        return -1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to connect server");
        close(sockfd);
        return -1;
    }

    char filePath[128] = { 0, };
    sprintf(filePath, "%s/data/noboduck.png", HOME);
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open image file" << std::endl;
        close(sockfd);
        return -1;
    }

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    char* image = new char[fileSize];
    if (!file.read(image, fileSize)) {
        std::cerr << "Failed to read image file" << std::endl;
        close(sockfd);
        delete[] image;
        return -1;
    }

    int sent = send(sockfd, (char*)&fileSize, sizeof(int), 0);
    if (sent < 0) {
        std::cerr << "Failed to send file size" << std::endl;
        close(sockfd);
        delete[] image;
        return -1;
    }

    sent = send(sockfd, image, fileSize, 0);
    if (sent < 0) {
        std::cerr << "Failed to send image" << std::endl;
        close(sockfd);
        delete[] image;
        return -1;
    }

    char buf[1024] = { 0, };
    int read = recv(sockfd, buf, 1024, 0);
    if (read < 0) {
        std::cerr << "Failed to read message" << std::endl;
    }

    close(sockfd);
    delete[] image;

    return 0;
}