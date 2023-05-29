#include "gfx/gfx.h"
#include <stdio.h>
#include <unistd.h>

typedef struct AlocList{ //Lista de blocos alocados.
    unsigned id, startAdress, endAdress;
    struct AlocList *next;
    struct AlocList *prev;
}AlocList;

typedef struct EmptyList{ //Lista de blocos vazios.
    unsigned startEmptyAdd, b;
    struct EmptyList* next;
}EmptyList;

float calculateMemoryUsage(AlocList *aloc, unsigned M);
void drawBlocks(EmptyList *empty, AlocList *aloc, unsigned M);
void firstEmptyListInsert(EmptyList **list, unsigned size);
void deallocatesFreeBlock(EmptyList **empty, unsigned key, unsigned t);
void allocNewBlock(AlocList **list, EmptyList **empty);
void insertInOrder(EmptyList **empty, EmptyList **new);
void allocatesFreeBlock(EmptyList **empty, unsigned start, unsigned end);
void deallocatesAllocatedBlock(AlocList **list, EmptyList **empty);

float calculateMemoryUsage(AlocList *aloc, unsigned M){
    float memuse=0;
    AlocList *aux = aloc;

    while(aux){
        memuse += aux->endAdress - aux->startAdress + 1;
        aux = aux->next;
    }

    return (memuse/M) * 100;
}

void drawBlocks(EmptyList *empty, AlocList *aloc, unsigned M){
    gfx_rectangle(30, 304, 994, 464); //Cria o "array"

    int x1, x2, height, width; //Posições para colocar o bloco alocado/desalocado
    float memUse;
    double Mdraw = (double)M; //Utilizo esta variável para não dividir por um número inteiro M.
    char memUseChar[3], startAdd[10], endAdd[10], id[10]; //Variáveis auxiliares para imprimir na gfx.

    memUse = calculateMemoryUsage(aloc, M);
    sprintf(memUseChar, "%.0f", memUse); //Transforma a procentagem de uso da memória em char para printar.

    gfx_set_font_size(30);
    gfx_text(360, 184, "Memory Usage:         %");
    gfx_text(580, 184, memUseChar);

    while(empty){ //Imprime os blocos de memória desalocados.
        x1 = 31 + (964/Mdraw) * empty->startEmptyAdd; //31 inicia na borda esquerda do "array".
        x2 = x1 + empty->b * (964/Mdraw) - 2; //-2 para encaixar melhor no bloco.

        gfx_set_color(255, 0, 0);
        gfx_filled_rectangle(x1, 305, x2, 463);

        //Imprime o endereço inicial e o tamanho na gfx.
        sprintf(startAdd, "%u", empty->startEmptyAdd);
        sprintf(endAdd, "%u", empty->b);
        gfx_set_font_size(15);
        gfx_text(x1+3, 467, startAdd);
        gfx_text(x2-15, 283, endAdd);

        //Imprime uma linha ao lado esquerdo e direito do bloco para ficar mais visível a separação.
        gfx_set_color(255, 255, 255);
        gfx_line(x1-1, 305, x1-1, 463);
        gfx_line(x2+1, 305, x2+1, 463);

        empty = empty->next;
    }

    while(aloc){ //Imprime os blocos alocados.
        x1 = 31 + (964/Mdraw) * aloc->startAdress;
        x2 = x1 + (aloc->endAdress - aloc->startAdress + 1) * (964/Mdraw) - 2;

        gfx_set_color(0, 255, 0);
        gfx_filled_rectangle(x1, 305, x2, 463);

        sprintf(startAdd, "%u", aloc->startAdress);
        sprintf(endAdd, "%u", aloc->endAdress);
        gfx_set_font_size(15);

        if(aloc->startAdress == aloc->endAdress) { //Caso o bloco tenha endereço inicial e final iguais (1 byte ocupado) imprime uma só vez.
            gfx_text(x1 + ((aloc->endAdress - aloc->startAdress + 1) * (964/Mdraw))/2 - width/2, 467, startAdd);
        }
        else {
            gfx_text(x1 + 3, 467, startAdd);
            gfx_text(x2 - 14, 467, endAdd);
        }

        sprintf(id, "%u", aloc->id);
        gfx_set_font_size(20);
        gfx_set_color(0, 0, 0);
        gfx_get_text_size(id, &width, &height);
        gfx_text(x1 + ((aloc->endAdress - aloc->startAdress + 1) * (964/Mdraw))/2 - width/2,384 - height/2, id);
        //Este último gfx_text imprime o ID do bloco no meio dele.

        gfx_set_color(255, 255, 255);
        gfx_line(x1-1, 305, x1-1, 463);
        gfx_line(x2+1, 305, x2+1, 463);

        aloc = aloc->next;
    }

    //Legendas
    gfx_set_font_size(15);
    gfx_set_color(0, 255, 0);
    gfx_text(31, 600, "Legendas:");
    gfx_set_color(255, 255, 255);
    gfx_text(31, 618, "Em cima: Tamanho (b)");
    gfx_text(31, 636, "Dentro do bloco: ID");
    gfx_text(31, 654, "Em baixo: Endereco inicial/final");

    gfx_set_color(255, 255, 255);
    gfx_paint();
}

