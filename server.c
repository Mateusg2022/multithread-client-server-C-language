#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <unistd.h>

#define BUFSZ 1024

void usage(int argc, char **argv){
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
    if(argc < 3){
        usage(argc, argv);
    }
    
    struct sockaddr_storage storage;
    if(server_sockaddr_init(argv[1], argv[2], &storage) != 0){//parsing e joga no storage
        usage(argc, argv);
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0); //storage.ss_family, depois do parse, sabe se é v4 ou v6

    if(s == -1){
        logexit("erro no socket");
    }

    int enable = 1;
    if(0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))){ //permite usar a porta depois de cortar o servidor, para evitar esperar os 2min de timeout do linux
        logexit("erro setsockopt");
    }
    
    struct sockaddr *addr = (struct sockaddr *)(&storage);
    // no servidor, faremos: bind -> listen -> accept

    if(bind(s, addr, sizeof(storage)) != 0){
        logexit("erro bind");
    }

    if(listen(s, 10) != 0){ //10 é o maximo de conexoes que permite na fila. Se exceder, o client ja recebe a mensagem de maximo de lotação
        logexit("erro listen");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);

    printf("bound to %s, waiting connections\n", addrstr);

    while(1){

        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(s, caddr, &caddrlen);//espera conexao em s. QUando achar, cria um socket (csock) novo em caddr com as informacoes do peer de conexao (ie, o client) 
        if(csock == -1){
            logexit("erro accept");
        }

        char caddrstr[BUFSZ];
        addrtostr(caddr, caddrstr, BUFSZ);
        printf("[log] connection from %s\n", caddrstr);

        //le a mensagem do client
        char buf[BUFSZ];
        memset(buf, 0, BUFSZ);
        size_t count = recv(csock, buf, BUFSZ, 0); //no servidor, vamos receber todos os dados de uma vez. se o client mandasse em pacotes, deveria ter a mesma logica de receber bytes ate chegar em BUFSZ bytes
        printf("[msg] %s, %d bytes: %s\n", caddrstr, (int)count, buf);

        //resposta para o client
        sprintf(buf, "remote endpoint: %.1000s\n", caddrstr);
        count = send(csock, buf, strlen(buf)+1, 0);

        if(count != strlen(buf)+1){
            logexit("erro send");
        }
        close(csock);
    }

    exit(EXIT_SUCCESS);
}