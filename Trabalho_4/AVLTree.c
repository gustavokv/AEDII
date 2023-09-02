//Aluno proprietário: Gustavo Kermaunar Volobueff
//UEMS - Dourados/MS - rgm47006
//volobueffgustavo@gmail.com
//Algoritmos e Estruturas de Dados II

#include "gfx/gfx.h"
#include <stdio.h>  /* printf */
#include <unistd.h> /* sleep */
#include <stdint.h>

#define NODE_RADIUS 9
#define NODE_POSITION_ADJUST 70
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

struct s_no{
    int32_t chave:28;
    int32_t bal:2;
    int32_t reservado:2; /* sem uso */
    struct s_no* esq;
    struct s_no* dir;
};

struct s_arq_no{
    int32_t chave:28;
    int32_t bal:2;
    uint32_t esq:1;
    uint32_t dir:1;
};

void drawTree(struct s_no *pt, float x, float y, float adjust);
void inicioNo(struct s_no **pt, int x);
void caso1(struct s_no **pt, unsigned short int *h);
void caso2(struct s_no **pt, unsigned short int *h);
void insAVL(int x, struct s_no **pt, unsigned short int *h);
void buscaAVL(int x, struct s_no **pt, unsigned short int *f);
void searchFather(struct s_no **father, struct s_no *T, struct s_no *u);
void transplant(struct s_no **T, struct s_no **u, struct s_no **v);
struct s_no* searchBiggest(struct s_no *pt);
struct s_no* successor(struct s_no *pt, int searchKey, unsigned short int f);
short int height(struct s_no *pt);
short int getBalance(struct s_no *pt);
short int max(short int a, short int b);
struct s_no *leftRotate(struct s_no *x);
struct s_no *rightRotate(struct s_no *y);
struct s_no* removeAVL(struct s_no *pt, int remKey);
void balanceUpdate(struct s_no **pt);
void insertArchiveInPt(FILE *treeFile, struct s_no **pt);
void arquiveStruct(struct s_no *pt, struct s_arq_no *arq);
void saveInTree(FILE **treeFile, struct s_no *pt);
void deallocate(struct s_no **pt);

void drawTree(struct s_no *pt, float x, float y, float adjust) {
	char stringKey[11], stringBal[3];
	int keyWidth, balWidth;

	if(pt){
		if(pt->esq){
			gfx_line(x, y, x - adjust, y + NODE_POSITION_ADJUST);
			drawTree(pt->esq, x - adjust, y + NODE_POSITION_ADJUST, adjust/2);
		}
		if(pt->dir){
			gfx_line(x, y, x + adjust, y + NODE_POSITION_ADJUST);
			drawTree(pt->dir, x + adjust, y + NODE_POSITION_ADJUST, adjust/2);
		}

		sprintf(stringKey, "%d", pt->chave);
		sprintf(stringBal, "%d", pt->bal);

		//Desenha o círculo da chave.
		gfx_set_color(0,0,0);
		gfx_filled_ellipse(x, y, NODE_RADIUS*2, NODE_RADIUS*2); //Faz com que a parte de dentro do nó seja preta pra linha não sobrepor.
		gfx_set_color(255,255,255);
		gfx_get_text_size(stringKey, &keyWidth, 0);
		gfx_ellipse(x, y, NODE_RADIUS*2, NODE_RADIUS*2);
		gfx_text(x-keyWidth/2, y-8, stringKey);

		//Desenha o balanço ao lado do círculo.
		gfx_set_color(0, 0, 0);
		gfx_filled_rectangle(x+20, y-NODE_RADIUS, x+40, y+NODE_RADIUS);
		gfx_get_text_size(stringBal, &balWidth, 0);
		gfx_set_color(255, 255, 255);
		gfx_rectangle(x+20, y-NODE_RADIUS, x+38, y+NODE_RADIUS);
		gfx_set_color(255, 0, 0);
		gfx_text(x + 20 + balWidth/2, y - 8, stringBal);

		gfx_set_color(255, 255, 255);

		gfx_paint();

	}
	else{ //Caso a árvore seja vazia, ele só imprime a tela limpa.
		gfx_clear();
		gfx_paint();
	}
}

