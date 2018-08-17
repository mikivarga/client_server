#ifndef SNIFFER_RAW_SOCKET_H
#define SNIFFER_RAW_SOCKET_H

#include<netinet/in.h>
#include<errno.h>
#include<netdb.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/ip_icmp.h>
#include<netinet/udp.h>
#include<netinet/tcp.h>
#include<netinet/ip.h>
#include<netinet/if_ether.h>
#include<net/ethernet.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/ioctl.h>
#include<sys/time.h>
#include<sys/types.h>
#include<unistd.h>

#define MAXBUFSIZE 65536

#define TCP 6
#define UDP 17

void print_ip_header(unsigned char* buffer, int size);
void print_data(unsigned char* data , int size);
void print_tcp_packet(unsigned char *buffer, int size);
void print_udp_packet(unsigned char *buffer , int size);

#endif