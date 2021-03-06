#include "server.h"
#include <pthread.h>

static void *comunicatinon_with_client(void *sock_desc)
{
    int bytes;
    char client_message[MAXDATASIZE];
    int sock_fd = *(int*)sock_desc;

    pthread_detach(pthread_self());
    while(bytes = recv(sock_fd , client_message , MAXDATASIZE - 1, 0)) {
        if (bytes == -1) {
            perror("recv");
            pthread_exit(NULL);
        }
        printf("Server receive %d bytes\n", bytes);
        client_message[bytes] = '\0';
        if (send(sock_fd, client_message, strlen(client_message), 0) == -1) {
            perror("send");
            pthread_exit(NULL);
        }
        printf("Server sent %d bytes\n", bytes);
    }
    close(sock_fd);
    free(sock_desc);
    return 0;
}

static void handle_connections_from_clients(int sock_fd)
{
    int client_fd, *new_client_fd;
    struct sockaddr_storage remote_addr;    
    socklen_t sin_size;
    char s[INET6_ADDRSTRLEN];

    while(1) {
        sin_size = sizeof(remote_addr);
        client_fd = accept(sock_fd, (struct sockaddr *)&remote_addr, &sin_size);
        if (client_fd == -1) {
            perror("accept");
            continue ;
        }
        inet_ntop(remote_addr.ss_family, get_ip_addr((struct sockaddr *)&remote_addr), s, sizeof(s));
        printf("server: got connection from %s\n", s);
        pthread_t sniffer_thread;
        pthread_join(sniffer_thread, NULL);
        new_client_fd = malloc(1);
        *new_client_fd = client_fd;
        if (pthread_create( &sniffer_thread , NULL,
        comunicatinon_with_client , (void*)new_client_fd) < 0)
        {
            perror("could not create thread");
            exit(1);
        }
    }
}

static int  server_bind_socket(void)
{
    int sock_fd;
    int status;
    int yes = 1;
    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    status = getaddrinfo(NULL, PORT, &hints, &servinfo);
    if (status != 0) {
        fprintf(stderr, "Select Server: %sn", gai_strerror(status));
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
        exit (1);
    }
    freeaddrinfo(servinfo);

    return sock_fd;
}

int main(void)
{
    int server_socket_fd;

    server_socket_fd = server_bind_socket();
    if (listen(server_socket_fd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }
    printf("server waiting for connections, port %s...\n", PORT);
    handle_connections_from_clients(server_socket_fd);

    return 0;
}