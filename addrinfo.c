//
// Created by gcz on 2021/9/12.
//

#include "stdio.h"
#include "apue.h"
#if defined(SOLARIS)
#include "netinet/in.h"
#endif
#include "netdb.h"
#include "arpa/inet.h"
#if defined(BSD)
#include "sys/socket.h"
#include "netinet/in.h"
#endif

void print_type(struct addrinfo *aip) {

    printf(" type ");

    switch (aip->ai_socktype) {
        case SOCK_STREAM:
            printf("stream\n");
            break;
        case SOCK_DGRAM:
            printf("datagram\n");
            break;
        case SOCK_SEQPACKET:
            printf("seqpacket\n");
            break;
        case SOCK_RAW:
            printf("raw\n");
            break;
        default:
            printf("unknown (%d)", aip->ai_socktype);
    }
}

void print_family (struct addrinfo *aip) {

    printf(" family ");
    switch (aip->ai_family) {
        case AF_INET:
            printf("inet");
            break;
        case AF_INET6:
            printf("inet6");
            break;
        case AF_UNIX:
            printf("unix");
            break;
        case AF_UNSPEC:
            printf("unspec");
            break;
        default:
            printf("unknown");
    }

}
int main(int argc, char *argv[]) {

    struct addrinfo *ailist, *aip;
    struct addrinfo hint;
    struct sockaddr_in *sinp;
    const char *addr;
    int err;
    char abuf[INET_ADDRSTRLEN];

    if (argc != 3) {
        err_quit("usage: %s node name service", argv[0]);
    }

    hint.ai_flags = AI_CANONNAME;
    hint.ai_family = 0;
    hint.ai_socktype = 0;
    hint.ai_protocol = 0;
    hint.ai_addrlen = 0;
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;

    if ((err = getaddrinfo(argv[1], argv[2], &hint, &ailist)) != 0) {
        err_quit("getaddrinfo error: %s ", gai_strerror(err));
    }

//    Run->Edit configuration->Templates->Application->Workning directory
    for (aip = ailist; aip != NULL; aip = aip->ai_next) {
        print_type(aip);
        print_family(aip);
        printf("\n\thost %s", aip->ai_canonname?aip->ai_canonname:"-");
        if (aip->ai_family == AF_INET) {
            sinp = (struct sockaddr_in *)aip->ai_addr;
            addr = inet_ntop(AF_INET, &sinp->sin_addr, abuf, INET_ADDRSTRLEN);
            printf(" address %s", addr?addr:"unknown");
            printf(" port %d ", ntohs(sinp->sin_port));
        }
    }

}