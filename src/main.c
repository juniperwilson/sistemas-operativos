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
    info->wallets_pids = allocate_dynamic_memory(info->n_wallets * sizeof(int));
    //create structs for servers
    info->servers_pids = allocate_dynamic_memory(info->n_servers * sizeof(int));
 }

 /* Função que reserva a memória partilhada necessária para a execução
 * do SOchain, incluindo buffers e arrays partilhados. É necessário
 * reservar memória partilhada para todos os buffers da estrutura
 * buffers, incluindo tanto os buffers em si como os respetivos
 * pointers, assim como para os arrays *_stats, balances e a variável
 * terminate do info_container. Pode ser usada a função
 * create_shared_memory do memory.h.
 */
void create_shared_memory_structs(struct info_container* info, struct buffers* buffs) {

    buffs->buff_main_wallets = create_shared_memory(ID_SHM_MAIN_WALLETS_BUFFER, info->buffers_size * sizeof(struct transaction));
    buffs->buff_main_wallets->ptrs = create_shared_memory(ID_SHM_MAIN_WALLETS_PTR, info->buffers_size * sizeof(int));

    buffs->buff_servers_main = create_shared_memory(ID_SHM_SERVERS_MAIN_BUFFER, info->buffers_size * sizeof(struct transaction));
    buffs->buff_servers_main->ptrs = create_shared_memory(ID_SHM_SERVERS_MAIN_PTR, info->buffers_size * sizeof(int));

    buffs->buff_wallets_servers = create_shared_memory(ID_SHM_WALLETS_SERVERS_BUFFER, info->buffers_size * sizeof(struct transaction));
    buffs->buff_wallets_servers->ptrs = create_shared_memory(ID_SHM_WALLETS_SERVERS_PTR, sizeof(struct pointers));

    info->wallets_stats = create_shared_memory(ID_SHM_WALLETS_STATS, info->n_wallets * sizeof(int));
    info->servers_stats = create_shared_memory(ID_SHM_SERVERS_STATS, info->n_servers * sizeof(int));

    info->balances = create_shared_memory(ID_SHM_BALANCES, info->n_wallets * sizeof(float));

    info->terminate = create_shared_memory(ID_SHM_TERMINATE, sizeof(int));
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

/* Liberta a memória partilhada previamente reservada. Pode utilizar a
 * função destroy_shared_memory do memory.h
 */
void destroy_shared_memory_structs(struct info_container* info, struct buffers* buffs) {

    destroy_shared_memory(ID_SHM_MAIN_WALLETS_BUFFER, buffs->buff_main_wallets, info->buffers_size * sizeof(struct transaction));
    destroy_shared_memory(ID_SHM_MAIN_WALLETS_PTR, buffs->buff_main_wallets->ptrs, info->buffers_size * sizeof(int));

    destroy_shared_memory(ID_SHM_SERVERS_MAIN_BUFFER, buffs->buff_servers_main, info->buffers_size * sizeof(struct transaction));
    destroy_shared_memory(ID_SHM_SERVERS_MAIN_PTR, buffs->buff_servers_main->ptrs, info->buffers_size * sizeof(int));

    destroy_shared_memory(ID_SHM_WALLETS_SERVERS_BUFFER, buffs->buff_wallets_servers, info->buffers_size * sizeof(struct transaction));
    destroy_shared_memory(ID_SHM_WALLETS_SERVERS_PTR, buffs->buff_wallets_servers->ptrs, sizeof(struct pointers));

    destroy_shared_memory(ID_SHM_WALLETS_STATS, info->wallets_stats, info->n_wallets * sizeof(int));
    destroy_shared_memory(ID_SHM_SERVERS_STATS, info->servers_stats, info->n_servers * sizeof(int));

    destroy_shared_memory(ID_SHM_BALANCES, info->balances, info->n_wallets * sizeof(float));

    destroy_shared_memory(ID_SHM_TERMINATE, info->terminate, sizeof(int));
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

/* Função responsável pela interação com o utilizador. 
 * Permite o utilizador pedir para visualizar saldos, criar
 * transações, consultar recibos, ver estatísticas do sistema e
 * encerrar a execução.
 */
void user_interaction(struct info_container* info, struct buffers* buffs) {

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

/* Cria uma nova transação com os dados inseridos pelo utilizador na linha de
 * comandos (e que ainda estão no stdin a espera de serem lidos com o scanf
 * dentro da função), escreve-a no buffer de memória partilhada entre a main 
 * e as carteiras e atualiza o contador de transações criadas tx_counter. Caso
 * a aplicação já tenha atingido o número máximo de transações permitidas
 * a função retorna apenas uma mensagem de erro e não cria a nova transação.
 */
void create_transaction(int* tx_counter, struct info_container* info, struct buffers* buffs) {

    if (*tx_counter == info->max_txs) {
        perror("error: transaction limit reached;");
    } else {
        int source_id;
        int destination_id;
        float amount;

        scanf("%d%d%f", &source_id, &destination_id, &amount);

        struct transaction tx = {*tx_counter + 1, source_id, destination_id, amount, source_id, 0};
        write_main_wallets_buffer(buffs->buff_main_wallets, info->buffers_size, &tx);

        *tx_counter = *tx_counter + 1;
    }
}

/* Tenta ler o recibo da transação (identificada por id, o qual ainda está no
 * stdin a espera de ser lido dentro da função com o scanf) do buffer de memória
 * partilhada entre os servidores e a main, comprovando a conclusão da transação.
 */
void receive_receipt(struct info_container* info, struct buffers* buffs) {

    int id;
    scanf("%d",&id);
    read_servers_main_buffer(buffs->buff_servers_main, id, info->buffers_size, );
}

/* Imprime as estatísticas atuais do sistema, incluindo as configurações iniciais
 * do sistema, o valor das variáveis terminate e contador da transações criadas,
 * os pids dos processos e as restantes informações (e.g., número de transações 
 * assinadas pela entidade e saldo atual no caso das carteiras).
 */
void print_stat(int tx_counter, struct info_container* info) {

    printf("saldo inicial: %d\n", info->init_balance);
    printf("numero wallets: %d\n", info->n_wallets);
    printf("numero servers: %d\n", info->n_servers);
    printf("buffer size: %d\n", info->buffers_size);
    printf("max transactions: %d\n", info->max_txs);

    printf("terminate: %d\n", *info->terminate);

    write_final_statistics(info);
    print_balance(info);
}

/* Exibe informações sobre os comandos disponíveis na aplicação. 
 */
void help() {
    printf("bal id - Obtém o saldo atual (balance) da carteira (wallet) cujo identificador é id.");
    printf("trx src_id dest_id amount - O utilizador solicita a criação de uma transação em que a carteira de");
    printf("    origem (src_id) envia uma determinada quantidade (amount) de SOT tokens para a carteira de");
    printf("    destino (dest_id). Como resultado, obtém o identificador da transação criada (id).");
    printf("rcp id - Obtém o comprovativo (receipt) da execução de uma transação específica, identificada por id. O");
    printf("    resultado retorna a instância da transação com todas as suas propriedades preenchidas.");
    printf("stat - Apresenta o estado atual das variáveis do info_container (descrito no final desta secção).");
    printf("help - Mostra as informações de ajuda sobre as operações disponíveis.");
    printf("end - Termina a execução do sistema SOchain.");
}

/* Função principal do SOchain. Inicializa o sistema, chama as funções de alocação
 * de memória, a de criação de processos filhos, a de interação do utilizador 
 * e aguarda o encerramento dos processos para chamar as funções para libertar 
 * a memória alocada.
 */
int main(int argc, char *argv[]) {

}