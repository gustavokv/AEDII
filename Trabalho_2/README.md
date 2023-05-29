Considere uma unidade de memória com capacidade para armazenar M bytes, onde os endereços dos bytes variam de 0 a M-1. 
O gerenciamento da alocação/desalocação de blocos de memória pode ser feito da seguinte forma:

-Um bloco só pode ser alocado em posições contíguas da memória;

-Há uma lista ordenada que representa os blocos alocados na memória. Cada elemento nessa lista possui 
uma id do bloco (número natural único que identifica o bloco), seu endereço inicial e final na memória. 
Essa lista, ordenada pelo campo id, é inicialmente vazia;

-Há uma lista ordenada que representa os blocos disponíveis na memória. Cada elemento nessa lista possui 
um endereço inicial e um tamanho b. Inicialmente a memória toda é considerada um bloco livre com endereço 
inicial 0 e tamanho M. Quando a memória está totalmente ocupada, essa lista é vazia. A ordenação dessa lista 
lista é pelo campo b;

-Quando é solicitada a alocação de um novo bloco de tamanho t bytes é necessário encontrar o menor bloco disponível 
na memória com tamanho b ≥ t. Se não houver, a alocação não pode ser realizada. Se houver, o novo bloco será alocado
no início do bloco disponível e a lista de blocos alocados terá um novo elemento. Ainda, se b – t > 0, o bloco de memória 
disponível que existia agora terá endereço inicial no próximo byte após o fim do bloco recém alocado. Se b = t, o bloco de 
memória disponível foi totalmente ocupado e, portanto, não deve mais existir. O bloco alocado receberá uma id distinta de 
todos os demais blocos;

-Quando é solicitada a desalocação de um bloco com determinada id, o mesmo é removido da lista de blocos alocados e a memória
possuirá um novo bloco disponível. Quando um novo bloco disponível é inserido na lista, deve-se verificar se há blocos 
adjacentes que também estão livres. Caso haja, devem ser juntados para representar um único bloco maior.

Implemente um programa que gerencie a alocação/desalocação de blocos de memória utilizando uma lista duplamente encadeada 
(sem nó-cabeça) de blocos alocados e uma lista simplesmente encadeada (sem nó-cabeça) de blocos disponíveis.

Seu programa deverá apresentar interface com usuário para:

-Definição do valor de M;
-Entrada de dados para alocação e desalocação de blocos;
-Mensagens de saída/erro diversas, conforme necessidade;
-Em modo gráfico, usando a biblioteca gfx, em uma janela de tamanho 1024x768 pixels:
     Visualizar mapa da memória com informações sobre os blocos disponíveis (endereço inicial e tamanho) e 
     alocados (id, endereço inicial e endereço final);
-Apresentar a porcentagem de memória alocada.

O código deve estar devidamente organizado, identado, comentado e sem erros ou avisos usando as opções -Wall -Wextra -pedantic do gcc.

No encerramento do programa, toda a memória alocada dinamicamente deve ser desalocada.
