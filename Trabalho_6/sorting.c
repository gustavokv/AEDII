//Aluno proprietário: Gustavo Kermaunar Volobueff
//UEMS - Dourados/MS - rgm47006
//volobueffgustavo@gmail.com
//Algoritmos e Estruturas de Dados II

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <math.h>

struct stat finfo;

void trocar(int32_t *L, int32_t *M){
    int32_t aux = *L;
    *L = *M;
    *M = aux; 
}

void originalBubbleSort(int32_t *L, int n){
    for(int i=0;i<n;i++){
        for(int j=0;j<n-1;j++){
            if(L[j]>L[j+1])
                trocar(&L[j], &L[j+1]);
        }
    }
}

void betterBubbleSort(int32_t *L, int n){
    int mudou=1, j, N=n-1, guarda=n;

    while(mudou){
        j = 0;
        mudou = 0;

        while(j<N){
            if(L[j] > L[j+1]){
                trocar(&L[j], &L[j+1]);
                mudou = 1;
                guarda = j;
            }
            j++;
        }
        N = guarda;
    }
}

void insertionSort(int32_t *L, int n){
    int32_t prov, valor;
    int j;

    for(int i=1;i<n;i++){
        prov = L[i];
        valor = L[i];
        j = i-1;
        
        while(j >= 0 && valor < L[j]){
            L[j+1] = L[j];
            j = j - 1;
        }
        L[j+1] = prov;
    }
}

void intercalar(int32_t *L, int ini1, int ini2, int fim2){
    int fim1 = ini2+1, nro=0, ind = ini1, tam = fim2 - ini1 + 1, start = ini1;
    int32_t *Tmp = (int32_t*)malloc(tam*sizeof(int32_t));

    while(start<=ini2 && fim1<=fim2){
        if(L[start] < L[fim1]){
            Tmp[nro] = L[start];
            start++;
        }
        else{
            Tmp[nro] = L[fim1];
            fim1++;
        }
        ind++;
        nro++;
    }
    while(start<=ini2){
        Tmp[nro] = L[start];
        start++;
        ind++;
        nro++;
    }
    while(fim1<=fim2){
        Tmp[nro] = L[fim1];
        fim1++;
        ind++;
        nro++;
    }
    for(int i=ini1;i<=fim2;i++)
        L[i] = Tmp[i-ini1];

    free(Tmp);
}

void mergeSort(int32_t *L, int esq, int dir){
    if(esq<dir){
        int centro = (esq+dir)/2;
        mergeSort(L, esq, centro);
        mergeSort(L, centro+1, dir);
        intercalar(L, esq, centro, dir);
    }
}

void lastElementQuickSort(int32_t *L, int ini, int fim){
    int i, j, key;

    if(fim - ini < 2){
        if(fim - ini == 1){
            if(L[ini]>L[fim])
                trocar(&L[ini], &L[fim]);
        }
    }
    else{
        i = ini;
        j = fim-1;
        key = fim;

        while(j>=i){
            while(L[i] < L[key])
                i++;
            while(j>=ini && L[j] >= L[key])
                j--;

            if(j>=i){
                trocar(&L[i], &L[j]);
                i++;
                j--;
            }
        }
        trocar(&L[i], &L[key]);

        if(ini < i-1)
            lastElementQuickSort(L, ini, i-1);
        if(i+1 < fim)
            lastElementQuickSort(L, i+1, fim);
    }
}

void randomElementQuickSort(int32_t *L, int ini, int fim){
    int i, j, key;

    if(fim - ini < 2){
        if(fim - ini == 1){
            if(L[ini]>L[fim])
                trocar(&L[ini], &L[fim]);
        }
    }
    else{
        srand(time(NULL));
        int random = ini + rand() % (fim-ini); 
        
        trocar(&L[random], &L[fim]);
        i = ini;
        j = fim-1;
        key = L[fim];

        while(j>=i){
            while(L[i] < key)
                i++;
            while(j >= ini && L[j] >= key)
                j--;

            if(j>=i){
                trocar(&L[i], &L[j]);
                i++;
                j--;
            }
        }
        trocar(&L[i], &L[fim]);
        
        if(ini < i-1)
            randomElementQuickSort(L, ini, i-1);
        if(i+1 < fim)
            randomElementQuickSort(L, i+1, fim);
    }
}

//Obtém o pivô do array por mediana de 3, especificamente para o QuickSort.
void PIVO(int32_t *L, int ini, int fim, int *mediana){
    int mid = floor((ini+fim)/2);

    if((L[ini] > L[mid]) ^ (L[ini] > L[fim]))
        *mediana = ini;
    else if((L[mid] < L[ini]) ^ (L[mid] < L[fim]))
        *mediana = mid;
    else
        *mediana = fim; 
}

