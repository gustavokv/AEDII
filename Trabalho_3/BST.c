//Aluno proprietário: Gustavo Kermaunar Volobueff - rgm47006

#include "gfx/gfx.h"
#include <stdio.h>  /* printf */
#include <unistd.h> /* sleep  */
#include <stdint.h>

#define NODE_RADIUS 9
#define NODE_POSITION_ADJUST 70

struct s_arq_no{
    int32_t chave:30;
    uint32_t esq:1;
    uint32_t dir:1;
};

typedef struct Root{
	int key;
	struct Root *left, *right;
}Root;

void drawTree(Root *root, float x, float y, float adjust);
Root* searchSmallest(Root *root);
Root* searchBiggest(Root *root);
void searchNode(Root **root, int key, unsigned *f);
Root* successor(Root *root, int searchKey, unsigned f);
Root* predeccessor(Root *root, int manipkey, unsigned f);
void insertNode(Root **root, int newKey, unsigned f);
void searchFather(Root **father, Root *T, Root *u);
void transplant(Root **T, Root **u, Root **v);
void removeNode(Root **root, int remKey);
void arquiveStruct(Root *root, struct s_arq_no *arq);
void saveInTree(FILE **tree, Root *root);
void insertArchiveInRoot(FILE *tree, Root **root);
void deallocate(Root **root);

void drawTree(Root *root, float x, float y, float adjust) {
	char stringKey[4];
	int width;

	if(root){
		if(root->left){
			gfx_line(x, y, x - adjust, y + NODE_POSITION_ADJUST);
			drawTree(root->left, x - adjust, y + NODE_POSITION_ADJUST, adjust/2);
		}
		if(root->right){
			gfx_line(x, y, x + adjust, y + NODE_POSITION_ADJUST);
			drawTree(root->right, x + adjust, y + NODE_POSITION_ADJUST, adjust/2);
		}


		sprintf(stringKey, "%d", root->key);
		gfx_set_color(0,0,0);
		gfx_filled_ellipse(x, y, NODE_RADIUS*2, NODE_RADIUS*2); //Faz com que a parte de dentro do nó seja preta pra linha não sobrepor.
		gfx_set_color(255,255,255);
		gfx_get_text_size(stringKey, &width, 0);
		gfx_ellipse(x, y, NODE_RADIUS*2, NODE_RADIUS*2);
		gfx_text(x-width/2, y-8, stringKey);

		gfx_paint();

	}
	else{ //Caso a árvore seja vazia, ele só imprime a tela limpa.
		gfx_clear();
		gfx_paint();
	}
}

Root* searchSmallest(Root *root){
	if(root){
		if(root->left)
			return searchSmallest(root->left);
		else
			return root;
	}
	else
		return NULL;

}

Root* searchBiggest(Root *root){
	if(root){
		if(root->right)
			return searchBiggest(root->right);
		else
			return root;
	}
	else
		return NULL;
}

//Altera ponteiro para o nó e o valor de f.
void searchNode(Root **root, int key, unsigned *f){

	if(*root == NULL)
		*f = 0;
	else if((*root)->key == key)
		*f = 1;
	else if(key < (*root)->key){
		if((*root)->left == NULL)
			*f = 2;
		else{
			*root = (*root)->left;
			searchNode(&(*root), key, f);
		}
	}
	else if((*root)->right == NULL)
		*f = 3;
	else{
		*root = (*root)->right;
		searchNode(&(*root), key, f);
	}
}

Root* successor(Root *root, int searchKey, unsigned f){
	Root *auxRoot = NULL;

	//Faz a verificação para ver se o valor pedido é o maior da árvore, se for, não há sucessor.
	auxRoot = searchBiggest(root); 
	if(auxRoot->key == searchKey)
		return NULL;

	auxRoot = root;
	searchNode(&auxRoot, searchKey, &f);

	if(f != 1) 
		return NULL;

	if(auxRoot->right){ //Caso em que existe subárvore direita do nó de searchKey.
		auxRoot = auxRoot->right;
		while(auxRoot->left)
			auxRoot = auxRoot->left;

		return auxRoot;
	}
	else{ //Caso o nó não tenha uma subárvore direita.
		Root *successor = NULL;
		Root *ancestor = root;

		while(ancestor != auxRoot){
			if(auxRoot->key < ancestor->key){
				successor = ancestor;
				ancestor = ancestor->left;
			}
			else
				ancestor = ancestor->right;
		}

		return successor;
	}
}

Root* predeccessor(Root *root, int searchKey, unsigned f) {
	Root *auxRoot = NULL;

	//Se o valor for o menor da árvore, não há predecessor.
	auxRoot = searchSmallest(root);
	if(auxRoot->key == searchKey)
		return NULL;
	
	auxRoot = root;
	searchNode(&auxRoot, searchKey, &f);

	if(f != 1)
		return NULL;

	if(auxRoot->left){ //Caso em que possui a subárvore esquerda.
		auxRoot = auxRoot->left;

		while(auxRoot->right)
			auxRoot = auxRoot->right;
		
		return auxRoot;
	}
	else{ //Caso em que não possui a subárvore esquerda.
		Root *predeccessor = NULL;
		Root *ancestor = root;

		while(ancestor != auxRoot){
			if(auxRoot->key > ancestor->key){
				predeccessor = ancestor;
				ancestor = ancestor->right;
			}
			else
				ancestor = ancestor->left;
		}

		return predeccessor;
	}
}

