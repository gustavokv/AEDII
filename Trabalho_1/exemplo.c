#include "gfx/gfx.h"
#include <stdio.h>  /* printf */
#include <unistd.h> /* sleep  */
#include <stdlib.h> /* system clear */
#include <math.h> /* sqrt */

void CreateRecursionCircle(int centerX, int centerY, int radiusX, int radiusY, int recLevel, int i){
    if(i==0) {
        gfx_set_color(255, 255, 255);
        gfx_ellipse(centerX, centerY, radiusX, radiusY); //Gera o círculo maior, idependente da quantidade de recursões.
    }
    if(recLevel==0)
        return; //Para a execução caso a recursividade seja 0, para não construir os círculos da recursividade 1.

    double x1 = radiusX/2 * sqrt(2)/2 + centerX; //Coloca o centro X no lado direito do círculo maior a ele.
    double y1 = radiusY/2 * sqrt(2)/2 + centerY; //Coloca o centro Y na parte de baixo do círculo maior a ele.
    double x2 = x1 + (centerX - x1)*2; //Leva o centro X no lado esquerdo do círculo maior.
    double y2 = y1 + (centerY - y1)*2; //Leva o centro Y para "cima" do círculo maior.

    gfx_ellipse(x1, y1, radiusX/2, radiusY/2);
    gfx_ellipse(x1, y2, radiusX/2, radiusY/2);
    gfx_ellipse(x2, y1, radiusX/2, radiusY/2);
    gfx_ellipse(x2, y2, radiusX/2, radiusY/2);

    if(i<recLevel-1) {
        CreateRecursionCircle(x1, y1, radiusX/2, radiusY/2, recLevel, ++i);
        CreateRecursionCircle(x1, y2, radiusX/2, radiusY/2, recLevel+1, ++i);
        CreateRecursionCircle(x2, y1, radiusX/2, radiusY/2, recLevel+2, ++i);
        CreateRecursionCircle(x2, y2, radiusX/2, radiusY/2, recLevel+3, ++i);
    }
}

void CreateRecursionSquare(int x, int y, int size, int recLevel, int i){
    if(i<recLevel) {
        CreateRecursionSquare(x + size / 2, y - size, size / 2, recLevel, ++i);//Cria o quadrado para cima.
        CreateRecursionSquare(x + size / 2, y + size * 2, size / 2, recLevel+1, ++i);//Quadrado para baixo.
        CreateRecursionSquare(x + size * 2, y + size / 2, size / 2, recLevel+2, ++i);//Quadrado para a direita.
        CreateRecursionSquare(x - size, y + size/2, size / 2, recLevel+3, ++i);//Quadrado para a esquerda.
    }

    gfx_set_color(255,255,255);
    gfx_rectangle(x, y, x+size*2, y+size*2);//Com o "+size*2" ele coloca o x2 e y2 corretamente para o tamanho do quadrado, com base no x1 e y1.
    gfx_set_color(0, 0, 0);
    gfx_filled_rectangle(x+1, y+1, x+size*2-1, y+size*2-1);//Preencher dentro dos quadrados brancos para não ficar bugado, com quadrado dentro de quadrados.
}

int main(){
    int choose=0, recLevel=0;

    printf("\nVoce deseja um CIRCULO [1] ou QUADRADO [2]?\n");
    scanf("%d", &choose);
    printf("\nE qual o nivel de recursao [0-10]?\n");
    scanf("%d", &recLevel);

    while(recLevel<0||recLevel>10){
        system("clear");

        printf("\nO valor deve estar entre 0 e 10! Digite novamente:\n");
        scanf("%d", &recLevel);
    }

    if(choose==1) {
        gfx_init(600, 600, "Círculo Recursivo");
        CreateRecursionCircle(300, 300, 256, 256, recLevel, 0);
        gfx_paint();
    }
    else {
        gfx_init(600, 600, "Quadrado Recursivo");
        CreateRecursionSquare(200, 200, 100, recLevel, 0);
        gfx_paint();
    }

	sleep(5);
	gfx_quit();

	return 0;
}
