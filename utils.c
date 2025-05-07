#include "utils.h"

void calcular_notas(Aluno *aluno) {
    if (aluno == NULL || aluno->num_avaliacoes < 2) {
        perror("Não há avaliações suficientes para cálculo da nota final.\n");
        return;
    }
    float avs[2];
    for (int i = 0; i < 2; i++) {
        Avaliacao *av = &aluno->avaliacoes[i];
        if (av->ap1 < 0) av->ap1 = 0;
        else if (av->ap1 > 10) av->ap1 = 10;
        if (av->ap2 < 0) av->ap2 = 0;
        else if (av->ap2 > 10) av->ap2 = 10;
        if (av->ap3 < 0) av->ap3 = 0;
        else if (av->ap3 > 10) av->ap3 = 10;
        if (av->np < 0) av->np = 0;
        else if (av->np > 10) av->np = 10;
        float media_ap = (av->ap1 + av->ap2 + av->ap3) / 3.0f;
        float av_calc = media_ap * 0.3f + av->np * 0.7f;
        if (av_calc < 0) av_calc = 0;
        else if (av_calc > 10) av_calc = 10;
        avs[i] = av_calc;
    }
    float nf = (avs[0] + avs[1]) / 2.0f;
    if (nf < 0) nf = 0;
    else if (nf > 10) nf = 10;
    aluno->nf = nf;
    aluno->status = (nf >= 6.0f) ? 1 : 0;
}

Aluno* realocar_memoria_aluno(Aluno *alunos, int novo_tamanho) {
    Aluno *temp = realloc(alunos, novo_tamanho * sizeof(Aluno));
    if (temp == NULL) {
        perror("Erro ao realocar memória para o novo tamanho de alunos!\n");
        free(alunos);
        return NULL;
    }
    return temp;
}

Avaliacao* realocar_memoria_avaliacao(Avaliacao *avaliacoes, int novo_tamanho) {
    Avaliacao *temp = realloc(avaliacoes, novo_tamanho * sizeof(Avaliacao));
    if (temp == NULL) {
        perror("Erro ao realocar memória para o novo tamanho das avalicações!\n");
        free(avaliacoes);
        return NULL;
    }
    return temp;
}

void liberar_memoria(Aluno *alunos, int num_alunos) {
    if (alunos == NULL) return;
    for (int i = 0; i < num_alunos; i++) {
        free(alunos[i].avaliacoes);
    }
    free(alunos);
}

void adicionar_avaliacoes(Aluno *aluno, Avaliacao avaliacao) {
    Avaliacao *temp = realloc(
        aluno->avaliacoes,
        (aluno->num_avaliacoes + 1) * sizeof(Avaliacao)
    );
    if (temp == NULL) {
        perror("Erro ao alocar memória para avaliações!\n");
        return;
    }
    aluno->avaliacoes = temp;
    aluno->avaliacoes[aluno->num_avaliacoes] = avaliacao;
    aluno->num_avaliacoes++;
}

Aluno *carregar_alunos(const char *nome_arquivo, int *num_alunos) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) {
        perror("Erro ao abrir arquivo de alunos");
        return NULL;
    }
    Aluno *alunos = NULL;
    char linha[256];
    fgets(linha, sizeof(linha), arquivo);
    while (fgets(linha, sizeof(linha), arquivo)) {
        Aluno novo_aluno;
        Avaliacao avaliacao1;
        Avaliacao avaliacao2;
        float av1ap1, av1ap2, av1ap3, np1, av2ap1, av2ap2, av2ap3, np2;
        if (sscanf(linha, "%d,%[^,],%f,%f,%f,%f,%f,%f,%f,%f", &novo_aluno.matricula, novo_aluno.nome, &avaliacao1.ap1, &avaliacao1.ap2, &avaliacao1.ap3, &avaliacao1.np, &avaliacao2.ap1, &avaliacao2.ap2, &avaliacao2.ap3, &avaliacao2.np) == 10) {
            alunos = realocar_memoria_aluno(alunos, (*num_alunos + 1));
            if (!alunos) {
                perror("Erro ao alocar memória para alunos");
                fclose(arquivo);
                return NULL;
            }
            novo_aluno.avaliacoes = NULL;
            novo_aluno.num_avaliacoes = 0;
            adicionar_avaliacoes(&novo_aluno, avaliacao1);
            adicionar_avaliacoes(&novo_aluno, avaliacao2);
            calcular_notas(&novo_aluno);
            alunos[*num_alunos] = novo_aluno;
            (*num_alunos)++;
        } else {
            fprintf(stderr, "Erro ao ler linha do arquivo de alunos: %s", linha);
        }
    }
    fclose(arquivo);
    return alunos;
}

void ordenar_alunos(Aluno *alunos, int num_alunos) {
    for (int i = 0; i < num_alunos - 1; i++) {
        for (int j = 0; j < num_alunos - i - 1; j++) {
            if (alunos[j].nf < alunos[j + 1].nf) {
                Aluno temp = alunos[j];
                alunos[j] = alunos[j + 1];
                alunos[j + 1] = temp;
            }
        }
    }
}

void listar_alunos(const Aluno *alunos, int num_alunos) {
    if (num_alunos == 0) {
        printf("Nenhum aluno cadastrado.\n");
        return;
    }
    printf("\n");
    printf("%-10s | %-30s | %-8s | %-8s | %-8s | %-8s | %-8s | %-8s | %-8s | %-8s | %-8s | %-10s\n",
           " Matrícula", "Nome", "AV1:AP1", "AV1:AP2", "AV1:AP3", "NP1", "AV2:AP1", "AV2:AP2", "AV2:AP3", "NP2", "NF", "Status");
    printf("-----------|--------------------------------|----------|----------|----------|----------|----------|----------|----------|----------|----------|------------\n");
    for (int i = 0; i < num_alunos; i++) {
        if (alunos[i].status == 0) {
            printf(RED_TEXT);
        }
        printf("%-10d | %-30s | %-8.2f | %-8.2f | %-8.2f | %-8.2f | %-8.2f | %-8.2f | %-8.2f | %-8.2f | %-8.2f | %-10s",
               alunos[i].matricula,
               alunos[i].nome,
               alunos[i].avaliacoes[0].ap1,
               alunos[i].avaliacoes[0].ap2,
               alunos[i].avaliacoes[0].ap3,
               alunos[i].avaliacoes[0].np,
               alunos[i].avaliacoes[1].ap1,
               alunos[i].avaliacoes[1].ap2,
               alunos[i].avaliacoes[1].ap3,
               alunos[i].avaliacoes[1].np,
               alunos[i].nf,
               alunos[i].status == 1 ? "Aprovado" : "Reprovado");
        if (alunos[i].status == 0) {
            printf(RESET_TEXT);
        }
        printf("\n");
    }
    printf("\n");
}