//Algoritmo auxiliar.
void inicioNo(struct s_no **pt, int x){
	*pt = malloc(sizeof(struct s_no));

	(*pt)->esq = NULL;
	(*pt)->dir = NULL;
	(*pt)->chave = x;
	(*pt)->bal = 0;
}

//Algoritmo auxiliar para rotação ou dupla rotação à direita.
void caso1(struct s_no **pt, unsigned short int *h){
	struct s_no *ptu = (*pt)->esq;

	if(ptu->bal == -1){
		(*pt)->esq = ptu->dir;
		ptu->dir = *pt;
		(*pt)->bal = 0;
		*pt = ptu;
	}
	else{
		struct s_no *ptv = ptu->dir;

		ptu->dir = ptv->esq;
		ptv->esq = ptu;
		(*pt)->esq = ptv->dir;
		ptv->dir = *pt;

		if(ptv->bal == -1)
			(*pt)->bal = 1;
		else
			(*pt)->bal = 0;

		if(ptv->bal == 1)
			ptu->bal = -1;
		else
			ptu->bal = 0;

		*pt = ptv;
	}

	(*pt)->bal = 0;
	*h = 0;
}

//Algoritmo auxiliar para rotação ou dupla rotação à esquerda.
void caso2(struct s_no **pt, unsigned short int *h){
	struct s_no *ptu = (*pt)->dir;

	if(ptu->bal == 1){
		(*pt)->dir = ptu->esq;
		ptu->esq = *pt;
		(*pt)->bal = 0;
		*pt = ptu;
	}
	else{
		struct s_no *ptv = ptu->esq;

		ptu->esq = ptv->dir;
		ptv->dir = ptu;
		(*pt)->dir = ptv->esq;
		ptv->esq = *pt;

		if(ptv->bal == 1)
			(*pt)->bal = -1;
		else
			(*pt)->bal = 0;
		
		if(ptv->bal == -1)
			ptu->bal = 1;
		else
			ptu->bal = 0;
		
		*pt = ptv;
	}

	(*pt)->bal = 0;
	*h = 0;
}

//Algoritmo de inserção na árvore segundo o livro.
void insAVL(int x, struct s_no **pt, unsigned short int *h){
	if(*pt == NULL){
		inicioNo(&(*pt), x);
		*h = 1;
		return;
	}
	else if(x == (*pt)->chave)
		return;

	if(x < (*pt)->chave){
		insAVL(x, &(*pt)->esq, h);

		if(*h){
			switch((*pt)->bal){
				case 1:
					(*pt)->bal = 0;
					*h = 0;
					break;
				case 0:
					(*pt)->bal = -1;
					break;
				case -1:
					caso1(&(*pt), h);
					break;
			}
		}
	}
	else{
		insAVL(x, &(*pt)->dir, h);

		if(*h){
			switch((*pt)->bal){
				case -1:
					(*pt)->bal = 0;
					*h = 0;
					break;
				case 0:
					(*pt)->bal = 1;
					break;
				case 1:
					caso2(&(*pt), h);
					break;
			}
		}
	}
}

//Algoritmo de busca em Árvore Binária de Busca.
void buscaAVL(int x, struct s_no **pt, unsigned short int *f){
	if(*pt == NULL)
		*f = 0;
	else if((*pt)->chave == x)
		*f = 1;
	else if(x < (*pt)->chave){
		if((*pt)->esq == NULL)
			*f = 2;
		else{
			*pt = (*pt)->esq;
			buscaAVL(x, &(*pt), f);
		}
	}
	else if((*pt)->dir == NULL)
		*f = 3;
	else{
		*pt = (*pt)->dir;
		buscaAVL(x, &(*pt), f);
	}
}

//Encontra o pai do nó u para ajudar na remoção.
void searchFather(struct s_no **father, struct s_no *T, struct s_no *u) {
	if(T == u){ //Caso u seja a raiz, não possui pai.
		*father = NULL;
		return;
	}
	
	if(T->dir == u || T->esq == u){ //Caso o T ache o próximo que seja u, ele é o nó pai.
		*father = T;
		return;
	}
	else{
		if(T->chave < u->chave)
			searchFather(&(*father), T->dir, u);
		else
			searchFather(&(*father), T->esq, u);
	}
}

