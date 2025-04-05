#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define WALLET_DIR "wallets"
#define FIFO_CLIENT "/tmp/client_fifo"
#define FIFO_SERVER "/tmp/server_fifo"
#define MAX_MSG 256

void wallet(const char *username) {
    char wallet_path[256];
    snprintf(wallet_path, sizeof(wallet_path), "%s/%s.dat", WALLET_DIR, username);

    // cria diretório de wallets 
    mkdir(WALLET_DIR, 0777);

    // cria ficheiro da wallet 
    FILE *fp = fopen(wallet_path, "r+");
    if (!fp) {
        fp = fopen(wallet_path, "w+");
        if (!fp) {
            perror("Erro ao criar wallet");
            exit(1);
        }
        fprintf(fp, "100\n");  // saldo inicial
        fflush(fp);
    }

    printf("Bem-vindo, %s!\n", username);

    // interface do user
    while (1) {
        printf("\n1. Ver saldo\n2. Enviar moedas\n3. Pedir moedas\n4. Sair\nEscolha: ");
        int opcao;
        scanf("%d", &opcao);

        if (opcao == 1) {
            rewind(fp);
            int saldo;
            fscanf(fp, "%d", &saldo);
            printf("Saldo atual: %d coins\n", saldo);
        }
        else if (opcao == 2) {
            char destino[64];
            int quantia;
            printf("Destinatário: ");
            scanf("%s", destino);
            printf("Quantia: ");
            scanf("%d", &quantia);

            // cria FIFO do cliente
            mkfifo(FIFO_CLIENT, 0666);

            // enviar pedido ao server
            int server_fd = open(FIFO_SERVER, O_WRONLY);
            dprintf(server_fd, "TRANSFER %s %s %d %s\n", username, destino, quantia, FIFO_CLIENT);
            close(server_fd);

            // esperar resposta do server
            int client_fd = open(FIFO_CLIENT, O_RDONLY);
            char resposta[MAX_MSG];
            read(client_fd, resposta, MAX_MSG);
            close(client_fd);
            unlink(FIFO_CLIENT);

            printf("Resposta do servidor: %s\n", resposta);
        }
        else if (opcao == 3) {
            // Similar ao anterior, mas pedido é de tipo "MINT"
            mkfifo(FIFO_CLIENT, 0666);
            int server_fd = open(FIFO_SERVER, O_WRONLY);
            dprintf(server_fd, "MINT %s %s\n", username, FIFO_CLIENT);
            close(server_fd);

            int client_fd = open(FIFO_CLIENT, O_RDONLY);
            char resposta[MAX_MSG];
            read(client_fd, resposta, MAX_MSG);
            close(client_fd);
            unlink(FIFO_CLIENT);

            printf("Resposta do servidor: %s\n", resposta);
        }
        else if (opcao == 4) {
            break;
        }
        else {
            printf("Opção inválida.\n");
        }
    }

    fclose(fp);
}
