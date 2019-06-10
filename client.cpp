#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>


int main() {
    int clientSocket;
    if ((clientSocket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        std::cerr << "Error occured during creating the socket" << "\n";
        exit(EXIT_FAILURE);
    }
    struct sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, "/tmp/hw7", sizeof(addr.sun_path) - 1);
    if (connect(clientSocket, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1) {
        std::cerr << "Error occured during creating the socket" << "\n";
        exit(EXIT_FAILURE);
    }
    std::cout<<"Hello World"<<"\n"<<"Type here your messages to send to the server"<<"\n";
    char iobuf[256];
    struct iovec io = {.iov_base = iobuf, .iov_len = sizeof(iobuf)};
    union {
        char buf[CMSG_SPACE(sizeof(int))];
        struct cmsghdr align;
    } u{};

    struct msghdr msg{};
    msg.msg_iov = &io,
    msg.msg_iovlen = 1,
    msg.msg_control = u.buf,
    msg.msg_controllen = sizeof(u.buf);
    if (recvmsg(clientSocket, &msg, 0) == -1) {
        std::cerr << "Error occured during receiving the message" << '\n';
        exit(EXIT_FAILURE);
    }
    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    int fd;
    memcpy(&fd, CMSG_DATA(cmsg), sizeof(fd));
    dup2(fd, 1);
    while (true) {
        char buffer[256];
        std::cin >> buffer;
        if (feof(stdin)) {
            break;
        }
        std::cout<<"Received message from the client : "<< buffer << "\n";
    }
    if (close(clientSocket) == -1) {
        std::cerr << "Error occured while closing the client socket" << "\n";
        exit(EXIT_FAILURE);
    }
    std::cout<<"Good bye"<<"\n";
    exit(EXIT_SUCCESS);
}
