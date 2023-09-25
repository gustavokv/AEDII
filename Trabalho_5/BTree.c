//Aluno proprietário: Gustavo Kermaunar Volobueff
//UEMS - Dourados/MS - rgm47006
//volobueffgustavo@gmail.com
//Algoritmos e Estruturas de Dados II

//ATENÇÃO: Não consegui fazer a GFX corretamente, mas acredito que com os prints que consegui fazer, irá dar para entender
//como a árvore está, com auxílio da GFX incorreta.

#include "gfx/gfx.h"
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#define SQUARE_SIZE 30 //Tamanho de cada quadrado de cada página para a gfx, onde o quadrado será 20px por 20px
#define Y_ADJUST 60 //Pixels que vão descer na representação gráfica de cada página

typedef struct Page{
    unsigned short int m;
    int *keys;
    struct Page **link;
}Page;

void drawOnePage(Page *ptraiz, int x, int y);
void drawBTree(Page *pt, float x, int y);
void buscaB(int x, Page **pt, unsigned short *f, unsigned short *g, Page *ptraiz);
void insertKeyInPageLeaf(Page **ptraiz, unsigned short g, int x);
void insertKeyInPage(Page **ptraiz, unsigned short g, int x);
void insertB(Page **page, int *x, unsigned short d, unsigned short *g, Page **pt, unsigned short *alreadyExists, 
unsigned short *inserted);
void validatesRoot(Page **ptraiz, unsigned short d);
void excludeKeyInPageLeaf(Page **ptraiz, unsigned short g);
void excludeKeyInPageNotLeaf(Page **ptraiz, int x, unsigned short lORr);
int searchForSuccessor(Page **ptraiz, unsigned short g);
void mergePagesLeft(Page **ptDest, Page *toMerge, Page *father, unsigned short posiFather);
void mergePagesRight(Page **ptDest, Page *toMerge, Page *father, unsigned short posiFather);
void excludeB(Page **page, int x, unsigned short d,  unsigned short *g, Page **pt, unsigned short *found,
unsigned short *excluded, unsigned short *stopRecursion);
void deallocateTree(Page **pt);

void printBTree(Page *ptraiz, int blanks){
    if(ptraiz){
        int i;
        
        for(i=1;i<=blanks;i++)
            printf(" ");
        for(i=0;i<ptraiz->m;i++)
            printf("%d ", ptraiz->keys[i]);
        printf("\n");
        for(i=0;i<=ptraiz->m;i++)
            printBTree(ptraiz->link[i], blanks+10);
    }
}

//Desenha somente a página, sem os valores
void drawOnePage(Page *ptraiz, int x, int y){
    if(ptraiz->m){
        for(unsigned short i=0;i<ptraiz->m;i++){
            gfx_rectangle(x + SQUARE_SIZE, y, x, y + SQUARE_SIZE);
            x += SQUARE_SIZE;
        }
    }
}

//x sempre inicia em 512 e y em 10
void drawBTree(Page *pt, float x, int y){
    if(!pt)
        return;

    int i, height, width, xAux;
    char msg[10];

    xAux = x;
    drawOnePage(pt, xAux, y);

    for(i=0;i<pt->m;i++){
        sprintf(msg, "%d", pt->keys[i]);
        gfx_get_text_size(msg, &width, &height);
        gfx_text(xAux + width/4, y + height/2, msg);
        xAux += SQUARE_SIZE;
    }
    
    for(i=0;i<=pt->m;i++){
        if(pt->m % 2 == 0 && i == pt->m/2)
            drawBTree(pt->link[i], x, y + Y_ADJUST);
        else if(i < floor((pt->m+1)/2))
            drawBTree(pt->link[i], x - x/2, y + Y_ADJUST);
        else if(i >= floor((pt->m+1)/2))
            drawBTree(pt->link[i], x + x/2, y + Y_ADJUST);
    }

    gfx_paint();
}

