Faça um único programa em C para Linux que implemente os seguintes algoritmos de ordenação estudados:<br>

1.Bubble-sort original;<br>
2.Bubble-sort melhorado;<br>
3.Insertion-sort;<br>
4.Mergesort;<br>
5.Quicksort com pivô sendo o último elemento;<br>
6.Quicksort com pivô sendo um elemento aleatório;<br>
7.Quicksort com pivô sendo a mediana de três;<br>
8.Heapsort.<br>

 A entrada do programa consiste de um arquivo binário contendo apenas uma sequência de números inteiros do tipo int32_t. 
A sequência ordenada deve ser armazenada em outro arquivo binário. Os arquivos de entrada/saída não terão nenhuma outra 
informação além da sequência de entrada/saída, sendo que a quantidade de elementos nesses arquivos deve ser calculada 
considerando o tamanho do arquivo em bytes (veja página de manual da função fstat() ou stat()) e a quantidade de bytes 
que cada elemento ocupa.  O programa deve receber três argumentos da linha de comando, nessa ordem: 1) o número do algoritmo 
escolhido (1 a 8); 2) o nome do arquivo de entrada; 3) o nome do arquivo de saída. Não deve haver interação com o usuário e, 
ao final, deve ser apresentado o tempo de execução gasto apenas pelo algoritmo de ordenação (desconsiderando tempo de E/S como,
por exemplo, leitura/gravação em arquivos, impressão em tela, leitura de teclado, etc.) em segundos com precisão de 3 casas
decimais (milissegundos). Para cálculo do tempo, consulte funções como gettimeofday(), clock_gettime(), dentre outras.<br>

Teste cada uma das versões dos algoritmos implementados com três tipos de entrada: em ordem crescente, em ordem decrescente e em ordem aleatória. Devem ser consideradas entradas dos seguintes tamanhos nos testes:

10, 100, 1.000, 10.000 e 100.000 (todas as versões de todos os algoritmos);<br>
500.000, 5.000.000 e 50.000.000: Mergesort, Quicksort com pivô sendo um elemento aleatório, Quicksort com pivô sendo a mediana de três, Heapsort.<br>

Faça um relatório de aproximadamente 3 páginas contendo tabelas, gráficos e discussões sobre os resultados obtidos nos testes.
O relatório deve conter os dados do aluno e da disciplina, uma breve introdução, uma seção com os resultados obtidos e uma seção com a discussão sobre os 
resultados e conclusão do relatório. Os tempos de execução para elaboração do relatório devem ser coletados no Laboratório de Computação 4.

