#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include <arpa/inet.h>

void logexit(const char *msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

int addrparse(const char *addrstr, const char* portstr, struct sockaddr_storage *storage){
    if(addrstr == NULL || portstr == NULL){
        return -1;
    }

    //parser da porta
    uint16_t port = (uint16_t) atoi(portstr); //unsigned short 16 bits (16 é o padrao da internet pro port)
    
    if(port == 0){
        return -1;
    }

    port = htons(port); //host to network short --> network byte order - bigengine (representacao de rede). ntohs faz o contrario

    //parser do endereço (precisa funcionar para ipv4 e ipv6)
    struct in_addr inaddr4; //32 bits IPv4 address
    if(inet_pton(AF_INET, addrstr, &inaddr4)){ //faz o parser e joga em inaddr4. pton: presentation to network
        struct sockaddr_in *addr4 = (struct sockaddr_in *) storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr = inaddr4;

        return 0;
    }

    struct in6_addr inaddr6; //128 bits IPv6 addres
    if(inet_pton(AF_INET6, addrstr, &inaddr6)){ //faz o parser e joga em inaddr6
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *) storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        // addr6->sin6_addr = inaddr6; --> C nao permite atribuir bytes assim
        memcpy(&(addr6->sin6_addr), &inaddr6, sizeof(inaddr6));

        return 0;
    }

    //se nao deu parser nem no IPv6, deu ruim
    return -1;
}

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize){
    //verificar qual o tipo do protocolo
    int version;
    char addrstr[INET6_ADDRSTRLEN+1] = "";
    uint16_t port;

    if(addr->sa_family == AF_INET){//ipv4
        version = 4;    
        struct sockaddr_in *addr4 = (struct sockaddr_in *) addr; //só mudando o tipo do ponteiro antes pra acessar os campos do ipv4 
        if( !inet_ntop(AF_INET, &(addr4->sin_addr), addrstr, INET6_ADDRSTRLEN+1)){
            logexit("erro ntop");
        }
        port = ntohs(addr4->sin_port); //network to host short
    }else if(addr->sa_family == AF_INET6){//ipv6
        version = 6;    
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *) addr; //só mudando o tipo do ponteiro antes pra acessar os campos do ipv4 
        if( !inet_ntop(AF_INET6, &(addr6->sin6_addr), addrstr, INET6_ADDRSTRLEN+1)){
            logexit("erro ntop");
        }
        port = ntohs(addr6->sin6_port); //network to host short
    }else{
        logexit("unknown protocol family");
    }
    
    if(str){
        snprintf(str, strsize, "IPv%d %s %hu", version, addrstr, port);
    }
}

int server_sockaddr_init(const char *proto, const char *portstr, struct sockaddr_storage *storage){
    //parser da porta
    uint16_t port = (uint16_t) atoi(portstr); //unsigned short 16 bits (16 é o padrao da internet pro port)
    port = htons(port);

    if(port == 0){
        return -1;
    }

    memset(storage, 0, sizeof(*storage));
    if(strcmp(proto, "v4") == 0){ //IPv4
        struct sockaddr_in *addr4 = (struct sockaddr_in *) storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr.s_addr = INADDR_ANY; //qualquer endereco do pc do server vai receber conexoes
        
        return 0;
    }else if(strcmp(proto, "v6") == 0){//IPv6
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *) storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        addr6->sin6_addr = in6addr_any; //qualquer endereco do pc do server vai receber conexoes
        
        return 0;
    }
    
    return -1;
}