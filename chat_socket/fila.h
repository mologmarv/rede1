#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1500

typedef struct node {
    int origem;
    char msg[1500];
    char nickname[50];
    struct node *next;
} Node;

typedef struct {
    Node *first;
    Node *last;
    int size;
} Queue;



Queue *createQueue();
void initializeQueue(Queue *q);
int enqueue(Queue* q, char* msn, int origem,char* flag);
Node *dequeue(Queue* q, int* e);
int sizeQueue(Queue* q); //tamanho da fila
int isEmptyQueue(Queue* q); //a fila esta vazia


Queue *createQueue() {
    Queue* p;
    p = (Queue*) malloc(sizeof (Queue));
    if (p != NULL) {
        initializeQueue(p);
        return p;
    } else {
        printf("Erro Queue not allocated");
    }
}

void initializeQueue(Queue *q) {
    q->first = NULL;
    q->last = NULL;
    q->size = 0;
}

int enqueue(Queue* q, char* msn, int origem,char* flag) {
    Node* novo;
    char rq1[] = "NULO";
    novo = (Node*) malloc(sizeof (Node));
    novo->next = NULL;
    novo->origem = origem;
    memset(novo->msg,0,sizeof(BUFFER_SIZE));
    memset(novo->nickname,0,50);
    strcpy(novo->msg,msn);
    if (strcmp(flag, rq1) != 0) {
       strcpy(novo->nickname,flag); 
    }
    

    if (q->size == 0) {
        q->last = novo;
        q->first = novo;
        q->size++;
    } 
    else {
        q->last->next = novo;
        q->last = novo;
        q->size++;
    }
}

Node *dequeue(Queue* q, int* e) {
    Node* p;
    if (q->size == 0) {
        *e = 0;
    }
    else if (q->size == 1) {
        p = q->first;
        q->first = NULL;
        q->last = NULL;
        
        *e = 1;
        q->size--;
        return p;
    }
    else {
        p = q->first;
        q->first = q->first->next;
        
        
        q->size--;
        *e = 1;
        return p;
    }
}

int sizeQueue(Queue* q) {
    int aux;
    aux = q->size;
    return aux;
}

int isEmptyQueue(Queue* q) {
    if ((q->last == NULL) && (q->first == NULL)) {
        return 1;
    } else {
        return 0;
    }
}