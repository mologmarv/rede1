#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <string.h>
#include <semaphore.h>
#include "fila.h"
#include "lista.h"

#define BUFFER_SIZE 1500

typedef struct conect {
    int nick;
    int client_sock;
} conect;

Queue *rBuffer;
List *lista;
pthread_mutex_t enqueuemutex;
pthread_mutex_t listmutex;
sem_t semafaro;
pthread_t* w;

void* handler(void* args);
void addqueue(Queue* rBuffer, char* buffer, int origem, char* flag);
void pushList(List* Lista, int sock, int id);
void* broadcast();
void getRequest(char* buffer, char* request, char* nickname);
int setNick(char* request, char* nick, char* flag);

int main() {
    int id = 1;
    int i;
    int qtbroad = 4;
    conect* conec;
    struct sockaddr_in servidor;
    struct sockaddr_in clientaddr;
    int s2;

    rBuffer = createQueue(); //inicia a fila dinamica
    lista = createList(); //inicia a lista dinamica
    sem_init(&semafaro, 0, 0); //inicializa semafaro
    pthread_mutex_init(&enqueuemutex, NULL); //inicializa mutex

    //cria socket
    int s = socket(AF_INET, SOCK_STREAM, 0);

    if (s < 0) {
        printf("ERRO socket não criado\n");
        return s;
    }

    //valores
    servidor.sin_port = htons(9000); // porta
    servidor.sin_addr.s_addr = inet_addr("127.0.0.1"); //ip
    servidor.sin_family = AF_INET; //familia arpanet

    //associa porta com o ip
    bind(s, (struct sockaddr*) & servidor, sizeof (servidor));

    if (s < 0) {
        printf("Erro Bind\n");
        return -1;
    }

    //pega o tamanho da estrutura
    socklen_t size;
    size = sizeof (servidor);

    //aguarda ate 10 conexoes
    listen(s, 20);


    w = (pthread_t*) malloc(sizeof (pthread_t));
    pthread_create(&w, NULL, broadcast, NULL);
    /*w = (pthread_t*) malloc(qtbroad* sizeof (pthread_t));
    for (i = 0; i < qtbroad; i++) {
        pthread_create(&w[i], NULL, broadcast, NULL);
        printf("thread %d criada\n", i);
    }*/

    while (1) {
        //Aceitar conexões. Esta função retorna uma conexão (usando um novo socket) ou NULL.
        //listenSocket ainda pode ser chamado para aceitar outras conexões!
        int s2 = accept(s, (struct sockaddr*) &servidor, &size);

        if (s2 > 0) {
            conec = (conect*) malloc(sizeof (conect));
            conec->client_sock = s2;
            pthread_t* t;
            t = malloc(sizeof (pthread_t));
            pthread_create(t, NULL, handler, (void*) conec);
            pushList(lista, s2, id);
            id = id + 1;
        }
        s2 = 0;

    }

    close(s2);
    return 0;
}

void* handler(void* args) {
    char sbuf[BUFFER_SIZE];
    char rbuf[BUFFER_SIZE];
    int flag;
    conect* connection;
    char hello[] = "Bem Vindo para colocar um Nick use a tag NICKNAME apelido\n";
    connection = (conect*) args;
    int n, m;
    char rq1[] = "NICKNAME";
    struct pollfd fds[2];
    char temp[] = "NULO";
    char sFlag[50];
    memset(sFlag, 0, strlen(sFlag));
    strcpy(sFlag, temp);
    char request[8];
    char nickname[50];
    memset(request, 0, strlen(request));
    memset(nickname, 0, strlen(nickname));


    fds[0].fd = connection->client_sock;
    fds[0].events = POLLIN;
    fds[1].fd = connection->client_sock;
    fds[1].events = POLLIN;


    //Buffer usado para receber e enviar echo
    uint8_t buffer[BUFFER_SIZE];

    //CONN_send(connection, (void*)hello, strlen(hello), 0);

    sendto(connection->client_sock, hello, strlen(hello), flag, NULL, 0);
    printf("Dentro do handler\n");

    //A idéia de um servidor echo é simplesmente mandar de volta a mensagem recebida
    while (1) {
        poll(fds, 2, -1);
        if (fds[0].revents & POLLIN) {
            n = recv(connection->client_sock, rbuf, BUFFER_SIZE, 0);
            getRequest(rbuf, request, nickname);
            setNick(request, nickname, sFlag);
            if (strcmp(request, rq1) != 0) {
                addqueue(rBuffer, rbuf, connection->client_sock, sFlag);
            }
            if (n <= 0) {
                break;
            }

        }
        //memset(sbuf,0,BUFFER_SIZE);
        if (fds[1].revents & POLLIN) {
            /*sprintf(sbuf,"%d %s",sizeQueue(rBuffer),rbuf);
            m = send(connection->client_sock, sbuf, BUFFER_SIZE, 0);
            printf("%s\n", rbuf);
            if (m <= 0) {
                break;
            }*/
        }
        memset(sbuf, 0, BUFFER_SIZE);
        memset(rbuf, 0, BUFFER_SIZE);
        memset(request, 0, strlen(request));
        memset(nickname, 0, strlen(nickname));
        //imprimir a mensagem recebida e o tamanho da mensagem
    }
    //fechar a conexão
    close(connection->client_sock);

}

void addqueue(Queue* rBuffer, char* buffer, int origem, char* flag) {
    pthread_mutex_lock(&enqueuemutex);
    enqueue(rBuffer, buffer, origem, flag);
    pthread_mutex_unlock(&enqueuemutex);
    sem_post(&semafaro);
}

void pushList(List* lista, int sock, int id) {
    pthread_mutex_lock(&listmutex);
    addLastList(lista, sock, id);
    pthread_mutex_unlock(&listmutex);
}

void* broadcast() {
    int socket;
    int i;
    int e;
    Node* broad;
    char buf[1500];
    memset(buf, 0, strlen(buf));
    while (1) {
        sem_wait(&semafaro);
        if (sizeQueue(rBuffer) > 0) {
            pthread_mutex_lock(&enqueuemutex);
            broad = dequeue(rBuffer, &e); //entra no buffer e pega o trabalho
            pthread_mutex_unlock(&enqueuemutex);
            memset(buf, 0, strlen(buf));
            
            if (broad != NULL) {
                for (i = 1; i < 1 + sizeList(lista); i++) {
                    getList(lista, i, &socket);
                    if (socket != broad->origem) {
                        if (strlen(broad->nickname) > 0) {
                            sprintf(buf, "%s: %s", broad->nickname, broad->msg);
                            send(socket, buf, strlen(buf), 0);
                        } else {
                            send(socket, broad->msg, BUFFER_SIZE, 0);
                        }
                    }
                    socket = 0;
                }
            }
            broad->next = NULL;
            memset(broad->msg, 0, strlen(broad->msg));
            memset(broad->nickname, 0, strlen(broad->nickname));
            memset(buf, 0, strlen(buf));

        }
    }

}

void getRequest(char* buffer, char* request, char* nickname){
    memset(request, 0, strlen(request));
    memset(nickname, 0, strlen(nickname));
    sscanf(buffer, "%s %s", request, nickname);
}

int setNick(char* request, char* nick, char* flag) {
    char rq1[] = "NICKNAME";
    if (strcmp(request, rq1) == 0) {
        strcpy(flag,nick);
        printf("nickname changed\n");
        return 1;
    }
}