//Algoritmo de busca segundo o livro com algumas adaptações para C para evitar erros com ponteiros
void buscaB(int x, Page **pt, unsigned short *f, unsigned short *g, Page *ptraiz){
    unsigned short i;
    Page *p = ptraiz;
    *pt = NULL;
    *f = 0;

    while(p){
        i = *g = 0;
        *pt = p;    
        
        while(p && i < p->m){ 
            if(x > p->keys[i])
                i = *g = i+1;
            else if(x == p->keys[i]){
                p = NULL;
                *f = 1;
            }
            else{
                p = p->link[i];
                if(p) 
                    i = p->m + 2;
            }
        }
        
        if(p && i == p->m)
            p = p->link[p->m];
    }
}

void insertKeyInPageLeaf(Page **ptraiz, unsigned short g, int x){
    int auxArrayKeys[(*ptraiz)->m]; //Onde será armazenado os valores a frente da nova chave
    unsigned short j=0;

    for(unsigned short i=g;i<(*ptraiz)->m;i++)
        auxArrayKeys[j++] = (*ptraiz)->keys[i];

    //Inserção da nova chave
    (*ptraiz)->keys[g] = x;
    (*ptraiz)->m++;
    j=0;

    //Retorna à página as chaves
    for(unsigned short i=g+1;i<(*ptraiz)->m;i++)
        (*ptraiz)->keys[i+1] = auxArrayKeys[j];

    for(unsigned short i=0;i<=(*ptraiz)->m;i++)
        (*ptraiz)->link[i] = NULL;
}

//Já insere na página a chave na posição correta, corrigindo a posição também dos ponteiros para as próximas páginas
void insertKeyInPage(Page **ptraiz, unsigned short g, int x){
    int auxArrayKeys[(*ptraiz)->m]; //Onde será armazenado os valores a frente da nova chave
    Page *auxArrayPointers[(*ptraiz)->m+1]; //Da mesma forma, os ponteiros
    unsigned short j=0;

    for(unsigned short i=g;i<(*ptraiz)->m;i++){
        auxArrayKeys[j] = (*ptraiz)->keys[i];
        auxArrayPointers[j++] = (*ptraiz)->link[i+1];
    }

    //Inserção da nova chave
    (*ptraiz)->keys[g] = x;
    (*ptraiz)->m++;
    j=0;

    //Retorna à página as chaves
    for(unsigned short i=g+1;i<(*ptraiz)->m;i++){
        (*ptraiz)->keys[i] = auxArrayKeys[j];
        (*ptraiz)->link[i+1] = auxArrayPointers[j++];
    }
}

