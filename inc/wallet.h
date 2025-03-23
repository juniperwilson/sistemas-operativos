/* ATENÇÃO: NÃO MODIFICAR ESTE FICHEIRO
 * Os ficheiros .h serão substituídos pelos fornecidos pelos professores durante a avaliação
 * Caso estes ficheiros sejam modificados, o trabalho deixará de compilar durante a avaliação
*/
#ifndef WALLET_H_GUARD
#define WALLET_H_GUARD

#include "memory.h"
#include "main.h"

/* Função principal de uma carteira. Deve executar um ciclo infinito onde,
 * em cada iteração, lê uma transação da main apenas caso o src_id da transação seja
 * o seu próprio id. Se info->terminate ainda for 0, a carteira assina autorizando a transação, 
 * encaminhando-a para os servidores. Se a transação tiver um id igual a -1, é ignorada e espera-se 
 * alguns milisegundos antes de tentar ler uma nova transação do buffer. Se info->terminate for 1,
 * a execução termina e retorna o número de transações assinadas.
 */
int execute_wallet(int wallet_id, struct info_container* info, struct buffers* buffs);

/* Função que lê uma transação do buffer de memória partilhada entre a main e as carteiras apenas 
 * caso o src_id da transação seja o seu próprio id. Antes de tentar ler a transação, deve verificar 
 * se info->terminate tem valor 1. Em caso afirmativo, retorna imediatamente da função.
 */
void wallet_receive_transaction(struct transaction* tx, int wallet_id, struct info_container* info, struct buffers* buffs);

/* Função que assina uma transação comprovando que a carteira de origem src_id da transação corresponde
 * ao wallet_id. Atualiza o campo wallet_signature da transação e incrementa o contador de transações 
 * assinadas pela carteira.
 */
void wallet_process_transaction(struct transaction* tx, int wallet_id, struct info_container* info);

/* Função que escreve uma transação assinada no buffer de memória partilhada entre
 * as carteiras e os servidores. Se não houver espaço disponível no buffer, a transação
 * perde-se.
 */
void wallet_send_transaction(struct transaction* tx, struct info_container* info, struct buffers* buffs);

#endif