<h1>Multithread Cliet-Server using C sockets</h1>
<h3>Redes de Computadores</h3>

Projeto desenvolvido seguindo a <a href="https://www.youtube.com/playlist?list=PLyrH0CFXIM5Wzmbv-lC-qvoBejsa803Qk">playlist</a> de aulas do professor Ítalo Cunha, do DCC/UFMG.

Linguagem: C

<h3>Explicação:</h3>

Este projeto implementa uma rede de comunicação protocolo IP (v4 e v6) client-server.

<b>Server:</b> Processo que espera uma conexão. Precisa saber o protocolo (IPv4 ou IPv6) e a porta que vai aguardar conexões.
<b>Client:</b> Processo que inicia a conexão. Precisa conhecer o identificador do servidor (endereço IP e porta)

Inicialmente, o desenvolvimento é feito em server.c de forma que, quando estabelecida a conexão client-server, o servidor fica "travado" enquanto aguarda que o cliente mande a mensagem para, então, responder e encerrar a conexão.
Esse fluxo é representado no diagrama a seguir:

<img width="1116" height="786" alt="image" src="https://github.com/user-attachments/assets/382ad116-7c41-48eb-89eb-159b5fa8a5e2" />

Porém, isso faz com que outros clients não possam conectar e receber a resposta do servidor apropriadamente enquanto o primeiro cliente não enviar a mensagem. Isso claramente é um grande problema, basta
imaginar um site que vocÊ só consegue acessar quando nenhum outro usuário estiver acessando.

Para resolver isso, server-mt.c faz uso de <b>multi-threads</b> separadas para, após a conexão, tratar
a conexão com o client em uma thread separada. O while(1) que trata as conexões agora, após o accept, chama pthread_create() passando de parametros a funcão que trata a comunicação com o client (client_thread), uma struct client_data com o socket e o endereço do client, e um id.

<img width="954" height="110" alt="image" src="https://github.com/user-attachments/assets/0ec0b082-48db-41d7-afcd-15bc625693f8" />

Dessa forma, o diagrama da comunicação passa a ter a seguinte propriedade: 

<img width="1369" height="775" alt="image" src="https://github.com/user-attachments/assets/8a63ad1c-0b53-4c44-91b0-58c4c31aa83f" />

Agora, após o accept() disparar, o servidor abre uma thread (um processo que roda em paralelo, sem interromper o fluxo do programa principal) e retorna para a fase de abertura passiva.

<h3>Execução:</h3>
Para rodar localmente, recomenda-se Linux ou WSL2.

Rode o comando make no terminal para compilar os arquivos e gerar os executáveis e, em seguida:
```
terminal 1: ./server-mt v4 5151
terminal 2: ./client 127.0.0.1 5151
terminal 3 (se quiser testar multi-thread handling): ./client 127.0.0.1 5151
```

Neste exemplo, o protocolo é IPv4, porém você pode mudar para IPv6 (neste caso, use o localhost ::1 no lugar de 127.0.0.1) e também pode usar outra porta livre do seu sistema.