void insertB(Page **page, int *x, unsigned short d, unsigned short *g, Page **pt, unsigned short *alreadyExists, 
unsigned short *inserted){
    unsigned short firstConfirm=0;
    
    if(*page){ 
        *g = 0; //Realiza uma busca para ver se a chave já está na árvore, caso não esteja, prossegue a inserção
        while(*g < (*page)->m && !(*alreadyExists)){
            if((*page)->keys[*g] == *x){
                (*alreadyExists)++; 
                break;
            }
            else if((*page)->keys[*g] > *x){
                if((*page)->link[*g]){
                    insertB(&(*page)->link[(*g)], x, d, g, &(*page), alreadyExists, inserted);
                    break;
                }
                else{
                    (*g)++;
                    firstConfirm++;
                    break;
                }
            }
            else if((*page)->keys[*g] < *x && (*page)->keys[*g + 1] > *x){
                if((*page)->link[*g+1])
                    insertB(&(*page)->link[*g+1], x, d, g, &(*page), alreadyExists, inserted);
                else{
                    (*g)++;
                    break;
                }
            }
            else if(*g == (*page)->m-1){
                if((*page)->link[*g+1])
                    insertB(&(*page)->link[*g+1], x, d, g, &(*page), alreadyExists, inserted);
                else{
                    (*g)++;
                    break;
                }
            }
            else
                (*g)++;
        }
        
        //Caso a chave não esteja na árvore, não tenha já sido inserida (por conta das recursões)
        if(!(*alreadyExists) && !(*inserted)){
            if(firstConfirm){                            //Esse if é para caso seja o elemento na posição 0 do link, pois segundo a busca acima caso seja
                insertKeyInPageLeaf(&(*page), (*g)-1, *x); //O primeiro ele irá inserir incorretamente na posição 1, e essa parte
            }                                            //previne isso               
            else
                insertKeyInPageLeaf(&(*page), *g, *x);

            (*inserted)++;
        }
        
        //Verificação, caso na página haja mais chaves que o possível
        if((*page)->m > 2 * d){ 
            Page *newLeftPage = malloc(sizeof(Page)), *newRightPage = malloc(sizeof(Page));
            
            //Inicia a cisão separando uma página à esquerda e uma à direita para a chave que será promovida
            if(d>1){
                newLeftPage->keys = malloc((2 * d-1) * sizeof(int));
                newLeftPage->link = malloc((2 * d) * sizeof(Page)); 
                newRightPage->keys = malloc((2 * d-1) * sizeof(int));
                newRightPage->link = malloc((2 * d) * sizeof(Page)); 
            }
            else{
                newLeftPage->keys = malloc(1 * sizeof(int));
                newLeftPage->link = malloc((2 * d) * sizeof(Page)); 
                newRightPage->keys = malloc(1 * sizeof(int));
                newRightPage->link = malloc((2 * d) * sizeof(Page)); 
            }

            //Copia metade da página para a esquerda e metade para a direita, excluindo a chave central
            for(unsigned int i=0;i<floor((*page)->m/2);i++){
                newLeftPage->keys[i] = (*page)->keys[i];
                newLeftPage->link[i] = (*page)->link[i];
            }
            unsigned short posiLeftPageLast = floor((*page)->m/2);
            newLeftPage->link[posiLeftPageLast] = (*page)->link[posiLeftPageLast];

            unsigned int j=0;
            for(unsigned int i=floor((*page)->m/2)+1;i<(*page)->m;i++){
                newRightPage->keys[j] = (*page)->keys[i];
                newRightPage->link[j++] = (*page)->link[i];
            }
            newRightPage->link[j] = (*page)->link[(*page)->m];

            newLeftPage->m = floor((*page)->m/2);
            newRightPage->m = floor((*page)->m/2);

            //Obtém a chave do centro para ser promovido
            unsigned short middleElement = floor((*page)->m/2), posiInsertFather;

            //Obtém a posição onde será inserido a chave central, no seu antigo pai
            for(unsigned short i=0;i<(*pt)->m;i++){
                if((*page)->keys[middleElement] < (*pt)->keys[i]){
                    posiInsertFather = i;
                    break;
                }
                if((*pt)->keys[i] < (*page)->keys[middleElement] && (*pt)->keys[i+1] > (*page)->keys[middleElement]){
                    posiInsertFather = i+1;
                    break;
                }
                if(i == (*pt)->m-1){
                    posiInsertFather = i+1;
                    break;
                }
            }

            //Nessa parte ele faz a promoção da chave central e alinha as páginas esquerda e direita à ela
            if(*page == *pt){
                (*page)->keys[0] = (*page)->keys[middleElement];

                for(unsigned short i=1;i<(*page)->m;i++)
                    (*page)->keys[i] = 0;

                (*page)->m = 1;
                (*page)->link[0] = newLeftPage;
                (*page)->link[1] = newRightPage;
            }
            else{
                insertKeyInPage(&(*pt), posiInsertFather, (*page)->keys[middleElement]);
                (*pt)->link[posiInsertFather] = newLeftPage;
                (*pt)->link[posiInsertFather+1] = newRightPage;
            }
        }
    }
}

