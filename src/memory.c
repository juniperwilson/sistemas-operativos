void* allocate_dynamic_memory(int size) {

    if (size == 0) {
        perror("Error: attempt to allocate size 0 memory.");
        exit(1);
    }

    int * ptr = malloc(size);
    if (ptr == NULL) {
        perror("Error: memory not allocated.");
        exit(1);
    }
    memset(ptr, 0, size);
    return ptr;
}

void* create_shared_memory(char* name, int size) {

    int fd = shm_open(name,);
    if (fd == -1) {
        perror("Error: shm fail")
        exit(1);
    }

    int result = ftruncate(fd, size);
    if (result == -1) {
        perror("Error: shm not truncated.");
        exit(1);
    }

    int * ptr = mmap(0, size, PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("shm_mmap");
        exit(1);
    }

    int i;
    for (i = 0; i < size; i++) {
        ptr[i] = 0;
    }

    return ptr;
}

void deallocate_dynamic_memory(void* ptr) {
    free(ptr);
}

void destroy_shared_memory(char* name, void* ptr, int size) {
    int ret = munmap(ptr, size);
    if (ret == -1) {
        perror("Error: munmap");
        exit(1);
    }

    ret = shm_unlink(name);
    if (ret == -1) {
        perror("Error: shm_unlink");
        exit(1);
    }
}

/* Escreve uma transação no buffer de memória partilhada entre a Main e as carteiras.
 * A transação deve ser escrita numa posição livre do buffer, 
 * tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
 * Se não houver nenhuma posição livre, não escreve nada e a transação é perdida.
 */
void write_main_wallets_buffer(struct ra_buffer* buffer, int buffer_size, struct transaction* tx) {
    
    int i;
    for (i = 0; i < buffer_size; i++) {
        if (buffer.ptrs[i] == 0) {
            buffer.transaction[i] = tx;
            buffer.ptrs[i] == 1;
            break;
        }
    }
}

/* Função que escreve uma transação no buffer de memória partilhada entre as carteiras e os servidores.
 * A transação deve ser escrita numa posição livre do buffer, tendo em conta o tipo de buffer 
 * e as regras de escrita em buffers desse tipo. Se não houver nenhuma posição livre, não escreve nada.
 */
void write_wallets_servers_buffer(struct circ_buffer* buffer, int buffer_size, struct transaction* tx) {

    int i;
    for (int )
}

/* Função que escreve uma transação no buffer de memória partilhada entre os servidores e a Main, a qual 
 * servirá de comprovativo da execução da transação. A transação deve ser escrita numa posição livre do 
 * buffer, tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo. 
 * Se não houver nenhuma posição livre, não escreve nada.
 */
void write_servers_main_buffer(struct ra_buffer* buffer, int buffer_size, struct transaction* tx) {
    write_main_wallets_buffer(buffer, buffer_size, tx);
}

/* Função que lê uma transação do buffer entre a Main e as carteiras, se houver alguma disponível para ler 
 * e que seja direcionada a própria carteira que está a tentar ler. A leitura deve ser feita tendo em conta 
 * o tipo de buffer e as regras de leitura em buffers desse tipo. Se não houver nenhuma transação disponível, 
 * afeta tx->id com o valor -1.
 */
void read_main_wallets_buffer(struct ra_buffer* buffer, int wallet_id, int buffer_size, struct transaction* tx) {
    
}




