#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>

void deleteDir(){
    if ((unlink("/tmp/hw7") == -1) && errno != ENOENT) {
        std::cerr<<"Error occured during removing the directory"<<"\n";
        exit(EXIT_FAILURE);
    }
}

int main() {
    deleteDir();
    int sockfd;
    if ((sockfd = socket(AF_UNIX,SOCK_STREAM,0)) == -1) {
        std::cerr<<"Error occured creating the socket"<<"\n";
        exit(EXIT_FAILURE);
    }
    struct sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, "/tmp/hw7", sizeof(addr.sun_path) - 1);
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1) {
        std::cerr<<"Error occured binding to socket"<<"\n";
    }
    if (listen(sockfd, 0) == -1) {
        std::cerr<<"Error occured during listening to socket"<<"\n";
        exit(EXIT_FAILURE);
    }
    int clientfd;
    while ((clientfd = accept(sockfd, NULL, NULL)) != -1) {
        struct msghdr msg = {nullptr};
        struct cmsghdr *cmsg;
        int myfds[sizeof(int)];  /* Contains the file descriptors to pass */
        char iobuf[256];
        struct iovec io = {
                .iov_base = iobuf,
                .iov_len = sizeof(iobuf)
        };
        union {         /* Ancillary data buffer, wrapped in a union
                              in order to ensure it is suitably aligned */
            char buf[CMSG_SPACE(sizeof(myfds))];
            struct cmsghdr align;
        } u{};
        msg.msg_iov = &io;
        msg.msg_iovlen = 1;
        msg.msg_control = u.buf;
        msg.msg_controllen = sizeof(u.buf);
        cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;
        cmsg->cmsg_len = CMSG_LEN(sizeof(int));
        if(sendmsg(clientfd, &msg, 0) == -1) {
            std::cerr<<"Error occured sending message"<<"\n";
            exit(EXIT_FAILURE);
        }
        if(close(sockfd) == -1){
            std::cerr<<"Error occured while closing the server"<<"\n";
            exit(EXIT_FAILURE);
    }
    }
}