//Caso passe pela inserção e a raiz tenha mais que 2*d chaves
void validatesRoot(Page **ptraiz, unsigned short d){
    if((*ptraiz)->m > 2*d){
        Page *newLeftPage = malloc(sizeof(Page)), *newRightPage = malloc(sizeof(Page));
            
        //Inicia a cisão separando uma página à esquerda e uma à direita para a chave que será promovida
        if(d>1){
            newLeftPage->keys = malloc((2 * d-1) * sizeof(int));
            newLeftPage->link = malloc((2 * d) * sizeof(Page)); 
            newRightPage->keys = malloc((2 * d-1) * sizeof(int));
            newRightPage->link = malloc((2 * d) * sizeof(Page)); 
        }
        else{
            newLeftPage->keys = malloc(1 * sizeof(int));
            newLeftPage->link = malloc((2 * d) * sizeof(Page)); 
            newRightPage->keys = malloc(1 * sizeof(int));
            newRightPage->link = malloc((2 * d) * sizeof(Page)); 
        }

        //Copia metade da página para a esquerda e metade para a direita, excluindo a chave central
        for(unsigned int i=0;i<floor((*ptraiz)->m/2);i++){
            newLeftPage->keys[i] = (*ptraiz)->keys[i];
            newLeftPage->link[i] = (*ptraiz)->link[i];
        }
        unsigned short posiLeftPageLast = floor((*ptraiz)->m/2);
        newLeftPage->link[posiLeftPageLast] = (*ptraiz)->link[posiLeftPageLast];

        unsigned int j=0;
        for(unsigned int i=floor((*ptraiz)->m/2)+1;i<(*ptraiz)->m;i++){
            newRightPage->keys[j] = (*ptraiz)->keys[i];
            newRightPage->link[j++] = (*ptraiz)->link[i];
        }
        newRightPage->link[j] = (*ptraiz)->link[(*ptraiz)->m];

        newLeftPage->m = floor((*ptraiz)->m/2);
        newRightPage->m = floor((*ptraiz)->m/2);

        //Obtém a chave do centro para ser promovido
        unsigned short middleElement = floor((*ptraiz)->m/2);

        (*ptraiz)->keys[0] = (*ptraiz)->keys[middleElement];

        for(unsigned short i=1;i<(*ptraiz)->m;i++)
            (*ptraiz)->keys[i] = 0;

        (*ptraiz)->m = 1;
        (*ptraiz)->link[0] = newLeftPage;
        (*ptraiz)->link[1] = newRightPage;
    }
}

void excludeKeyInPageLeaf(Page **ptraiz, unsigned short g){
    int auxArrayKeys[(*ptraiz)->m]; //Onde será armazenado os valores a frente da chave deletada
    Page *auxArrayPointers[(*ptraiz)->m+1];
    unsigned short j=0;

    for(unsigned short i=g+1;i<(*ptraiz)->m;i++){
        auxArrayKeys[j] = (*ptraiz)->keys[i];
        auxArrayPointers[j++] = (*ptraiz)->link[i];
    }
    auxArrayPointers[j] = (*ptraiz)->link[(*ptraiz)->m];
    
    (*ptraiz)->m--;
    j=0;

    //Retorna à página as chaves
    for(unsigned short i=g;i<(*ptraiz)->m;i++){
        (*ptraiz)->keys[i] = auxArrayKeys[j];
        (*ptraiz)->link[i] = auxArrayPointers[j++];
    }
    (*ptraiz)->link[(*ptraiz)->m] = auxArrayPointers[j];
}

void excludeKeyInPageNotLeaf(Page **ptraiz, int x, unsigned short lORr){
    int auxArrayKeys[(*ptraiz)->m];
    Page *auxArrayPointers[(*ptraiz)->m+1];
    unsigned short j=0;

    for(unsigned short i=0;i<(*ptraiz)->m;i++){
        if((*ptraiz)->keys[i] != x)
            auxArrayKeys[j++] = (*ptraiz)->keys[i];
    }

    j=0;

    for(unsigned short i=0;i<=(*ptraiz)->m;i++){
        if((*ptraiz)->keys[i] == x){
            if(lORr){
                auxArrayPointers[j++] = (*ptraiz)->link[i];
                i++;
            }
            else{
                auxArrayPointers[j++] = (*ptraiz)->link[i+1];
                i++;
            }
        }
        else
            auxArrayPointers[j++] = (*ptraiz)->link[i];
    }

    (*ptraiz)->m--;
    j=0;

    for(unsigned short i=0;i<(*ptraiz)->m;i++){
        (*ptraiz)->keys[i] = auxArrayKeys[j];
        (*ptraiz)->link[i] = auxArrayPointers[j++];
    }
    (*ptraiz)->link[(*ptraiz)->m] = auxArrayPointers[j];
}

