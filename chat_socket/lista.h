#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#define BUFFER_SIZE 1500

typedef struct node2{
    int id;
    int client_sock;
    struct node2 *next;
} Node2;

typedef struct {
    Node2 *first;
    Node2 *last;
    int size;
} List;


List *createList();
void initializeList(List *l);

int addLastList(List *l,int e, int id);
int addList(List* l, int e, int index);
int removeElementList(List* l, int* e);

int getList(List* l, int index, int* e);
int setList(List* l, int index, int* e);

int indexOfList(List* l, int* e);

int containsList(List* l, int* e);

int sizeList(List* l);
int isEmptyList(List* l);
void printList(List* l);

List *createList() {
    List* l;
    l = (List*) malloc(sizeof (List));
    if (l != NULL) {
        initializeList(l);
        return l;

    } else {
        printf("Error List not allocated");
    }
}

void initializeList(List *l) {
    l->first = NULL;
    l->last = NULL;
    l->size = 0;
}

int addLastList(List *l, int sock, int id) {
    Node2* novo;
    novo = (Node2*) malloc(sizeof (Node2));
    novo->id = id;
    novo->client_sock = sock;
    novo->next = NULL;

    if (l->size == 0) {
        l->first = novo;
        l->last = novo;
        l->size++;
    } else {
        l->last->next = novo;
        l->last = novo;
        l->size++;
    }
}

int addList(List* l, int e, int index) {
    int pos = 1;
    Node2* i;
    Node2 *antes, *depois;

    Node2* novo;
    novo = (Node2*) malloc(sizeof (Node2));
    novo->id = e;
    novo->next = NULL;


    if (index > l->size) {
        addLastList(l, e, e);
        return 0;
    }
    else if (index == 0) {
        novo->next = l->first;
        l->first = novo;
        l->size++;
    }
    else {
        depois = l->first;
        for (i = l->first; i != NULL; i = i->next) {
            antes = depois;
            depois = depois->next;
            if (pos == index) {
                novo->next = depois;
                antes->next = novo;
                return 0;
            }
            antes = antes->next;
            pos++;
        }
        l->size++;
    }
}

int removeElementList(List* l, int* e) {

    Node2 *i, *p;
    Node2 *antes, *depois;
    if (l->first->id == *e) {
        p = l->first;
        l->first = l->first->next;
        if (l->first == NULL) l->last = NULL;

        *e = p->id;
        free(p);
        l->size--;
    } else {
        antes = l->first;
        for (depois = l->first->next; depois != NULL; depois = depois->next) {
            if (depois->id == *e) {
                antes->next = depois->next;
                if (antes->next == NULL) l->last = antes;
                free(depois);
                l->size--;
                return 1;
            }
        }
        return 0;
    }
}

int getList(List* l, int index, int* e) {
    int pos = 1;
    Node2 *depois;
    if (index == 0) {
        *e = l->first->client_sock;
        return 0;
    }
    else if(index == sizeList(l)){
        *e = l->last->client_sock;
        return 0;
    }
    else {
        for (depois = l->first; depois->next != NULL; depois = depois->next) {
            if (pos == index) {
                *e = depois->client_sock;
                return 1;
            }

            pos++;
        }
        return 0;
    }
}

int setList(List* l, int index, int* e) {
    Node2 *ponte;
    int pos = 0;
    ponte = l->first;

    if (pos <= l->size) {
        while (ponte != NULL) {
            if (pos == index) {
                ponte->id = *e;
                return 1;
            }
            ponte = ponte->next;
            pos++;
        }
    }
    else {
        return 0;
    }
    return 0;
}

int indexOfList(List* l, int* e) {
    Node2 *ponte;
    int pos = 0;
    ponte = l->first;

    if (pos <= l->size) {
        while (ponte != NULL) {
            if (ponte->id == *e) {
                return pos;
            }
            ponte = ponte->next;
            pos++;
        }
    }
    else {
        return 0;
    }
}

int containsList(List* l, int *e) {
    Node2 *ponte;
    int pos = 0;
    ponte = l->first;

    if (pos <= l->size) {
        while (ponte != NULL) {
            if (ponte->id == *e) {
                return 1;
            }
            ponte = ponte->next;
            pos++;
        }
    }
    else {
        return 0;
    }
}

int sizeList(List* l) {
    int p;
    p = l->size;
    return p;
}

int isEmptyList(List* l) {
    return ((l->first == NULL)&&(l->last == NULL));
}

void printList(List* l) {
    Node2 *i;
    printf("|");
    for (i = l->first; i != NULL; i = i->next) {
        printf(" %d |", i->id);
    }
    printf("\n");
}
