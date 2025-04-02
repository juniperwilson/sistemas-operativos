#include "main.h"
#include <stdlib.h>

/* Função que lê do stdin com o scanf apropriado para cada tipo de dados
 * e valida os argumentos da aplicação, incluindo o saldo inicial, 
 * o número de carteiras, o número de servidores, o tamanho dos buffers 
 * e o número máximo de transações. Guarda essa informação na estrutura info_container.
 */
 void main_args(int argc, char *argv[], struct info_container *info) {
    
    if (argc < 6) {
        perror("incorrect args");
        exit(0);
    }

    float start_balance = *argv[1];
    int wallet_count = *argv[2];
    int server_count = *argv[3];
    int size_buffer = *argv[4];
    int transaction_limit = *argv[5];

    if (start_balance <= 0) {
        perror("error: incorrect start balance");
        exit(0);
    }

    if (wallet_count <= 0) {
        perror("error: incorrect wallet count");
        exit(0);
    }

    if (server_count <= 0) {
        perror("error: incorrect server count");
        exit(0);
    }

    if (size_buffer <= 0) {
        perror("error: incorrect size of buffer");
        exit(0);
    }

    if (transaction_limit <= 0) {
        perror("error: incorrect transaction limit");
        exit(0);
    }

    info->init_balance = start_balance;
    info->n_wallets = wallet_count;
    info->n_servers = server_count;
    info->buffers_size = size_buffer;
    info->max_txs = transaction_limit;
 }

/* Função que reserva a memória dinâmica necessária, por exemplo, 
 * para os arrays *_pids de info_container. Para tal, pode ser usada
 * a função allocate_dynamic_memory do memory.h.
 */
 void create_dynamic_memory_structs(struct info_container* info, struct buffers* buffs) {

    //create structs for wallets
    allocate_dynamic_memory(info->n_wallets * sizeof(int));
    allocate_dynamic_memory(info->n_wallets * sizeof(int));
    allocate_dynamic_memory(info->n_wallets * sizeof(float));
    //create structs for servers
    allocate_dynamic_memory(info->n_servers * sizeof(int));
    allocate_dynamic_memory(info->n_servers * sizeof(int));
 }

 /* Liberta a memória dinâmica previamente reservada. Pode utilizar a
 * função deallocate_dynamic_memory do memory.h
 */
void destroy_dynamic_memory_structs(struct info_container* info, struct buffers* buffs) {

    deallocate_dynamic_memory(info->balances);
    deallocate_dynamic_memory(info->wallets_pids);
    deallocate_dynamic_memory(info->wallets_stats);

    deallocate_dynamic_memory(info->servers_pids);
    deallocate_dynamic_memory(info->servers_stats);
}

 /* Função que cria os processos das carteiras e servidores. 
 * Os PIDs resultantes são armazenados nos arrays apropriados 
 * da estrutura info_container.
 */
void create_processes(struct info_container* info, struct buffers* buffs) {

    int wallet_id;
    int server_id;

    int i;
    for (i = 0; i < info->n_wallets; i++) {
        wallet_id = launch_wallet(i, info, buffs);
        *(info->wallets_pids + i) = wallet_id;
    }

    int j;
    for (j = 0; j < info->n_servers; j++) {
        server_id = launch_server(j, info, buffs);
        *(info->servers_pids + j) = server_id;
    }
}

/* Função que imprime as estatísticas finais do SOchain, incluindo 
 * o número de transações assinadas por cada carteira e processadas
 * por cada servidor.
 */
 void write_final_statistics(struct info_container* info) {
    int i;
    for (i = 0; i < info->n_wallets; i++) {
        printf("A Wallet %d assinou %d transacoes\n", *(info->wallets_pids = i), *(info->wallets_stats = i));
    }

    int j;
    for (j = 0; j < info->n_servers; j++) {
        printf("O Server %d processou %d transacoes\n", *(info->servers_pids + j), *(info->servers_stats + j));
    }
 }

/* Termina a execução do programa. Deve atualizar a flag terminate e,
 * em seguida, aguardar a terminação dos processos filhos, escrever as 
 * estatísticas finais e retornar.
 */
 void end_execution(struct info_container* info, struct buffers* buffs) {

    *info->terminate = 1;
    wait_processes(info);
    write_final_statistics(info);
    exit(0);
 }

/* Aguarda a terminação dos processos filhos previamente criados. Pode usar
 * a função wait_process do process.h
 */
 void wait_processes(struct info_container* info) {

    int i;
    for (i = 0; i < info->n_wallets; i++) {
        wait_process(*(info->wallets_pids+i));
    }

    int j;
    for (j = 0; j < info->n_servers; j++) {
        wait_process(*(info->servers_pids + j));
    }
 }

 /* Imprime o saldo atual de uma carteira identificada pelo id que ainda está
 * no stdin a espera de ser lido com o scanf dentro da função
 */
void print_balance(struct info_container* info) {
    int current_id;
    scanf("%d", &current_id);
    
    int i;
    for (i = 0; i < info->n_wallets; i++) {
        if (*(info->wallets_pids + i) == current_id) {
            printf("O saldo da carteira %d eh %d\n", *(info->wallets_pids + i), *(info->balances + i));
        }
    }
}