//Essa função substitui a chave removida pelo seu sucessor, além de corrigir na folha a quantidade m.
int searchForSuccessor(Page **ptraiz, unsigned short g){
    int returnValue;

    *ptraiz = (*ptraiz)->link[g+1];

    while((*ptraiz)->link[0])
        (*ptraiz) = (*ptraiz)->link[0];
    
    returnValue = (*ptraiz)->keys[0];
    excludeKeyInPageLeaf(&(*ptraiz), 0);

    return returnValue;
}

//Concatena duas páginas(Caso da remoção onde há um irmão à esquerda), inserindo a chave do nó pai junto.
void mergePagesLeft(Page **ptDest, Page *toMerge, Page *father, unsigned short posiFather){
    int auxPageKeys[(*ptDest)->m];
    Page *auxPagePointers[(*ptDest)->m + 1];
    unsigned short j=0, antM = (*ptDest)->m;

    for(unsigned short i=0;i<(*ptDest)->m;i++){
        auxPageKeys[i] = (*ptDest)->keys[i];
        auxPagePointers[i] = (*ptDest)->link[i]; 
    }
    auxPagePointers[(*ptDest)->m] = (*ptDest)->link[(*ptDest)->m];

    for(unsigned short i=0;i<toMerge->m;i++){
        (*ptDest)->keys[i] = toMerge->keys[i];
        (*ptDest)->link[i] = toMerge->link[i];
    }
    (*ptDest)->link[toMerge->m] = toMerge->link[toMerge->m];

    (*ptDest)->keys[toMerge->m] = father->keys[posiFather];
    (*ptDest)->m = toMerge->m + 1;

    for(unsigned short i = (*ptDest)->m;i < (*ptDest)->m + antM;i++){
        (*ptDest)->keys[i] = auxPageKeys[j];
        (*ptDest)->link[i] = auxPagePointers[j++];
    }

    (*ptDest)->m += antM;
    (*ptDest)->link[(*ptDest)->m] = auxPagePointers[antM];
}

//Concatena duas páginas(Caso da remoção onde há um irmão à direita), inserindo a chave do nó pai junto.
void mergePagesRight(Page **ptDest, Page *toMerge, Page *father, unsigned short posiFather){
    unsigned short j=0;

    (*ptDest)->keys[(*ptDest)->m] = father->keys[posiFather];

    for(unsigned short i=(*ptDest)->m+1;i<(*ptDest)->m + toMerge->m+1;i++){
        (*ptDest)->keys[i] = toMerge->keys[j];
        (*ptDest)->link[i] = toMerge->link[j++];
    }
    (*ptDest)->link[(*ptDest)->m + toMerge->m+1] = toMerge->link[toMerge->m];

    (*ptDest)->m += toMerge->m + 1;
}