//Função Transplant do Tree-Delete (Remoção da BST).
void transplant(struct s_no **T, struct s_no **u, struct s_no **v){
	struct s_no *uFather;

	searchFather(&uFather, *T, *u);
	
	if(uFather == NULL)
		*T = *v;
	else if(*u == uFather->esq)
		uFather->esq = *v;
	else
		uFather->dir = *v;

	if(*v != NULL){
		struct s_no *vFather;
		searchFather(&vFather, *T, *v);
		vFather = uFather;
	}
}

struct s_no* searchBiggest(struct s_no *pt){
	if(pt){
		if(pt->dir)
			return searchBiggest(pt->dir);
		else
			return pt;
	}
	else
		return NULL;
}

struct s_no* successor(struct s_no *pt, int searchKey, unsigned short int f){
	struct s_no *auxPt = NULL;

	//Faz a verificação para ver se o valor pedido é o maior da árvore, se for, não há sucessor.
	auxPt = searchBiggest(pt); 
	if(auxPt->chave == searchKey)
		return NULL;

	auxPt = pt;
	buscaAVL(searchKey, &auxPt, &f);

	if(f != 1) 
		return NULL;

	if(auxPt->dir){ //Caso em que existe subárvore direita do nó de searchKey.
		auxPt = auxPt->dir;
		while(auxPt->esq)
			auxPt = auxPt->esq;

		return auxPt;
	}
	else{ //Caso o nó não tenha uma subárvore direita.
		struct s_no *successor = NULL;
		struct s_no *ancestor = pt;

		while(ancestor != auxPt){
			if(auxPt->chave < ancestor->chave){
				successor = ancestor;
				ancestor = ancestor->esq;
			}
			else
				ancestor = ancestor->dir;
		}

		return successor;
	}
}

//Retorna a altura do nó dado.
short int height(struct s_no *pt){
	if(pt == NULL)
		return 0;
	else{
		int left = height(pt->esq);
		int right = height(pt->dir);
		if(left > right)
			return left + 1;
		else
			return right + 1;	
	}
}

short int getBalance(struct s_no *pt){
    if (pt == NULL)
        return 0;

    return height(pt->esq) - height(pt->dir);
}

//Retorna o valor máximo entre dois valores.
short int max(short int a, short int b){
    return (a > b)? a : b;
}

//Função para rotacionar à esquerda, diferente da caso2 que também rotaciona, porém esta é utilizada
//para a remoção.
struct s_no *leftRotate(struct s_no *x){
    struct s_no *y = x->dir;
    struct s_no *T2 = y->esq;

	//Executa a rotação
    y->esq = x;
    x->dir = T2;
	
    x->bal = max(height(x->esq), height(x->dir))+1;
    y->bal = max(height(y->esq), height(y->dir))+1;
 
    return y;
}

//Função para rotacionar à direita, diferente da caso1 que também rotaciona, porém esta é utilizada
//para a remoção.
struct s_no *rightRotate(struct s_no *y){
    struct s_no *x = y->esq;
	struct s_no *T2 = x->dir;

	//Executa a rotação
	x->dir = y;
    y->esq = T2;
 
    y->bal = max(height(y->esq), height(y->dir))+1;
	x->bal = max(height(x->esq), height(x->dir))+1;
 
    return x;
}

//Algoritmo Tree-Delete da árvore binária de busca.
struct s_no* removeAVL(struct s_no *pt, int remKey){
	struct s_no *auxPt = pt;

	if(pt == NULL){
		printf("Chave não encontrada!\n");
		return pt;
	}

	//Faz a busca pela chave aqui dentro dessa função mesmo, para ajudar na volta da recursão, balanceando.
	if(remKey < pt->chave)
		pt->esq = removeAVL(pt->esq, remKey);
	else if(remKey > pt->chave)
		pt->dir = removeAVL(pt->dir, remKey);
	else{
		if(auxPt->esq == NULL) 
			transplant(&pt, &auxPt, &(auxPt)->dir);
		else if(auxPt->dir == NULL)
			transplant(&pt, &auxPt, &(auxPt)->esq);
		else{
			struct s_no *y, *yFather, *yrFather, *ylFather;
			
			y = successor(pt, remKey, 1);
			searchFather(&yFather, pt, y);
			
			if(yFather != auxPt){
				transplant(&pt, &y, &(y)->dir);
				y->dir = auxPt->dir;
				searchFather(&yrFather, pt, y->dir);
				yrFather = y;
			}

			transplant(&pt, &auxPt, &y);
			y->esq = auxPt->esq;
			searchFather(&ylFather, pt, y->esq);
			ylFather = y;
		}
	}