//Starta a lista simples de blocos não alocados, endereço inicial 0 e final M.
void firstEmptyListInsert(EmptyList **list, unsigned size){
    EmptyList *new = malloc(sizeof(EmptyList));

    if(new) {
        new->startEmptyAdd = 0;
        new->b = size;
        new->next = NULL;
        *list = new;
    }
    else {
        printf("--> Erro em alocar nó!\n");
        system("read -p \"\nPressione enter para sair.\" saindo");
    }
}

//Desaloca o bloco vazio que foi alocado o novo bloco.
void deallocatesFreeBlock(EmptyList **empty, unsigned key, unsigned t){
    EmptyList *aux;
    aux = *empty;

    while(aux->startEmptyAdd != key)//Percorre a lista até achar o nó para ser alterado.
        aux = aux->next;

    if(aux->b - t > 0) { //Caso ainda sobre espaço no bloco vazio para criar outro.
        aux->startEmptyAdd += t;
        aux->b -= t;
    }
    else {
        if((*empty)->startEmptyAdd == key) { //Remove do início.
            aux = *empty;
            *empty = (*empty)->next;
            free(aux);
        }
        else{
            EmptyList *prev;
            prev = *empty;
            aux = (*empty)->next;

            while(aux->next) { //Remove do meio ou do final.
                if(aux->startEmptyAdd == key) {
                    prev->next = aux->next;
                    free(aux);
                }
            }
        }
    }
}

//Aloca um novo bloco na lista de alocados.
void allocNewBlock(AlocList **list, EmptyList **empty){
    unsigned t;
    static unsigned ID=0; //Este ID sempre será incrementado.
    EmptyList *emptyAux;
    AlocList *alocAux;

    emptyAux = *empty;
    alocAux = *list;

    system("clear");

    printf("--> E o tamanho (t) do bloco?\n");
    scanf("%u", &t);

    while(t<=0){
        printf("--> O bloco deve possuir tamanho 1 ou maior!\n--> Digite novamente:\n");
        scanf("%u", &t);
    }

    while(emptyAux){ //Enquanto na lista de blocos livres houver blocos livres...
        if(emptyAux->b>=t){ //Caso encontre algum bloco livre que caiba o novo bloco alocado, e tenha menor tamanho necessário.
            AlocList *new = malloc(sizeof(AlocList));
            ID++;
            new->id = ID;

            if(!alocAux){ //Caso a lista de alocados esteja vazia.
                new->next = NULL;
                new->prev = NULL;
                new->startAdress = emptyAux->startEmptyAdd;
                new->endAdress = new->startAdress + t-1;
                *list = new;

                deallocatesFreeBlock(*&empty, emptyAux->startEmptyAdd, t);

                return;
            }

            while(alocAux){
                if(!alocAux->next){ //Como a lista é ordenada e o ID é sempre incrementado, sempre adiciono no final da lista.
                    new->startAdress = emptyAux->startEmptyAdd;
                    new->endAdress = new->startAdress + t-1;
                    new->next = alocAux->next;
                    new->prev = alocAux;
                    if(alocAux->next)
                        alocAux->next->prev = new;
                    alocAux->next = new;

                    deallocatesFreeBlock(*&empty, emptyAux->startEmptyAdd, t);

                    return;
                }
                alocAux = alocAux->next;
            }
        }
        emptyAux = emptyAux->next;
    }
    printf("--> Não há espaço para alocar um novo bloco!\n");
    system("read -p \"\nPressione enter para sair.\" saindo");
}

//Insere o bloco alocado na lista de desalocados em ordem.
void insertInOrder(EmptyList **empty, EmptyList **new) {
    if(!(*empty)){ //Caso não haja lista.
        *empty = *new;
        (*empty)->next = NULL;
    }
    else if((*new)->b < (*empty)->b){ //Caso o primeiro nó já seja referência para colocar ordenado.
        (*new)->next = *empty;
        *empty = *new;
    }
    else{
        EmptyList *aux = *empty;

        while(aux->next && (*new)->b < aux->next->b)
            aux = aux->next;

        (*new)->next = aux->next;
        aux->next = *new;
    }
}

