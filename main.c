#include "utils.h"

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Uso: %s <notas.csv>\n", argv[0]);
        return 1;
    }

    const char *nome_arquivo_alunos = argv[1];

    
    int num_alunos = 0;
    Aluno *alunos = carregar_alunos(nome_arquivo_alunos, &num_alunos);
    if (!alunos) {
        return 1;
    }

    ordenar_alunos(alunos, num_alunos);
    listar_alunos(alunos, num_alunos);

    
    liberar_memoria(alunos, num_alunos);
    return 0;
}
