Faça um programa em C para Linux que suporte as seguintes operações sobre árvores AVL:

-Busca, inserção e remoção de chaves inteiras: quando necessário, devem ser realizadas as rotações adequadas.<br>
Serão aceitos apenas os algoritmos apresentados/discutidos em sala de aula;<br>
-Visualização da árvore em modo gráfico usando a biblioteca gfx. Deve ser impressa a representação hierárquica <br>
contendo  a chave e o balanço de cada nó;<br>
-Leitura e gravação da árvore em arquivo formato binário usando percurso em pré-ordem. A árvore não deve ser <br>
reconstruída através de operações de inserção;<br>
-Desalocação da árvore.<br>
-Deve ser apresentada uma interface (modo texto ou gráfico) de simples uso que suporte todas as operações solicitadas.

A seguinte estrutura de dados deve ser utilizada para representar na memória cada nó da árvore:

struct s_no{<br>
    int32_t chave:28;<br>
    int32_t bal:2;<br>
    int32_t reservado:2; /* sem uso */<br>
    struct s_no* esq;<br>
    struct s_no* dir;<br>
};

A seguinte estrutura de dados deve ser utilizada para representar em arquivo cada nó da árvore:

struct s_arq_no{<br>
    int32_t chave:28;<br>
    int32_t bal:2;<br>
    uint32_t esq:1;<br>
    uint32_t dir:1;<br>
};
