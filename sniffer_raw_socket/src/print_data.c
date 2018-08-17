#include "./../inc/sniffer_raw_socket.h"

void print_ethernet_header(unsigned char *buffer, int size)
{
    struct ethhdr *eth = (struct ethhdr *)buffer;
     
    printf("\n");
    printf("Ethernet Header\n");
    printf("   |-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_dest[0] , eth->h_dest[1] , eth->h_dest[2] , eth->h_dest[3] , eth->h_dest[4] , eth->h_dest[5] );
    printf("   |-Source Address      : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_source[0] , eth->h_source[1] , eth->h_source[2] , eth->h_source[3] , eth->h_source[4] , eth->h_source[5] );
    printf("   |-Protocol            : %u \n",(unsigned short)eth->h_proto);
}

void print_ip_header(unsigned char* buffer, int size)
{
    unsigned short iphdrlen;
    struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    struct sockaddr_in source, dest;
    iphdrlen =iph->ihl*4;

    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = iph->saddr;
    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr; 
    printf("\n");
    printf("IP Header\n");
    printf("   |-IP Version        : %d\n", (unsigned int)iph->version);
    printf("   |-IP Header Length  : %d DWORDS or %d Bytes\n", (unsigned int)iph->ihl,((unsigned int)(iph->ihl))*4);
    printf("   |-Type Of Service   : %d\n", (unsigned int)iph->tos);
    printf("   |-IP Total Length   : %d  Bytes(Size of Packet)\n", ntohs(iph->tot_len));
    printf("   |-Identification    : %d\n", ntohs(iph->id));
    printf("   |-TTL      : %d\n", (unsigned int)iph->ttl);
    printf("   |-Protocol : %d\n", (unsigned int)iph->protocol);
    printf("   |-Checksum : %d\n", ntohs(iph->check));
    printf("   |-Source IP        : %s\n", inet_ntoa(source.sin_addr));
    printf("   |-Destination IP   : %s\n", inet_ntoa(dest.sin_addr));
}

void print_data(unsigned char* data , int size)
{
    int i, j;
    for (i = 0 ; i < size ; i++)
    {
        if (i != 0 && i % 16 == 0)
        {
            printf("         ");
            for (j = i - 16 ; j < i ; j++)
            {
                if (data[j]>=32 && data[j]<=128)
                    printf("%c",(unsigned char)data[j]);              
                else
                    printf(".");
            }
            printf("\n");
        }
        if (i % 16 == 0)
            printf("   ");
        printf(" %02X",(unsigned int)data[i]);
        if (i == size - 1)
        {
            for (j = 0; j < 15 - i % 16; j++)
                printf("   ");
            printf("         ");
            for( j = i - i % 16; j <= i; j++)
            {
                if( data[j] >= 32 && data[j] <= 128)
                    printf("%c",(unsigned char)data[j]);
                else
                    printf(".");
            }
            printf("\n");
        }
    }
}

void print_tcp_packet(unsigned char *buffer, int size)
{
    unsigned short iphdrlen;
    struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    iphdrlen = iph->ihl * 4;
    struct tcphdr *tcph=(struct tcphdr*)(buffer + iphdrlen + sizeof(struct ethhdr));         
    int header_size = sizeof(struct ethhdr) + iphdrlen + tcph->doff*4;

    print_ethernet_header(buffer , size);

    printf("\n\n***********************TCP Packet*************************\n");     
    print_ip_header(buffer, size);
    printf("\n");
    printf("TCP Header\n");
    printf("   |-Source Port      : %u\n",ntohs(tcph->source));
    printf("   |-Destination Port : %u\n",ntohs(tcph->dest));
    printf("   |-Sequence Number    : %u\n",ntohl(tcph->seq));
    printf("   |-Acknowledge Number : %u\n",ntohl(tcph->ack_seq));
    printf("   |-Header Length      : %d DWORDS or %d BYTES\n" ,(unsigned int)tcph->doff,(unsigned int)tcph->doff*4);
    printf("   |-Urgent Flag          : %d\n",(unsigned int)tcph->urg);
    printf("   |-Acknowledgement Flag : %d\n",(unsigned int)tcph->ack);
    printf("   |-Push Flag            : %d\n",(unsigned int)tcph->psh);
    printf("   |-Reset Flag           : %d\n",(unsigned int)tcph->rst);
    printf("   |-Synchronise Flag     : %d\n",(unsigned int)tcph->syn);
    printf("   |-Finish Flag          : %d\n",(unsigned int)tcph->fin);
    printf("   |-Window         : %d\n",ntohs(tcph->window));
    printf("   |-Checksum       : %d\n",ntohs(tcph->check));
    printf("   |-Urgent Pointer : %d\n",tcph->urg_ptr);
    printf("\n");
    printf("                        DATA Dump                         ");
    printf("\n");
    printf("IP Header\n");
    print_data((buffer + sizeof(struct ethhdr)), iphdrlen);
    printf("TCP Header\n");
    print_data(buffer + sizeof(struct ethhdr) + iphdrlen, tcph->doff * 4);
    printf("Data Payload\n");  
    print_data(buffer + header_size, size - header_size);                    
    printf("\n###########################################################");
}
 
void print_udp_packet(unsigned char *buffer , int size)
{
    unsigned short iphdrlen;
    struct iphdr *iph = (struct iphdr *)(buffer +  sizeof(struct ethhdr));
    iphdrlen = iph->ihl*4;
    struct udphdr *udph = (struct udphdr*)(buffer + iphdrlen  + sizeof(struct ethhdr));
    int header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof(udph);

    printf("\n\n***********************UDP Packet*************************\n");
     
    print_ip_header(buffer,size);
    printf("\nUDP Header\n");
    printf("   |-Source Port      : %d\n" , ntohs(udph->source));
    printf("   |-Destination Port : %d\n" , ntohs(udph->dest));
    printf("   |-UDP Length       : %d\n" , ntohs(udph->len));
    printf("   |-UDP Checksum     : %d\n" , ntohs(udph->check));
     
    printf("\n");
    printf("IP Header\n");
    print_data((buffer + sizeof(struct ethhdr)), iphdrlen);
    printf("UDP Header\n");
    print_data(buffer+iphdrlen + sizeof(struct ethhdr), sizeof(udph));
    printf("Data Payload\n");  
    print_data(buffer + header_size, size - header_size);
    printf("\n###########################################################");
}