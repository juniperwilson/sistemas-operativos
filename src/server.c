#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define WALLET_DIR "wallets"
#define FIFO_SERVER "/tmp/server_fifo"
#define LEDGER_FILE "ledger.dat"
#define MAX_MSG 256

// função auxiliar: lê saldo da wallet
int ler_saldo(const char *utilizador) {
    char path[256];
    snprintf(path, sizeof(path), "%s/%s.dat", WALLET_DIR, utilizador);
    FILE *fp = fopen(path, "r");
    if (!fp) return -1;
    int saldo;
    fscanf(fp, "%d", &saldo);
    fclose(fp);
    return saldo;
}

// Função auxiliar: atualiza saldo
int atualizar_saldo(const char *utilizador, int novo_saldo) {
    char path[256];
    snprintf(path, sizeof(path), "%s/%s.dat", WALLET_DIR, utilizador);
    FILE *fp = fopen(path, "w");
    if (!fp) return -1;
    fprintf(fp, "%d\n", novo_saldo);
    fclose(fp);
    return 0;
}

// Função auxiliar: escreve no ledger
void registar_ledger(const char *linha) {
    FILE *fp = fopen(LEDGER_FILE, "a");
    if (fp) {
        fprintf(fp, "%s\n", linha);
        fclose(fp);
    }
}

void server() {
    // Criar FIFO do servidor
    unlink(FIFO_SERVER);
    mkfifo(FIFO_SERVER, 0666);

    printf("Servidor ativo. À escuta em %s...\n", FIFO_SERVER);

    while (1) {
        char buffer[MAX_MSG];

        int fd = open(FIFO_SERVER, O_RDONLY);
        if (fd < 0) {
            perror("Erro ao abrir FIFO");
            continue;
        }

        int bytes = read(fd, buffer, sizeof(buffer));
        close(fd);
        if (bytes <= 0) continue;

        buffer[bytes] = '\0';

        // Interpretar pedido
        char tipo[16], de[64], para[64], fifo_cliente[128];
        int quantia;

        if (strncmp(buffer, "TRANSFER", 8) == 0) {
            sscanf(buffer, "TRANSFER %s %s %d %s", de, para, &quantia, fifo_cliente);

            int saldo_de = ler_saldo(de);
            if (saldo_de < quantia || saldo_de < 0) {
                int fd_cli = open(fifo_cliente, O_WRONLY);
                dprintf(fd_cli, "Transferência falhou: saldo insuficiente ou erro.\n");
                close(fd_cli);
                continue;
            }

            int saldo_para = ler_saldo(para);
            if (saldo_para < 0) saldo_para = 0;

            atualizar_saldo(de, saldo_de - quantia);
            atualizar_saldo(para, saldo_para + quantia);

            char linha[256];
            snprintf(linha, sizeof(linha), "TRANSFER %s -> %s : %d", de, para, quantia);
            registar_ledger(linha);

            int fd_cli = open(fifo_cliente, O_WRONLY);
            dprintf(fd_cli, "Transferência concluída com sucesso.\n");
            close(fd_cli);
        }
        else if (strncmp(buffer, "MINT", 4) == 0) {
            sscanf(buffer, "MINT %s %s", para, fifo_cliente);

            int saldo = ler_saldo(para);
            if (saldo < 0) saldo = 0;
            saldo += 10; // MINT sempre 10 moedas
            atualizar_saldo(para, saldo);

            char linha[256];
            snprintf(linha, sizeof(linha), "MINT -> %s : 10", para);
            registar_ledger(linha);

            int fd_cli = open(fifo_cliente, O_WRONLY);
            dprintf(fd_cli, "Recebeste 10 coins!\n");
            close(fd_cli);
        }
        else {
            printf("Pedido desconhecido: %s\n", buffer);
        }
    }
}
