#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFSZ 1024

// struct sockaddr --> instancia o _in ou o _in6 e passa o ponteiro atribuindo ao sockaddr. Ele funciona como uma interface, seu unico campo é a familia do protocolo
// struct sockaddr_in --> 32 bits IPv4
// struct sockaddr_in6 --> 128 bits IPv6
// struct sockaddr_storage --> struck grande que instanciamos e fazemos o castting para o _in ou o _in6. Até se surgir o IPv10, dá pra adaptar ele e usar

void usage(int argc, char **argv){
    printf("usage: %s <server_IP> <server port>\n", argv[0]);
    printf("example: %s 127.0.0.1 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
    if(argc < 3){
        usage(argc, argv);
    }
    
    struct sockaddr_storage storage;
    if(addrparse(argv[1], argv[2], &storage) != 0){//parsing e joga no storage
        usage(argc, argv);
    }
    
    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0); //storage.ss_family, depois do parse, sabe se é v4 ou v6

    if(s == -1){
        logexit("erro no socket");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);

    if(connect(s, addr, sizeof(storage)) != 0){ //o conecct vai usar o *sockaddr
        logexit("erro no connect");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);

    printf("connected to %s\n", addrstr);

    char buf[BUFSZ];
    memset(buf, 0, BUFSZ);

    printf("mensagem> ");
    fgets(buf, BUFSZ-1, stdin);

    size_t count = send(s, buf, strlen(buf)+1, 0); //count armazena quantos bytes de buf foram enviados

    if(count != strlen(buf)+1){
        logexit("erro send");
    }

    memset(buf, 0, BUFSZ);
    unsigned total = 0; //o server pode mandar os dados em pacotes, vamos acumular esses dados

    while(1){
        count = recv(s, buf+total, BUFSZ-total, 0);

        if(count == 0){ //count == 0 significa que eu nao recebi nada, se o recv nao recebeu nada é pq a conexao foi fechada
            break;
        }
        total += count;
    }

    close(s);

    printf("received %u bytes\n", total);
    puts(buf);

    exit(EXIT_SUCCESS);
}