void excludeB(Page **page, int x, unsigned short d,  unsigned short *g, Page **pt, unsigned short *found,
unsigned short *excluded, unsigned short *stopRecursion){
    unsigned short firstConfirm=0;

    if(*page){
        *g = 0;

        while(*g < (*page)->m && !(*found)){
            if((*page)->keys[*g] == x){
                (*found)++;
                break;
            }
            else if((*page)->keys[*g] > x){
                if((*page)->link[*g]){
                    excludeB(&(*page)->link[(*g)], x, d, g, &(*page), found, excluded, stopRecursion);
                    break; 
                }
                else{
                    (*g)++;
                    firstConfirm++;
                    break;
                }
            }
            else if((*page)->keys[*g] < x && (*page)->keys[*g + 1] > x){
                if((*page)->link[*g+1])
                    excludeB(&(*page)->link[*g+1], x, d, g, &(*page), found, excluded, stopRecursion);
                else{
                    (*g)++;
                    break;
                }
            }
            else if(*g == (*page)->m-1){
                if((*page)->link[*g+1])
                    excludeB(&(*page)->link[*g+1], x, d, g, &(*page), found, excluded, stopRecursion);
                else{
                    (*g)++;
                    break;
                }
            }
            else
                (*g)++;
        }

        if(*found && !(*excluded)){
            if(firstConfirm) //Caso esteja mais à esquerda das chaves
                *g -= 1;

            if(!(*page)->link[0])//Folha
                excludeKeyInPageLeaf(&(*page), *g);
            else{//Não folha
                Page *ptAux = *page;

                (*page)->keys[*g] = searchForSuccessor(&ptAux, *g);
                (*excluded)++;
                //Inicia a recursão até a folha em que a chave foi substituída
                excludeB(&(*page)->link[*g+1], (*page)->keys[*g], d, g, &(*page), found, excluded, stopRecursion);
            }

            (*excluded)++;
        }

        //Faz a recursão até a folha onde a chave foi substituída, caso não seja folha
        if((*page)->link[0] && !(*stopRecursion))
            excludeB(&(*page)->link[0], x, d, g, &(*page), found, excluded, stopRecursion);

        (*stopRecursion)++;

        //Caso na remoção a quantidade de chaves seja menor que a ordem
        if((*page)->m < d){
            if(*pt != *page){
                unsigned short g2;

                //Procura a posição da chave no pai que será inserido na página com a chave excluída
                for(unsigned short i=0;i<(*pt)->m;i++){
                    if((*pt)->keys[i] > x){
                        g2 = i;
                        break;
                    }
                    else if(i == (*pt)->m-1){
                        g2 = i+1;
                        break;
                    }
                    else if((*pt)->keys[i] < x && (*pt)->keys[i+1] > x){
                        g2 = i+1;
                        break;
                    }
                }
                
                //REDISTRIBUIÇÃO 
                //Irmão esquerdo da página que houve exclusão possui mais que o mínimo de chaves
                if(g2>0 && (*pt)->link[g2-1]->m > d){
                    if(g2 == (*pt)->m)
                        g2--;

                    insertKeyInPage(&(*page), 0, (*pt)->keys[g2]);
                    (*page)->link[0] = (*pt)->link[g2]->link[(*pt)->link[g2]->m];
                    (*pt)->keys[g2] = (*pt)->link[g2]->keys[(*pt)->link[g2]->m-1];
                    excludeKeyInPageLeaf(&(*pt)->link[g2], (*pt)->link[g2]->m-1);
                }
                else if(g2 != (*pt)->m && (*pt)->link[g2+1]->m > d){//Irmão direito da página que houve exclusão possui mais que o mínimo de chaves
                    insertKeyInPage(&(*page), (*page)->m, (*pt)->keys[g2]);
                    (*page)->link[(*page)->m] = (*pt)->link[g2+1]->link[0];
                    (*pt)->keys[g2] = (*pt)->link[g2+1]->keys[0];
                    excludeKeyInPageLeaf(&(*pt)->link[g2+1], 0);
                }
                else if(!(*pt)->link[g2+1] || (g2 > 0 && (*pt)->link[g2-1]->m <= (*pt)->link[g2+1]->m)){ //CONCATENAÇÃO
                    if(g2 == 0)
                        mergePagesLeft(&(*page), (*pt)->link[g2], *pt, g2);
                    else
                        mergePagesLeft(&(*page), (*pt)->link[g2-1], *pt, g2-1);

                    Page *ptAux = (*pt)->link[g2-1];
                    
                    free(ptAux);

                    if((*pt)->m == 1){
                        **pt = **page;
                    }
                    else
                        excludeKeyInPageNotLeaf(&(*pt), (*pt)->keys[g2-1], 0);
                }
                else if((*pt)->link[g2+1]){ //Caso o irmão direito da página com o nó deletado tenha menos chaves que o irmão esquerdo
                    if(g2 == 0 || g2 == (*pt)->m-1)
                        mergePagesRight(&(*page), (*pt)->link[g2+1], *pt, g2);
                    else
                        mergePagesRight(&(*page), (*pt)->link[g2+1], *pt, g2-1);

                    Page *ptAux = (*pt)->link[g2+1];

                    free(ptAux);

                    if(g2==0 || g2 == (*pt)->m-1)
                        g2++;
                    
                    if((*pt)->m == 1){
                        **pt = **page;
                    }
                    else
                        excludeKeyInPageNotLeaf(&(*pt), (*pt)->keys[g2-1], 1);
                }
            }
        }
    }
}

