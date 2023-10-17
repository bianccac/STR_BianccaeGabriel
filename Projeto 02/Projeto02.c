#include <stdio.h>

int main(int argc, char *argv[]) {
    // Verifica se o nome do arquivo foi passado como argumento
    if (argc != 2) {
        printf("Uso: %s <nome_do_arquivo>\n", argv[0]);
        return 1;
    }

    // Abre o arquivo em modo de leitura
    FILE *arquivo = fopen(argv[1], "r");

    // Verifica se o arquivo foi aberto corretamente
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    // Lê e imprime o conteúdo do arquivo linha por linha
    char linha[100]; // Defina um tamanho apropriado para a linha
    while (fgets(linha, sizeof(linha), arquivo)) {
        printf("%s", linha);
    }

    // Fecha o arquivo
    fclose(arquivo);

    return 0;
}