//Aloca um novo bloco na lista de blocos livres.
void allocatesFreeBlock(EmptyList **empty, unsigned start, unsigned end) {
    EmptyList *aux = NULL, *new = malloc(sizeof(EmptyList));
    EmptyList *pontDir = NULL, *pontEsq = NULL;

    if(*empty)
        aux = (*empty)->next;

    new->startEmptyAdd = start; //Ele cria o suposto novo bloco que será inserido, porém ele pode ser alterado depois.
    new->b = end-start+1;

    if(*empty) {
        if ((*empty)->startEmptyAdd == end + 1) { //Caso o nó que dê para unir estiver a direita e for o primeiro nó da lista.
            new->b += (*empty)->b;
            pontDir = *empty; //pontDir marca o nó a direita para ser removido da lista depois.
        }
        if ((*empty)->startEmptyAdd + (*empty)->b ==
            start) { //Caso o nó que dê para unir esteja na esquerda e for o primeiro.
            new->startEmptyAdd = (*empty)->startEmptyAdd;
            new->b += (*empty)->b;
            pontEsq = *empty; //pontEsq marca o nó a esquerda para ser removido da lista depois.
        }
    }

    while(aux) {
        if(aux->startEmptyAdd == end+1) { //Mesma coisa do de cima, só que para os outros nós sem ser o primeiro nó.
            new->b += aux->b;
            pontDir = aux;
        }
        if(aux->startEmptyAdd + aux->b == start){
            new->startEmptyAdd = aux->startEmptyAdd;
            new->b += aux->b;
            pontEsq = aux;
        }

        aux = aux->next;
    }

    if(pontDir) { //Caso haja o bloco no lado direito para ser removido.
        EmptyList *ant = *empty;
        aux = (*empty)->next;

        if(*empty == pontDir){
            *empty = (*empty)->next;
            free(ant);
        }
        else {
            while (aux != pontDir) {
                aux = aux->next;
                ant = ant->next;
            }

            ant->next = aux->next;
            free(aux);
        }
    }
    if(pontEsq) { //Caso haja o bloco no lado esquerdo para ser removido.
        EmptyList *ant = *empty;
        aux = (*empty)->next;

        if(*empty == pontEsq){
            *empty = (*empty)->next;
            free(ant);
        }
        else {
            while (aux != pontEsq) {
                aux = aux->next;
                ant = ant->next;
            }

            ant->next = aux->next;
            free(aux);
        }
    }

    insertInOrder(*&empty, &new); //Depois de ter alterado a lista, removendo os nós que devem ser juntados
                                        //e alterado o novo nó, ele insere o novo nó na lista ordenada por b.
}

//Desaloca um bloco alocado e insere na lista de blocos desalocados um novo bloco.
void deallocatesAllocatedBlock(AlocList **list, EmptyList **empty){
    unsigned ID, startAdd, endAdd;
    AlocList *alocAux;

    system("clear");

    printf("--> Qual o ID do bloco que você deseja remover?\n");
    scanf("%u", &ID);

    if(*list) {
        alocAux = *list;

        if ((*list)->id == ID) { // Caso o nó seja o primeiro
            startAdd = (*list)->startAdress;
            endAdd = (*list)->endAdress;

            alocAux = *list;
            if(alocAux->next) {
                *list = alocAux->next;
                (*list)->prev = NULL;
            }
            else
                *list = NULL;

            free(alocAux);
        } else {
            while (alocAux->id != ID && alocAux->next)
                alocAux = alocAux->next;

            if (alocAux->id == ID) {
                startAdd = alocAux->startAdress;
                endAdd = alocAux->endAdress;

                alocAux->prev->next = alocAux->next;
                if (alocAux->next)
                    alocAux->next->prev = alocAux->prev;
                free(alocAux);
            }
            else {
                printf("--> Nó não encontrado!\n");
                system("read -p \"\nPressione enter para sair.\" saindo");
                return;
            }
        }
        allocatesFreeBlock(*&empty, startAdd, endAdd);
    }
    else {
        printf("--> Não há lista para desalocar!\n");
        system("read -p \"\nPressione enter para sair.\" saindo");
        return;
    }
}

int main(){
    unsigned M, choose=0;
    EmptyList *emptyList = NULL;
    AlocList *list = NULL;

    printf("--> Qual o tamanho M de bytes?\n");
    scanf("%u", &M);

    if(M<=0)
        M = 1;

    firstEmptyListInsert(&emptyList, M);
    gfx_init(1024, 768, "Linked List");

    while(choose!=3) {
        drawBlocks(emptyList, list, M);

        system("clear");
        printf("--> Deseja alocar[1], desalocar[2] um bloco de memória ou sair do programa[3]?\n");
        scanf("%u", &choose);

        switch(choose) {
            case 1:
                allocNewBlock(&list, &emptyList);
                break;
            case 2:
                deallocatesAllocatedBlock(&list, &emptyList);
                break;
            case 3:
                break;
            default:
                printf("--> Escolha uma opção válida!\n");
                system("read -p \"\nPressione enter para sair.\" saindo");
                break;
        }

        gfx_clear();
        gfx_paint();
    }

    AlocList *del1 = list;
    EmptyList *del2 = emptyList;

    while(list){
        list = list->next;
        free(del1);
        del1 = list;
    }
    while(emptyList){
        emptyList = emptyList->next;
        free(del2);
        del2 = emptyList;
    }

    printf("--> As memórias alocadas foram desalocadas com sucesso.\n");
    system("read -p \"\nPressione enter para sair.\" saindo");

	sleep(1);
	gfx_quit();

	return 0;
}
