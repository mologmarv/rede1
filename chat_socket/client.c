#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <string.h>

#define BUFFER_SIZE 1500

struct sockaddr_in cliente;

int main(){
    struct pollfd fds[2];//p checar se tem alguma modificacao no socket
    int b;//variavel p receber o socket
    char buffer[BUFFER_SIZE];
    //cria socket
    int s = socket(AF_INET, SOCK_STREAM, 0);//

    if (s < 0) {
        printf("ERRO socket não criado\n");
        return s;
    }

    fds[0].fd = 0;
    fds[0].events = POLLIN;
    fds[1].fd = s;
    fds[1].events = POLLIN;

    //valores
    cliente.sin_port = htons(9000);
    cliente.sin_addr.s_addr = inet_addr("127.0.0.1");
    cliente.sin_family = AF_INET;

    //conecta ao server
    int c = connect(s, (struct sockaddr*) &cliente, sizeof (cliente));

    if (c < 0) {
        printf("Erro não connect\n");
        return c;
    }

    while (1) {
        poll(fds, 2, -1);

        if (fds[1].revents & POLLIN) {
            int r = read(s,buffer,BUFFER_SIZE);
            printf("%s",buffer);
            if (r <= 0) {
                break;
            }

        }
        memset(buffer, 0, BUFFER_SIZE);
        if (fds[0].revents & POLLIN) {
            b = read(0, buffer, BUFFER_SIZE);
            write(s,buffer,b);
        }

        memset(buffer, 0, BUFFER_SIZE);
    }


    return 0;
}