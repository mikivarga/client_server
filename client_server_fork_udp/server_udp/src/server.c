#include "server.h"
#include <sys/wait.h>
#include <signal.h>

static void sigchl_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

static void comunicatinon_with_client(int sock_fd)
{
    int num_bytes;
    struct sockaddr_storage remote_addr;
    socklen_t addr_len;
    char buff[MAXDATASIZE];
    char s[INET6_ADDRSTRLEN];

    addr_len = sizeof(remote_addr);
    if ((num_bytes = recvfrom(sock_fd, buff, MAXDATASIZE - 1, 0, (struct sockaddr *)&remote_addr, &addr_len)) == -1) {
        perror("recvfrom");
        exit(1);
    }
    if (!fork()) {
        printf("server: got packet from %s\n",
        inet_ntop(remote_addr.ss_family, get_ip_addr(((struct sockaddr *)&remote_addr)), s, sizeof(s)));
        printf("server: packet is %d bytes long\n", num_bytes);
        buff[num_bytes] = '\0';
        printf("server: packet contains \"%s\"\n", buff);
        num_bytes = sendto(sock_fd, buff, num_bytes, 0, (struct sockaddr *) &remote_addr, addr_len);
        if ((num_bytes) == -1) {
            perror("sendto");
            exit (1);
        }
        printf("server: packet sent to %s : %d bytes\n",
        inet_ntop(remote_addr.ss_family, get_ip_addr(((struct sockaddr *)&remote_addr)), s, sizeof(s)), num_bytes);
        close(sock_fd);
        exit(0);
    }
}

static void handle_connections_from_clients(int sock_fd)
{
    while(1) {
        comunicatinon_with_client(sock_fd);
    }
}

static int  server_bind_socket(const char *port)
{
    int sock_fd, rv;
    int yes = 1;
    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    rv = getaddrinfo(NULL, port, &hints, &servinfo);
    if (rv != 0) {
        fprintf(stderr, "Select Server: %sn", gai_strerror(rv));
        exit(1);
    }
    for(p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket");
            continue ;
        }
        if ((setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)) {
            perror("server: setsockopt");
            exit(1);
        }
        if (bind(sock_fd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sock_fd);
            perror("server: bind");
            continue ;
        }

        break ;
    }
    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }
    freeaddrinfo(servinfo);

    return sock_fd;
}

int main(int argc, char *argv[])
{
    int server_socket_fd;
    struct sigaction sa;

    if (argc != 2) {
        fprintf(stderr, "server usage: %s port\n", argv[0]);
        exit(1);
    }
    server_socket_fd = server_bind_socket(argv[1]);
    sa.sa_handler = sigchl_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }
    printf("server waiting for connections, port %s...\n", argv[1]);
    handle_connections_from_clients(server_socket_fd);

    return 0;
}