/* ATENÇÃO: NÃO MODIFICAR ESTE FICHEIRO
 * Os ficheiros .h serão substituídos pelos fornecidos pelos professores durante a avaliação
 * Caso estes ficheiros sejam modificados, o trabalho deixará de compilar durante a avaliação
*/
#ifndef MEMORY_H_GUARD
#define MEMORY_H_GUARD

//Definições dos identificadores para serem utilizados para as zonas de memória partilhada
#define ID_SHM_MAIN_WALLETS_PTR 			"SHM_MAIN_WALLETS_PTR"
#define ID_SHM_MAIN_WALLETS_BUFFER 			"SHM_MAIN_WALLETS_BUFFER"
#define ID_SHM_WALLETS_SERVERS_PTR 			"SHM_WALLETS_SERVERS_PTR"
#define ID_SHM_WALLETS_SERVERS_BUFFER 		"SHM_WALLETS_SERVERS_BUFFER"
#define ID_SHM_SERVERS_MAIN_PTR 			"SHM_SERVERS_MAIN_PTR"
#define ID_SHM_SERVERS_MAIN_BUFFER 		    "SHM_SERVERS_MAIN_BUFFER"
#define ID_SHM_WALLETS_STATS				"SHM_WALLETS_STATS"
#define ID_SHM_SERVERS_STATS				"SHM_SERVERS_STATS"
#define ID_SHM_BALANCES						"SHM_BALANCES"
#define ID_SHM_TERMINATE					"SHM_TERMINATE"

// Estrutura de apontadores auxiliares para os buffer circulares
struct pointers { 																					
	int in;  																						
	int out; 																						
};	

// Estrutura que representa um buffer circular
struct circ_buffer { 	
	struct pointers *ptrs;
	struct transaction* buffer;
};

// Estrutura que representa um buffer de acesso aleatório
// Notem que agora só há um apontador
struct ra_buffer {
	int* ptrs;
	struct transaction* buffer;
};

// Estrutura que representa uma transação
struct transaction {
	int id; 					//ID da transação
	int src_id;		            //ID da carteira de origem da transação
	int dest_id;		        //ID da carteira de destino da transação
	float amount;               //quantidade de SOT a ser transferido na transação	
	int wallet_signature;		//ID da carteira que assinou a transação (tem que se igual ao src_id)
	int server_signature;	    //ID do servidor que validou e processou a transação (só assina se a transação é válida)
};

// Estrutura que agrega os shared memory buffers necessários para comunicação entre processos
struct buffers {
	struct ra_buffer* buff_main_wallets; 		//buffer para a main enviar as transações criadas às carteiras
	struct circ_buffer* buff_wallets_servers;	//buffer para as carteiras encaminharem as transações aos servidores
	struct ra_buffer* buff_servers_main; 	    //buffer para os servidores encaminharem os comprovativos à main
};

/* Função que reserva uma zona de memória dinâmica com o tamanho indicado
 * por size, preenche essa zona de memória com o valor 0, e retorna um 
 * apontador para a mesma.
 */
void* allocate_dynamic_memory(int size);


/* Função que reserva uma zona de memória partilhada com tamanho indicado
 * por size e nome name, preenche essa zona de memória com o valor 0, e 
 * retorna um apontador para a mesma. Pode concatenar o id do utilizador 
 * resultante da função getuid() a name, para tornar o nome único para 
 * a zona de memória.
 */
void* create_shared_memory(char* name, int size);

/* Liberta uma zona de memória dinâmica previamente alocada.
 */
void deallocate_dynamic_memory(void* ptr);

/* Remove uma zona de memória partilhada previamente criada. 
 */
void destroy_shared_memory(char* name, void* ptr, int size);

/* Escreve uma transação no buffer de memória partilhada entre a Main e as carteiras.
 * A transação deve ser escrita numa posição livre do buffer, 
 * tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
 * Se não houver nenhuma posição livre, não escreve nada e a transação é perdida.
 */
void write_main_wallets_buffer(struct ra_buffer* buffer, int buffer_size, struct transaction* tx);

/* Função que escreve uma transação no buffer de memória partilhada entre as carteiras e os servidores.
 * A transação deve ser escrita numa posição livre do buffer, tendo em conta o tipo de buffer 
 * e as regras de escrita em buffers desse tipo. Se não houver nenhuma posição livre, não escreve nada.
 */
void write_wallets_servers_buffer(struct circ_buffer* buffer, int buffer_size, struct transaction* tx);

/* Função que escreve uma transação no buffer de memória partilhada entre os servidores e a Main, a qual 
 * servirá de comprovativo da execução da transação. A transação deve ser escrita numa posição livre do 
 * buffer, tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo. 
 * Se não houver nenhuma posição livre, não escreve nada.
 */
void write_servers_main_buffer(struct ra_buffer* buffer, int buffer_size, struct transaction* tx);

/* Função que lê uma transação do buffer entre a Main e as carteiras, se houver alguma disponível para ler 
 * e que seja direcionada a própria carteira que está a tentar ler. A leitura deve ser feita tendo em conta 
 * o tipo de buffer e as regras de leitura em buffers desse tipo. Se não houver nenhuma transação disponível, 
 * afeta tx->id com o valor -1.
 */
void read_main_wallets_buffer(struct ra_buffer* buffer, int wallet_id, int buffer_size, struct transaction* tx);

/* Função que lê uma transação do buffer entre as carteiras e os servidores, se houver alguma disponível para ler.
 * A leitura deve ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo. Qualquer
 * servidor pode ler qualquer transação deste buffer. Se não houver nenhuma transação disponível, 
 * afeta tx->id com o valor -1.
 */
void read_wallets_servers_buffer(struct circ_buffer* buffer, int buffer_size, struct transaction* tx);

/* Função que lê uma transação do buffer entre os servidores e a Main, se houver alguma disponível para ler 
 * e que tenha o tx->id igual a tx_id. A leitura deve ser feita tendo em conta o tipo de buffer e as regras 
 * de leitura em buffers desse tipo. Se não houver nenhuma transação disponível, afeta tx->id com o valor -1.
 */
void read_servers_main_buffer(struct ra_buffer* buffer, int tx_id, int buffer_size, struct transaction* tx);

#endif