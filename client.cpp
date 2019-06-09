#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>

#define SOCKET_NAME "/tmp/hw7"

char message[256];


int receive_fd(int socket) {
    char iobuf[256];
    struct iovec io = {.iov_base = iobuf, .iov_len = sizeof(iobuf)};
    union {
        char buf[CMSG_SPACE(sizeof(int))];
        struct cmsghdr align;
    } u{};

    struct msghdr msg{};
    msg.msg_iov = &io,
    msg.msg_iovlen= 1,
    msg.msg_control = u.buf,
    msg.msg_controllen = sizeof(u.buf);
    if (recvmsg(socket, &msg, 0) == -1) {
        std::cerr<<"Error occured during receiving the message"<<'\n';
        exit(EXIT_FAILURE);
    }
    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    int fd;
    memcpy(&fd, CMSG_DATA(cmsg), sizeof(fd));
    return fd;
}

int main() {
    int clientSocket;
    if ((clientSocket = socket(AF_UNIX, SOCK_STREAM, 0))== -1) {
        std::cerr<<"Error occured during creating the socket"<<"\n";
        exit(EXIT_FAILURE);
    }
    struct sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_NAME, sizeof(addr.sun_path) - 1);
    if (connect(clientSocket, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1) {
        std::cerr<<"Error occured during creating the socket"<<"\n";
        exit(EXIT_FAILURE);
    }

    std::cout<<"Print something"<<"\n";
    std::cin>>message;
    dup2(receive_fd(clientSocket), 1);
    while (true) {
        char buffer[256];
        std::cin>>buffer;
        if (feof(stdin)) {
            break;
        }
        std::cout<<message<<": <<"<<buffer<<"\n";
    }
    if(close(clientSocket) == -1){
        std::cerr<<"Error occured while closing the client socket"<<"\n";
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}