//Desalocação da árvore
void deallocateTree(Page **pt){
    if(*pt){
        for(unsigned short i=0;i<=(*pt)->m;i++)
            deallocateTree(&(*pt)->link[i]);

        free(*pt);
    }
}

int main(){
    unsigned short int mainChoose, f, posiPag, d, existsInsExc, insExcConfirm, stopExcludeRecursion;
    Page *ptraiz = malloc(sizeof(Page)), *ptAux = NULL, *lastPageAnalysed = NULL;
    int manipKey;

    do{
        printf("Qual a ordem (d) da árvore?\n--> ");
        scanf("%hu", &d);
    }while(d<1);

    //Definição das chaves, ponteiros e quantidade de chaves. 
    //Como estamos em C, a quantidade de chaves se dá por (2*d - 1) e os ponteiros por (2*d).
    if(d>1){
        ptraiz->keys = malloc((2 * d-1) * sizeof(int));
        ptraiz->link = malloc((2 * d) * sizeof(Page));
    }
    else{
        ptraiz->keys = malloc(1 * sizeof(int));
        ptraiz->link = malloc((2 * d) * sizeof(Page));
    }
    ptraiz->m = 0;

    gfx_init(1024, 768, "B-Tree");

    do{
        gfx_clear();
        printBTree(ptAux, 0);
        if(ptraiz->m != 0)
            drawBTree(ptraiz, 512, 10);
        else{
            gfx_clear();
            gfx_paint();
        }

        system("read -p \"\nPressione enter para sair.\" saindo");
        system("clear");

        printf("1- Inserção\n\
2- Remoção\n\
3- Busca\n\
4- Encerrar\n-->");
        scanf("%hu", &mainChoose);

        switch(mainChoose){
            case 1:
                system("clear");
                printf("Qual chave deseja inserir?\n-->");
                scanf("%d", &manipKey);

                insExcConfirm=0;
                existsInsExc=0;
                ptAux = ptraiz;
                insertB(&ptAux, &manipKey, d, &posiPag, &ptAux, &existsInsExc, &insExcConfirm);
                ptAux = ptraiz;
                validatesRoot(&ptAux, d);
                break;
            case 2:
                system("clear");
                printf("Qual chave deseja excluir?\n-->");
                scanf("%d", &manipKey);

                existsInsExc=0;
                insExcConfirm=0;
                stopExcludeRecursion=0;
                ptAux = ptraiz;
                excludeB(&ptAux, manipKey, d, &posiPag, &ptAux, &existsInsExc, &insExcConfirm, &stopExcludeRecursion);
                break;
            case 3:
                system("clear");
                printf("Qual chave deseja buscar?\n-->");
                scanf("%d", &manipKey);

                buscaB(manipKey, &lastPageAnalysed, &f, &posiPag, ptraiz);

                if(f)
                    printf("Chave %d encontrada na posição [%hu] da página.\n", manipKey, posiPag); 
                else
                    printf("Chave %d não encontrada. Local de inserção: [%hu] da página.\n", manipKey, posiPag);
                break;
            case 4:
                printf("Encerrando...\n\n");
                break;
            default:
                printf("Escolha uma opção válida.\n");
            break;
        }
    }while(mainChoose != 4);

    d = 0;
    deallocateTree(&ptraiz);
    printf("Árvore desalocada com sucesso.\n");

    sleep(2);
    gfx_quit();

    return 0;
}