	//Caso o nó deletado seja único
	if(pt == NULL)
		return pt;

	pt->bal = 1 + max(height(pt->esq), height(pt->dir));

	//Coloco o balanço em uma variável auxiliar pois as verificações if e else iriam ocasionar em erros caso comparasse
	//diretamente
	short int balance = getBalance(pt); //Recebe o balanço para verificar se precisa de atualização.

	//Os 4 casos possíveis de rotação esquerda/direita.
	if(balance > 1 && getBalance(pt->esq) >= 0)
		return rightRotate(pt);
	if(balance > 1 && getBalance(pt->esq) < 0){
		pt->esq = leftRotate(pt->esq);
		return rightRotate(pt);
	}
	if(balance < -1 && getBalance(pt->dir) <= 0)
		return leftRotate(pt);
	if(balance < -1 && getBalance(pt->dir) > 0){
		pt->dir = rightRotate(pt->dir);
		return leftRotate(pt);
	}

	return pt;
}

//Atualiza o balanço da árvore.
void balanceUpdate(struct s_no **pt){
    (*pt)->bal = height((*pt)->dir) - height((*pt)->esq);

	if((*pt)->esq)
		balanceUpdate(&(*pt)->esq);
	if((*pt)->dir)
		balanceUpdate(&(*pt)->dir);
}

//Constrói a árvore que está no arquivo.
void insertArchiveInPt(FILE *treeFile, struct s_no **pt){
	struct s_arq_no *arq = malloc(sizeof(struct s_arq_no));
	struct s_no *ptAux;

	if(fread(arq, sizeof(struct s_arq_no), 1, treeFile)){
		(*pt)->chave = arq->chave;
		(*pt)->bal = arq->bal;
			
		if(arq->esq){
			struct s_no *newNodeEsq = malloc(sizeof(struct s_no));
			
			(*pt)->esq = newNodeEsq;
			ptAux = (*pt)->esq;
			insertArchiveInPt(treeFile, &ptAux);
		}
		else
			(*pt)->esq = NULL;

		if(arq->dir){
			struct s_no *newNodeDir = malloc(sizeof(struct s_no));
			
			(*pt)->dir = newNodeDir;
			ptAux = (*pt)->dir;
			insertArchiveInPt(treeFile, &ptAux);
		}
		else
			(*pt)->dir = NULL;
	}
	else
		*pt = NULL;
}

//Passa a struct do arquivo para a struct root.
void arquiveStruct(struct s_no *pt, struct s_arq_no *arq) {
	arq->chave = pt->chave;
	arq->bal = pt->bal;

	if(pt->esq)
		arq->esq = 1;
	else
		arq->esq = 0;

	if(pt->dir)
		arq->dir = 1;
	else
		arq->dir = 0;
}

//Salva em arquivo a árvore em pré-ordem.
void saveInTree(FILE **treeFile, struct s_no *pt) {
	struct s_arq_no arq;

	arquiveStruct(pt, &arq);

	fwrite(&arq, sizeof(struct s_arq_no), 1, *treeFile);

	if(pt->esq)
		saveInTree(&(*treeFile), pt->esq);
	if(pt->dir)
		saveInTree(&(*treeFile), pt->dir);
}

//Desaloca a árvore em pós-ordem.
void deallocate(struct s_no **pt) {
	if((*pt)->esq)
		deallocate(&(*pt)->esq);
	if((*pt)->dir)
		deallocate(&(*pt)->dir);
	
	free(*pt);
	*pt = NULL;
}