void medianQuickSort(int32_t *L, int ini, int fim){
    int i, j, key;

    if(fim - ini < 2){
        if(fim - ini == 1){
            if(L[ini]>L[fim])
                trocar(&L[ini], &L[fim]);
        }
    }
    else{
        int mediana;

        PIVO(L, ini, fim, &mediana);
        trocar(&L[mediana], &L[fim]);
        i = ini;
        j = fim-1;
        key = L[fim];

        while(j>=i){
            while(L[i] < key)
                i++;
            while(L[j] > key)
                j--;

            if(j>i){
                trocar(&L[i], &L[j]);
                i++;
                j--;
            }
            else
                break;
        }
        trocar(&L[i], &L[fim]);

        medianQuickSort(L, ini, i-1);
        medianQuickSort(L, i+1, fim); 
    }
}

void descer(int32_t *L, int i, int n){
    int j = 2 * i;

    if(j<=n){
        if(j<n){
            if(L[j+1] > L[j])
                j++;
        }
        if(L[i] < L[j]){
            trocar(&L[i], &L[j]);
            descer(L, j, n);
        }
    }
}

void arranjar(int32_t *L, int n){
    for(int i=n/2;i>=0;i--)
        descer(L, i, n-1);
}

void heapSort(int32_t *L, int n){
    arranjar(L, n);
    int m = n-1;

    while(m>=0){
        trocar(&L[0], &L[m]);
        m--;
        descer(L, 0, m);
    }
}

int main(int argc, char *argv[]){
    if(argc == 4){
        unsigned short choosenAlg = atoi(argv[1]);
        char *inArchName = argv[2];
        char *outArchName = argv[3];

        FILE *inArchive, *outArchive;

        int i=0, sizeOfArray;

        clock_t begin, end;

        //Abre os arquivos de entrada e saída.
        inArchive = fopen(inArchName, "rb");
        outArchive = fopen(outArchName, "wb");

        //Obtém a quantidade de bytes do arquivo de entrada e cria um array para armazenar os valores.
        fstat(fileno(inArchive), &finfo);
        int32_t *array = malloc(finfo.st_size);
        sizeOfArray = finfo.st_size / sizeof(int32_t);

        //Verificação caso haja erro na abertura dos arquivos.
        if(!inArchive){
            printf("Este arquivo de entrada não existe!\n");
            fclose(inArchive);
            fclose(outArchive);
            return 0;
        }
        if(!outArchive){
            printf("Erro na abertura do arquivo de saída!\n");
            fclose(inArchive);
            fclose(outArchive);
            return 0;
        }

        //Salva dentro de um array os elementos do arquivo de entrada.
        while(fread(&array[i], sizeof(int32_t), 1, inArchive))
            i++;
        fclose(inArchive);

        srand(time(NULL));

        //ALGORITMOS DE ORDENAÇÃO
        switch(choosenAlg){
            case 1:
                printf("1.Bubble-sort original\n");

                begin = clock();
                originalBubbleSort(array, sizeOfArray);
                end = clock();

                break;
            case 2:
                printf("2.Bubble-sort melhorado\n");

                begin = clock();
                betterBubbleSort(array, sizeOfArray);
                end = clock();

                break;
            case 3:
                printf("3.Insertion-sort\n");

                begin = clock();
                insertionSort(array, sizeOfArray);
                end = clock();

                break;
            case 4:
                printf("4.Mergesort\n");

                begin = clock();
                mergeSort(array, 0, sizeOfArray-1);
                end = clock();

                break;
            case 5:
                printf("5.Quicksort com pivô sendo o último elemento\n");

                begin = clock();
                lastElementQuickSort(array, 0, sizeOfArray-1);
                end = clock();

                break;
            case 6:
                printf("6.Quicksort com pivô sendo um elemento aleatório\n");

                begin = clock();
                randomElementQuickSort(array, 0, sizeOfArray-1);
                end = clock();

                break;
            case 7:
                printf("7.Quicksort com pivô sendo a mediana de três\n");

                begin = clock();
                medianQuickSort(array, 0, sizeOfArray-1);
                end = clock();

                break;
            case 8:
                printf("8.Heapsort\n");

                begin = clock();
                heapSort(array, sizeOfArray);
                end = clock();

                break;
            default:
                printf("Algoritmo inválido!\n");
                return 0;
        }

        //Insere no arquivo de saída o array ordenado.
        fwrite(array, sizeof(int32_t), sizeOfArray, outArchive);
        fclose(outArchive);

        free(array);

        double timeSpent = 0.0;
        timeSpent += (double)(end-begin) / CLOCKS_PER_SEC;
        
        printf("--> Array ordenado!\n");
        printf("--> O tempo de execução do algoritmo [%hu] foi de %.3f segundos.\n", choosenAlg, timeSpent);
    }
    else
        printf("Deve ser executado: ./(nomearquivo).out numeroAlg nomeAlg nomeArqSaida\n");

    return 0;
}
