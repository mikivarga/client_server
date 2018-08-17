#include "client.h"

static int connect_to_server_socket(char * hostname)
{
    int sock_fd, rv;
    struct addrinfo hints, *clientinfo, *p;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if ((rv = getaddrinfo(hostname, PORT, &hints, &clientinfo)) != 0) {
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
    inet_ntop(p->ai_family, get_ip_addr((struct sockaddr *)p->ai_addr), s, sizeof(s));
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
        printf("client: sent %d bytes '%s'\n", bytes, buf);
        if ((bytes = recv(sock_fd, buf, MAXDATASIZE - 1, 0)) == -1) {
            perror("recv");
            exit(1);
        }
        buf[bytes] = '\0';
        printf("client: received %d bytes '%s'\n", bytes, buf);
    }
}

int main (int argc, char *argv[])
{
    int server_fd;

    if (argc != 3) {
        fprintf(stderr, "Usage: client hostname port\n");
        return 1;
    }
    server_fd = connect_to_server_socket(argv[1]);
    communicating_with_server(server_fd);
    close(server_fd);

    return 0;
}
