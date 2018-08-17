#include "client.h"

#if 0

static int connect_to_server_socket(const char *hostname, const char *port)
{
    int sock_fd, rv;
    struct addrinfo hints, *clientinfo, *p;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    if ((rv = getaddrinfo(hostname, port, &hints, &clientinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }
    for (p = clientinfo; p != NULL; p = p->ai_next) {
        if ((sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket");
            continue ;
        }
        if (connect(sock_fd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sock_fd);
            perror("client: connect");
            continue ;
        }
        break ;
    }
    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        exit(1);
    }
    inet_ntop(p->ai_family, get_ip_addr((struct sockaddr *)p->ai_addr), s, sizeof(s);
    printf("client: conecting to %s\n", s);
    freeaddrinfo(clientinfo);

    return sock_fd;
}

static void communicating_with_server(int sock_fd)
{
    int bytes;
    char *p;
    char buf[MAXDATASIZE];

    printf("Enter message (for exit 'q'): ");
    while (1) {
        fgets(buf, MAXDATASIZE, stdin);
        if ((p = strchr(buf, '\n')))
            *p = '\0';
        if (!strcmp(buf, "q"))
            return ;
        if ((bytes = send(sock_fd, buf, strlen(buf), 0)) == -1) {
            perror("send");
            exit(1);
        }
        printf("client: sent %d bytes\nMessage: '%s'\n", bytes, buf);
        if ((bytes = recv(sock_fd, buf, MAXDATASIZE - 1, 0)) == -1) {
            perror("recv");
            exit(1);
        }
        buf[bytes] = '\0';
        printf("client: received %d bytes\nMessage '%s'\n", bytes, buf);
    }
}

int main (int argc, char *argv[])
{
    int server_fd;

    if (argc != 3) {
        fprintf(stderr, "Usage: client hostname port\n");
        return 1;
    }
    server_fd = connect_to_server_socket(argv[1], argv[2]);
    communicating_with_server(server_fd);
    close(server_fd);

    return 0;
}

#else

int main(int argc, char *argv[])
{
    int server_sock_fd, rv, num_bytes;
    int yes = 1;
    char *pstr;
    struct addrinfo hints, *servinfo, *p;
    char buf[MAXDATASIZE];

    if (argc != 3) {
        fprintf(stderr, "usage: client hostname port\n");
        return 1;
    }
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((server_sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket");
            continue ;
        }
        break ;
    }
    if (p == NULL) {
        fprintf(stderr, "client: failed to bind soket\n");
        return 1;
    }
    printf("Enter message (for exit 'q'): ");
    while (1) {
        fgets(buf, MAXDATASIZE, stdin);
        if ((pstr = strchr(buf, '\n')))
            *pstr = '\0';
        if (!strcmp(buf, "q"))
            break ;
        num_bytes = sendto(server_sock_fd, buf, strlen(buf), 0, p->ai_addr, p->ai_addrlen);
        if ((num_bytes) == -1) {
            perror("client: sendto");
            return 1;
        }
        printf("client: sent %d bytes to %s\nMessage '%s'\n", num_bytes, argv[1], buf);
        num_bytes = recvfrom(server_sock_fd, buf, MAXDATASIZE - 1, 0, p->ai_addr, &(p->ai_addrlen));
        if ((num_bytes) == -1) {
            perror("client: recvfrom");
            return 1;
        }
        buf[num_bytes] = '\0';
        printf("client: received %d bytes to %s\nMessage '%s'\n", num_bytes, argv[1], buf);
    }
    freeaddrinfo(servinfo);
    close(server_sock_fd);

    return 0;
}


#endif