int main() {
	unsigned short int mainChoose, h, f, archiveConfirm;
	struct s_no *pt = NULL, *ptAux;
	int manipKey;
	char archiveName[20];
	FILE *treeFile;

	printf("Deseja carregar uma árvore de um arquivo binário ou criar uma nova árvore? [1]Carregar [2]Criar\n");
	scanf("%hu", &archiveConfirm);

	//Caso o usuário deseja carregar uma árvore já existente.
	if(archiveConfirm == 1){
		do{
			printf("Qual o nome do arquivo que deseja abrir? (Escreva com a extensão .bin).\n");
			scanf("%s", archiveName);

			treeFile = fopen(archiveName, "rb"); 

			if(!treeFile){
				printf("O arquivo não foi encontrado! Digite novamente o nome.\n");
				archiveName[0] = '\0';
			}
		} while(!treeFile);

		pt = malloc(sizeof(struct s_no));
		ptAux = pt;
		insertArchiveInPt(treeFile, &ptAux);
		if(!ptAux)
			pt = NULL;
		fclose(treeFile);

		printf("Árvore carregada com sucesso!\n");
		archiveName[0] = '\0'; //Zera o array para poder usar na parte de guardar o arquivo novamnete sem dar bugs.
	}
	else
		printf("Modo de criação de nova árvore.\n");

	gfx_init(WINDOW_WIDTH, WINDOW_HEIGHT, "Árvore AVL");

	do{
		gfx_clear();

		gfx_set_color(255, 255, 255);
		drawTree(pt, WINDOW_WIDTH/2, 50, WINDOW_WIDTH/4);

		//Legenda no canto esquerdo superior da tela.
		gfx_filled_rectangle(2, 5, 13, 16);
		gfx_text(15, 2, "Chave");
		gfx_set_color(255, 0, 0);
		gfx_filled_rectangle(2, 20, 13, 31);
		gfx_text(15, 18, "Balanco");
		gfx_paint();

		system("read -p \"\nPressione enter para sair.\" saindo");
		system("clear");

		printf("[1] Busca\n\
[2] Inserção\n\
[3] Remoção\n\
[4] Encerrar a edição desta árvore.\n--> ");
		scanf("%hu", &mainChoose);

		switch(mainChoose){
			case 1:
				system("clear");
				printf("Qual a chave deseja procurar?\n--> ");
				scanf("%d", &manipKey);
				
				ptAux = pt;
				buscaAVL(manipKey, &ptAux, &f);

				switch(f) {
					case 0:
						printf("\n-> A árvore está vazia!\n");
						break;
					case 1:
						printf("\n-> Nó encontrado!\n");
						break;
					case 2:
						printf("\n-> Não foi encontrado à esquerda.\n");
						break;
					case 3:
						printf("\n-> Não foi encontrado à direita.\n");
						break;
				}
				break;
			case 2:
				system("clear");
				printf("Qual a chave do nó que deseja inserir?\n--> ");
				scanf("%d", &manipKey);
				h = 0;
				insAVL(manipKey, &pt, &h);
				break;
			case 3:
				system("clear");
				printf("Qual a chave do nó que deseja remover?\n--> ");
				scanf("%d", &manipKey);

				pt = removeAVL(pt, manipKey);
				if(pt) 
				 	balanceUpdate(&pt);
				break;
			case 4:
				printf("Encerrando edição da árvore...\n");
				break;
			default:
				printf("Digite uma opção válida!\n");
				break;
		}
	}while(mainChoose != 4);

	system("clear");

	printf("Deseja guardar esta árvore em arquivo binário? [1]Sim [2]Não\n");
	scanf("%hu", &archiveConfirm);

	if(archiveConfirm == 1){
		printf("Qual o nome deseja dar ao arquivo? (Escreva com a extensão .bin).\n");
		scanf("%s", archiveName);

		treeFile = fopen(archiveName, "wb");
		if(pt)
			saveInTree(&treeFile, pt);
		
		fclose(treeFile);
		
		printf("Árvore guardada com sucesso!\n");
	}

	if(pt){
		deallocate(&pt);
		printf("Árvore desalocada com sucesso.\n");
		drawTree(pt, WINDOW_WIDTH/2, 90, WINDOW_WIDTH/4);
	}

	printf("Saindo do programa...\n");

	sleep(2);
	gfx_quit();

	return 0;
}
