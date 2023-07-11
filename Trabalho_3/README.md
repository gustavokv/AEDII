Faça um programa em C para Linux que suporte as seguintes operações sobre uma árvore binária de busca que armazena chaves inteiras:

-Busca pela maior e menor chave na árvore;
-Busca por uma chave com valor x;
-Busca pelo sucessor e predecessor de uma chave com valor x;
-Inserção e remoção de uma chave com valor x;
-Visualização da representação hierárquica da árvore em modo gráfico usando a biblioteca gfx;
-Leitura e gravação da árvore em arquivo formato binário usando percurso em pré-ordem. A árvore não deve ser reconstruída através de operações de inserção;
-Desalocação da árvore.
-A seguinte struct deve ser utilizada para representar em arquivo cada nó da árvore:

struct s_arq_no{
    int32_t chave:30;
    uint32_t esq:1;
    uint32_t dir:1;
};

Serão aceitos apenas os algoritmos apresentados/discutidos em sala de aula.

Deve ser apresentada uma interface modo texto de simples uso que suporte todas as operações. A janela gráfica deve ser mantida sempre aberta com a visualização da árvore atual, sem necessidade de comando para atualização.
