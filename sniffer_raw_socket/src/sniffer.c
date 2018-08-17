#include "./../inc/sniffer_raw_socket.h"

static void process_packet(unsigned char* buffer, int size)
{
    static int total = 0, udp = 0, tcp = 0, others = 0; 
    struct iphdr *iph = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    ++total;
    switch (iph->protocol)
    {
        case TCP:
            tcp++;
            print_tcp_packet(buffer , size);
            break;

        case UDP:
            udp++;
            print_udp_packet(buffer , size);
            break;
         
        default:
            others++;
            printf("\nTCP : %d   UDP : %d   Others : %d   Total : %d\n",
            tcp, udp, others, total);
            break;
    }
}

int main(void)
{
    int data_size;
    socklen_t saddr_size;
    struct sockaddr saddr;
         
    unsigned char buffer[MAXBUFSIZE];
    int sock_raw = socket( AF_PACKET , SOCK_RAW , htons(ETH_P_ALL));
    if(sock_raw < 0)
    {
        perror("socket error");
        return 1;
    }
    while(1)
    {
        saddr_size = sizeof(saddr);
        data_size = recvfrom(sock_raw, buffer, MAXBUFSIZE, 0, &saddr , &saddr_size);
        if(data_size < 0)
        {
            printf("recvfrom error , failed to get packets\n");
            return 1;
        }
        process_packet(buffer, data_size);
    }
    close(sock_raw);

    return 0;
}