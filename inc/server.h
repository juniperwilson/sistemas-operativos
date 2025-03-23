/* ATENÇÃO: NÃO MODIFICAR ESTE FICHEIRO
 * Os ficheiros .h serão substituídos pelos fornecidos pelos professores durante a avaliação
 * Caso estes ficheiros sejam modificados, o trabalho deixará de compilar durante a avaliação
*/
#ifndef SERVER_H_GUARD
#define SERVER_H_GUARD

#include "memory.h"
#include "main.h"

/* Função principal de um servidor. Deve executar um ciclo infinito onde, em 
 * cada iteração, lê uma transação do buffer de memória partilhada entre as carteiras e os servidores.
 * Se a transação tiver um id válido e info->terminate ainda for 0, o servidor valida, processa e assina 
 * a transação e a encaminha para o buffer partilhado com a main. Transações com id igual a -1 são ignoradas,
 * e aguarda-se alguns milisegundos antes de tentar ler novamente do buffer. Se info->terminate for 1, 
 * significa que o sistema deve encerrar, retornando o número de transações processadas. 
 * Pode usar os outros métodos auxiliares definidos.
 */
int execute_server(int server_id, struct info_container* info, struct buffers* buffs);

/* Função que lê uma transação do buffer de memória partilhada entre as carteiras e os servidores. Caso não
 * exista transações a serem lidas do buffer, retorna uma transação com o tx.id -1. Antes de tentar ler a 
 * transação do buffer, deve verificar se info->terminate tem valor 1. Em caso afirmativo, retorna imediatamente.
 */
void server_receive_transaction(struct transaction* tx, struct info_container* info, struct buffers* buffs);

/* Função que processa uma transação tx, verificando a validade dos identificadores das carteiras de origem e destino,
 * dos fundos da carteira de origem e a assinatura da carteira de origem. Atualiza os saldos das carteiras envolvidas, 
 * adiciona a assinatura do servidor e incrementa o contador de transações processadas pelo servidor.
 */
void server_process_transaction(struct transaction* tx, int server_id, struct info_container* info);

/* Função que escreve uma transação correta processada no buffer de memória partilhada entre os servidores e a main.
 * Caso o servidor não tenha assinado a transação, essa não será escrita pois significa que a transação era inválida.
 * Se não houver espaço no buffer, a transação não é enviada e o recibo perde-se.
 */
void server_send_transaction(struct transaction* tx, struct info_container* info, struct buffers* buffs);

#endif