void insertNode(Root **root, int newKey, unsigned f){
	Root *auxRoot = *root;
	
	searchNode(&auxRoot, newKey, &f);

	if(f == 1)
		printf("Inserção inválida.\n");	
	else{
		Root *newNode = malloc(sizeof(Root));
		newNode->key = newKey;
		newNode->left = NULL;
		newNode->right = NULL;
	
		if(f == 0)
			*root = newNode;
		else if(f == 2)
			auxRoot->left = newNode;
		else
			auxRoot->right = newNode;
	}
}

//Encontra o pai do nó u para ajudar na remoção.
void searchFather(Root **father, Root *T, Root *u) {
	if(T == u){ //Caso u seja a raiz, não possui pai.
		*father = NULL;
		return;
	}
	
	if(T->right == u || T->left == u){ //Caso o T ache o próximo que seja u, ele é o nó pai.
		*father = T;
		return;
	}
	else{
		if(T->key < u->key)
			searchFather(&(*father), T->right, u);
		else
			searchFather(&(*father), T->left, u);
	}
}

//Função Transplant do Tree-Delete.
void transplant(Root **T, Root **u, Root **v){
	Root *uFather;

	searchFather(&uFather, *T, *u);
	
	if(uFather == NULL)
		*T = *v;
	else if(*u == uFather->left)
		uFather->left = *v;
	else
		uFather->right = *v;

	if(*v != NULL){
		Root *vFather;
		searchFather(&vFather, *T, *v);
		vFather = uFather;
	}
}

//Tentei fazer este código igual o Tree-Delete do livro.
void removeNode(Root **root, int remKey){
	Root *auxRoot = *root;
	unsigned f;

	searchNode(&auxRoot, remKey, &f);

	if(f != 1){
		printf("Chave não encontrada!\n");
		return;
	}

	if(auxRoot->left == NULL) 
		transplant(&(*root), &auxRoot, &(auxRoot)->right);
	else if(auxRoot->right == NULL)
		transplant(&(*root), &auxRoot, &(auxRoot)->left);
	else{
		Root *y, *yFather, *yrFather, *ylFather;
		
		y = successor(*root, remKey, 1);
		searchFather(&yFather, *root, y);
		
		if(yFather != auxRoot){
			transplant(&(*root), &y, &(y)->right);
			y->right = auxRoot->right;
			searchFather(&yrFather, *root, y->right);
			yrFather = y;
		}

		transplant(&(*root), &auxRoot, &y);
		y->left = auxRoot->left;
		searchFather(&ylFather, *root, y->left);
		ylFather = y;
	}
}

//Passa a struct do arquivo para a struct root.
void arquiveStruct(Root *root, struct s_arq_no *arq) {
	arq->chave = root->key;

	if(root->left)
		arq->esq = 1;
	else
		arq->esq = 0;

	if(root->right)
		arq->dir = 1;
	else
		arq->dir = 0;
}


//Salva em arquivo a árvore em pré-ordem.
void saveInTree(FILE **tree, Root *root) {
	struct s_arq_no arq;

	arquiveStruct(root, &arq);

	fwrite(&arq, sizeof(struct s_arq_no), 1, *tree);

	if(root->left)
		saveInTree(&(*tree), root->left);
	if(root->right)
		saveInTree(&(*tree), root->right);
}

//Constrói a árvore que está no arquivo.
void insertArchiveInRoot(FILE *tree, Root **root){
	struct s_arq_no *arq = malloc(sizeof(struct s_arq_no));
	Root *auxRoot;

	if(fread(arq, sizeof(struct s_arq_no), 1, tree)){
		(*root)->key = arq->chave;
			
		if(arq->esq){
			Root *newNodeEsq = malloc(sizeof(Root));
			
			(*root)->left = newNodeEsq;
			auxRoot = (*root)->left;
			insertArchiveInRoot(tree, &auxRoot);
		}
		else
			(*root)->left = NULL;

		if(arq->dir){
			Root *newNodeDir = malloc(sizeof(Root));
			
			(*root)->right = newNodeDir;
			auxRoot = (*root)->right;
			insertArchiveInRoot(tree, &auxRoot);
		}
		else
			(*root)->right = NULL;
	}
	else
		*root = NULL;
}

//Desaloca a árvore em pós-ordem.
void deallocate(Root **root) {
	if((*root)->left)
		deallocate(&(*root)->left);
	if((*root)->right)
		deallocate(&(*root)->right);
	
	free(*root);
	*root = NULL;
}

int main() {
	int manipKey;
	Root *root = NULL, *auxRoot, *auxSS, *auxBP; //auxSS e auxBP servem para receber tanto o maior e menor, sucessor e predecessor.
	unsigned f;
	short int archiveConfirm, loadOrCreate, mainChoose;
	char archiveName[20];
	FILE *tree;

	gfx_init(1024, 768, "Árvore Binária de Busca");

	printf("Deseja carregar uma árvore de um arquivo binário ou criar uma nova árvore? [1]Carregar [2]Criar\n");
	scanf("%hi", &loadOrCreate);

	//Caso o usuário deseja carregar uma árvore já existente.
	if(loadOrCreate == 1){
		do{
			printf("Qual o nome do arquivo que deseja abrir? (Escreva com a extensão .bin).\n");
			scanf("%s", archiveName);

			tree = fopen(archiveName, "rb"); 

			if(!tree){
				printf("O arquivo não foi encontrado! Digite novamente o nome.\n");
				archiveName[0] = '\0';
			}
		} while(!tree);

		root = malloc(sizeof(Root));
		auxRoot = root;
		insertArchiveInRoot(tree, &auxRoot);
		if(!auxRoot)
			root = NULL;
		fclose(tree);

		printf("Árvore carregada com sucesso!\n");
		archiveName[0] = '\0'; //Zera o array para poder usar na parte de guardar o arquivo novamnete sem dar bugs.
	}
	else
		printf("Modo de criação de nova árvore.\n");

	do{
		gfx_clear();
		drawTree(root, 512, 50, 256);
		system("read -p \"\nPressione enter para sair.\" saindo");
		system("clear");

		printf("[1] Busca pela maior e menor chave na árvore;\n\
[2] Busca por uma chave com valor x;\n\
[3] Busca pelo sucessor e predecessor de uma chave com valor x;\n\
[4] Inserção de uma chave com valor x;\n\
[5] Remoção de uma chave com valor x;\n\
[6] Encerrar edição desta árvore\n-> ");
		scanf("%hi", &mainChoose);

		switch(mainChoose){
			case 1:
				system("clear");
				auxRoot = root;
				auxSS = searchSmallest(auxRoot);
				auxBP = searchBiggest(auxRoot);

				if(root)
					printf("O menor elemento é %d e o maior é %d.\n", auxSS->key, auxBP->key);
				else
					printf("A árvore está vazia!\n");

				break;
			case 2:
				system("clear");

				printf("Qual a chave deseja procurar?\n");
				scanf("%d", &manipKey);
				auxRoot = root;
				searchNode(&auxRoot, manipKey, &f);

				switch(f){
					case 0:
						printf("-> A árvore está vazia!\n");
						break;
					case 1:
						printf("-> Nó encontrado!\n");
						break;
					case 2:
						printf("-> Não foi encontrado à esquerda.\n");
						break;
					case 3:
						printf("-> Não foi encontrado à direita.\n");
						break;
				}
				break;
			case 3:
				system("clear");
				printf("Qual a chave deseja procurar o sucessor e predecessor?\n");
				scanf("%d", &manipKey);
				auxRoot = root;

				if(!auxRoot)
					printf("Árvore vazia!\n");
				else{
					auxSS = successor(auxRoot, manipKey, f);
					auxBP = predeccessor(auxRoot, manipKey, f);

					if(!auxSS)
						printf("Erro ao encontrar o sucessor!\n");
					else
						printf("O sucessor de %d é %d.\n", manipKey, auxSS->key);
					
					if(!auxBP)
						printf("Erro ao encontrar o predecessor!\n");
					else
						printf("O predecessor de %d é %d.\n", manipKey, auxBP->key);
				}

				break;
			case 4:
				system("clear");
				printf("Qual a chave deseja inserir?\n");
				scanf("%d", &manipKey);
				insertNode(&root, manipKey, f);
				break;
			case 5:
				system("clear");
				printf("Qual a chave deseja remover?\n");
				scanf("%d", &manipKey);
				removeNode(&root, manipKey);
				break;
			case 6:
				printf("Saindo da edição da árvore atual...\n");
				break;
			default:
				printf("Escolha uma opção válida!\n");
				break;
		}
	} while(mainChoose != 6);

	system("clear");

	printf("Deseja guardar esta árvore em arquivo binário? [1]Sim [2]Não\n");
	scanf("%hi", &archiveConfirm);

	if(archiveConfirm == 1){
		printf("Qual o nome deseja dar ao arquivo? (Escreva com a extensão .bin).\n");
		scanf("%s", archiveName);

		tree = fopen(archiveName, "ab");
		if(root){
			saveInTree(&tree, root);
			fclose(tree);
		}
		else
			fclose(tree);
		
		printf("Árvore guardada com sucesso!\n");
	}

	if(root){
		deallocate(&root);
		printf("Árvore desalocada com sucesso.\n");
		drawTree(root, 512, 90, 256);
	}

	printf("Saindo do programa...\n");

	sleep(2);
	gfx_quit();

	return 0;
}
