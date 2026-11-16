// ============================================================================
// Arquivo: emprestimos.h | MODULO 4: EMPRESTIMOS E RELATORIOS
// Autor: Carlos Eduardo Silveira Atner 49572911
// Este modulo CRUZA dados dos modulos A (leitores.h) e B (livros.h)
// ============================================================================
#ifndef EMPRESTIMOS_H
#define EMPRESTIMOS_H

#include <stdio.h>
#include <string.h>
#include "leitores.h"
#include "livros.h"

struct Emprestimo {
    int idEmprestimo;
    int idLeitor;
    int idLivro;
    int dias;                // dias que o livro ficou emprestado
    float multa;             // valor da multa em reais
    int devolvido;           // 1 = Devolvido, 0 = Em aberto
    char dataEmprestimo[11]; // formato dd/mm/aaaa
};
struct Emprestimo historicoEmprestimos[50];
int totalEmprestimos = 0;
int proximoIdEmprestimo = 1; // ID que nunca se repete, mesmo apos cancelamentos

// Matriz 2D dos relatorios: linha = posicao do leitor, coluna = posicao do livro,
// valor = quantas vezes esse leitor pegou esse livro
int matrizEmprestimos[50][50];

// Usada no ranking de livros (Bubble Sort em structs)
struct RankingLivro {
    int posLivro; // posicao do livro no vetor acervo
    int total;    // quantos emprestimos o livro teve
};

// Grava (ou regrava) todo o vetor no arquivo emprestimos.csv
// Autor: Carlos Eduardo Silveira Atner 49572911
void salvarEmprestimosCSV() {
    int i;
    FILE *arq = fopen("emprestimos.csv", "w"); // modo "w" zera o arquivo e regrava tudo
    if (arq == NULL) {
        printf("[ERRO] Nao foi possivel gravar emprestimos.csv!\n");
        return;
    }
    for (i = 0; i < totalEmprestimos; i++) {
        fprintf(arq, "%d;%d;%d;%d;%.2f;%d;%s\n",
                historicoEmprestimos[i].idEmprestimo,
                historicoEmprestimos[i].idLeitor,
                historicoEmprestimos[i].idLivro,
                historicoEmprestimos[i].dias,
                historicoEmprestimos[i].multa,
                historicoEmprestimos[i].devolvido,
                historicoEmprestimos[i].dataEmprestimo);
    }
    fclose(arq);
}

// Carrega os emprestimos do arquivo emprestimos.csv ao abrir o programa
// Autor: Carlos Eduardo Silveira Atner 49572911
void carregarEmprestimosCSV() {
    int i;
    FILE *arq = fopen("emprestimos.csv", "r");
    if (arq == NULL) return; // se o arquivo ainda nao existe, comeca com 0 registros

    totalEmprestimos = 0;
    while (totalEmprestimos < 50 &&
           fscanf(arq, "%d;%d;%d;%d;%f;%d;%10[^\n]\n",
                  &historicoEmprestimos[totalEmprestimos].idEmprestimo,
                  &historicoEmprestimos[totalEmprestimos].idLeitor,
                  &historicoEmprestimos[totalEmprestimos].idLivro,
                  &historicoEmprestimos[totalEmprestimos].dias,
                  &historicoEmprestimos[totalEmprestimos].multa,
                  &historicoEmprestimos[totalEmprestimos].devolvido,
                  historicoEmprestimos[totalEmprestimos].dataEmprestimo) == 7) {
        totalEmprestimos++;
    }
    fclose(arq);

    // o proximo ID tem de ser maior que o maior ID que veio do arquivo
    proximoIdEmprestimo = 1;
    for (i = 0; i < totalEmprestimos; i++) {
        if (historicoEmprestimos[i].idEmprestimo >= proximoIdEmprestimo) {
            proximoIdEmprestimo = historicoEmprestimos[i].idEmprestimo + 1;
        }
    }
}

// Procura um emprestimo pelo ID. Retorna a posicao no vetor ou -1 se nao achar.
// Autor: Carlos Eduardo Silveira Atner 49572911
int buscarEmprestimo(int id) {
    int i;
    for (i = 0; i < totalEmprestimos; i++) {
        if (historicoEmprestimos[i].idEmprestimo == id) {
            return i;
        }
    }
    return -1;
}

// 1. CADASTRAR: registra um emprestimo CRUZANDO leitor (A) e livro (B)
// Regras: leitor e livro precisam existir, leitor sem multa pendente, livro com exemplar
// Autor: Carlos Eduardo Silveira Atner 49572911
void cadastrarEmprestimo() {
    int pos = totalEmprestimos; // proxima posicao livre do vetor
    int idLeitor, idLivro, posLeitor, posLivro;

    if (totalEmprestimos >= 50) {
        printf("\n[ERRO] Limite de 50 emprestimos atingido!\n");
        return;
    }

    printf("\nID do leitor: ");
    scanf("%d", &idLeitor);
    posLeitor = buscarIndiceLeitor(idLeitor);
    if (posLeitor == -1) {
        printf("\n[ERRO] Leitor nao encontrado no fichario!\n");
        return;
    }
    if (listaLeitores[posLeitor].multaPendente > 0) {
        printf("\n[BLOQUEADO] %s tem multa pendente de R$ %.2f. Quite a multa antes de um novo emprestimo.\n",
               listaLeitores[posLeitor].nome, listaLeitores[posLeitor].multaPendente);
        return;
    }

    printf("ID do livro: ");
    scanf("%d", &idLivro);
    posLivro = buscarIndiceLivro(idLivro);
    if (posLivro == -1) {
        printf("\n[ERRO] Livro nao encontrado no acervo!\n");
        return;
    }
    if (acervo[posLivro].exemplares <= 0) {
        printf("\n[ERRO] Nenhum exemplar de \"%s\" disponivel no momento!\n", acervo[posLivro].titulo);
        return;
    }

    printf("Data do emprestimo (dd/mm/aaaa): ");
    scanf("%10s", historicoEmprestimos[pos].dataEmprestimo);

    historicoEmprestimos[pos].idEmprestimo = proximoIdEmprestimo;
    historicoEmprestimos[pos].idLeitor = idLeitor;
    historicoEmprestimos[pos].idLivro = idLivro;
    historicoEmprestimos[pos].dias = 0;
    historicoEmprestimos[pos].multa = 0.0;
    historicoEmprestimos[pos].devolvido = 0;

    // baixa no estoque (modulo B) e atualiza o leitor (modulo A)
    acervo[posLivro].exemplares--;
    listaLeitores[posLeitor].livrosEmprestados++;

    totalEmprestimos++;
    proximoIdEmprestimo++;

    salvarEmprestimosCSV();
    salvarLivrosCSV();
    salvarLeitoresCSV();

    printf("\nEmprestimo %d registrado: \"%s\" para %s. Exemplares restantes: %d\n",
           historicoEmprestimos[pos].idEmprestimo, acervo[posLivro].titulo,
           listaLeitores[posLeitor].nome, acervo[posLivro].exemplares);
}

// 2. LISTAR: mostra todos os emprestimos, com o nome do leitor e o titulo do livro
// Autor: Carlos Eduardo Silveira Atner 49572911
void listarEmprestimos() {
    int i, posLeitor, posLivro;

    if (totalEmprestimos == 0) {
        printf("\nNenhum emprestimo registrado.\n");
        return;
    }

    printf("\n--- EMPRESTIMOS REGISTRADOS (%d) ---\n", totalEmprestimos);
    for (i = 0; i < totalEmprestimos; i++) {
        posLeitor = buscarIndiceLeitor(historicoEmprestimos[i].idLeitor);
        posLivro = buscarIndiceLivro(historicoEmprestimos[i].idLivro);

        printf("ID %d | ", historicoEmprestimos[i].idEmprestimo);
        if (posLeitor != -1) {
            printf("%s", listaLeitores[posLeitor].nome);
        } else {
            printf("Leitor %d (removido)", historicoEmprestimos[i].idLeitor);
        }
        printf(" | ");
        if (posLivro != -1) {
            printf("%s", acervo[posLivro].titulo);
        } else {
            printf("Livro %d (removido)", historicoEmprestimos[i].idLivro);
        }
        printf(" | %s | %d dias | Multa R$ %.2f | ",
               historicoEmprestimos[i].dataEmprestimo,
               historicoEmprestimos[i].dias,
               historicoEmprestimos[i].multa);
        if (historicoEmprestimos[i].devolvido == 1) {
            printf("Devolvido\n");
        } else {
            printf("Em aberto\n");
        }
    }
}

// 3. EDITAR: registra a devolucao, calcula a multa, devolve o exemplar ao estoque
// Regra assumida: prazo de 7 dias, multa de R$ 0,50 por dia de atraso
// Autor: Carlos Eduardo Silveira Atner 49572911
void devolverEmprestimo() {
    int id, pos, dias, posLeitor, posLivro;

    printf("\nID do emprestimo: ");
    scanf("%d", &id);

    pos = buscarEmprestimo(id);
    if (pos == -1) {
        printf("\n[ERRO] Emprestimo nao encontrado!\n");
        return;
    }
    if (historicoEmprestimos[pos].devolvido == 1) {
        printf("\n[AVISO] Este emprestimo ja foi devolvido.\n");
        return;
    }

    printf("Quantos dias o livro ficou emprestado? ");
    scanf("%d", &dias);

    historicoEmprestimos[pos].dias = dias;
    if (dias > 7) {
        historicoEmprestimos[pos].multa = (dias - 7) * 0.50;
    } else {
        historicoEmprestimos[pos].multa = 0.0;
    }
    historicoEmprestimos[pos].devolvido = 1;

    // devolve o exemplar ao estoque (B) e atualiza o leitor (A)
    posLivro = buscarIndiceLivro(historicoEmprestimos[pos].idLivro);
    posLeitor = buscarIndiceLeitor(historicoEmprestimos[pos].idLeitor);
    if (posLivro != -1) {
        acervo[posLivro].exemplares++;
    }
    if (posLeitor != -1) {
        listaLeitores[posLeitor].livrosEmprestados--;
        listaLeitores[posLeitor].multaPendente = listaLeitores[posLeitor].multaPendente
                                                 + historicoEmprestimos[pos].multa;
    }

    salvarEmprestimosCSV();
    salvarLivrosCSV();
    salvarLeitoresCSV();

    printf("\nDevolucao registrada! Multa: R$ %.2f\n", historicoEmprestimos[pos].multa);
}

// 4. REMOVER: cancela um emprestimo (puxa os proximos uma posicao para tras)
// Se o livro ainda estava emprestado, o exemplar volta ao estoque
// Autor: Carlos Eduardo Silveira Atner 49572911
void cancelarEmprestimo() {
    int id, pos, i, posLeitor, posLivro;

    printf("\nID do emprestimo a cancelar: ");
    scanf("%d", &id);

    pos = buscarEmprestimo(id);
    if (pos == -1) {
        printf("\n[ERRO] Emprestimo nao encontrado!\n");
        return;
    }

    if (historicoEmprestimos[pos].devolvido == 0) {
        posLivro = buscarIndiceLivro(historicoEmprestimos[pos].idLivro);
        posLeitor = buscarIndiceLeitor(historicoEmprestimos[pos].idLeitor);
        if (posLivro != -1) {
            acervo[posLivro].exemplares++;
        }
        if (posLeitor != -1) {
            listaLeitores[posLeitor].livrosEmprestados--;
        }
    }

    for (i = pos; i < totalEmprestimos - 1; i++) {
        historicoEmprestimos[i] = historicoEmprestimos[i + 1];
    }
    totalEmprestimos--;

    salvarEmprestimosCSV();
    salvarLivrosCSV();
    salvarLeitoresCSV();

    printf("\nEmprestimo %d cancelado.\n", id);
}

// Preenche a matriz 2D contando os emprestimos de cada leitor para cada livro
// Autor: Carlos Eduardo Silveira Atner 49572911
void montarMatriz() {
    int i, l, k, posLeitor, posLivro;

    for (l = 0; l < 50; l++) {
        for (k = 0; k < 50; k++) {
            matrizEmprestimos[l][k] = 0;
        }
    }

    for (i = 0; i < totalEmprestimos; i++) {
        posLeitor = buscarIndiceLeitor(historicoEmprestimos[i].idLeitor);
        posLivro = buscarIndiceLivro(historicoEmprestimos[i].idLivro);
        if (posLeitor != -1 && posLivro != -1) {
            matrizEmprestimos[posLeitor][posLivro]++;
        }
    }
}

// RELATORIO 1: ranking dos livros mais emprestados (ordenado com Bubble Sort)
// Autor: Carlos Eduardo Silveira Atner 49572911
void relatorioRankingLivros() {
    struct RankingLivro ranking[50];
    struct RankingLivro aux;
    int l, k, i, j, soma, n = 0;

    if (totalEmprestimos == 0) {
        printf("\nNenhum emprestimo registrado.\n");
        return;
    }

    montarMatriz();

    // soma cada coluna da matriz = total de emprestimos de cada livro
    for (k = 0; k < totalLivros; k++) {
        if (acervo[k].ativo == 1) {
            soma = 0;
            for (l = 0; l < totalLeitores; l++) {
                soma = soma + matrizEmprestimos[l][k];
            }
            ranking[n].posLivro = k;
            ranking[n].total = soma;
            n++;
        }
    }

    // Bubble Sort: ordem decrescente de total
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - 1 - i; j++) {
            if (ranking[j].total < ranking[j + 1].total) {
                aux = ranking[j];
                ranking[j] = ranking[j + 1];
                ranking[j + 1] = aux;
            }
        }
    }

    printf("\n--- RANKING: LIVROS MAIS EMPRESTADOS ---\n");
    for (i = 0; i < n; i++) {
        printf("%d. %s (ID %d) - %d emprestimo(s)\n",
               i + 1, acervo[ranking[i].posLivro].titulo,
               acervo[ranking[i].posLivro].id, ranking[i].total);
    }
}

// RELATORIO 2: leitor com mais emprestimos (soma de cada linha da matriz)
// Autor: Carlos Eduardo Silveira Atner 49572911
void relatorioLeitorMaisEmprestimos() {
    int l, k, soma, maior = 0, posMaior = -1;

    if (totalEmprestimos == 0) {
        printf("\nNenhum emprestimo registrado.\n");
        return;
    }

    montarMatriz();

    for (l = 0; l < totalLeitores; l++) {
        if (listaLeitores[l].ativo == 1) {
            soma = 0;
            for (k = 0; k < totalLivros; k++) {
                soma = soma + matrizEmprestimos[l][k];
            }
            if (soma > maior) {
                maior = soma;
                posMaior = l;
            }
        }
    }

    if (posMaior == -1) {
        printf("\nNenhum emprestimo de leitor ativo.\n");
    } else {
        printf("\nLeitor com mais emprestimos: %s (ID %d) - %d emprestimo(s)\n",
               listaLeitores[posMaior].nome, listaLeitores[posMaior].idLeitor, maior);
    }
}

// RELATORIO 3: emprestimos de um leitor (busca textual pelo nome com strstr)
// Autor: Carlos Eduardo Silveira Atner 49572911
void relatorioEmprestimosDoLeitor() {
    char termo[50];
    int i, posLeitor, posLivro, achou = 0;

    printf("\nDigite parte do nome do leitor: ");
    scanf(" %49[^\n]", termo);

    printf("\n--- Emprestimos dos leitores encontrados ---\n");
    for (i = 0; i < totalEmprestimos; i++) {
        posLeitor = buscarIndiceLeitor(historicoEmprestimos[i].idLeitor);
        if (posLeitor != -1 && strstr(listaLeitores[posLeitor].nome, termo) != NULL) {
            posLivro = buscarIndiceLivro(historicoEmprestimos[i].idLivro);
            printf("%s | Emprestimo %d | ", listaLeitores[posLeitor].nome,
                   historicoEmprestimos[i].idEmprestimo);
            if (posLivro != -1) {
                printf("%s", acervo[posLivro].titulo);
            } else {
                printf("Livro %d (removido)", historicoEmprestimos[i].idLivro);
            }
            printf(" | %s | ", historicoEmprestimos[i].dataEmprestimo);
            if (historicoEmprestimos[i].devolvido == 1) {
                printf("Devolvido\n");
            } else {
                printf("Em aberto\n");
            }
            achou = 1;
        }
    }

    if (achou == 0) {
        printf("Nenhum emprestimo encontrado para esse nome.\n");
    }
}

// RELATORIO 4: mostra a matriz 2D leitor x livro
// Autor: Carlos Eduardo Silveira Atner 49572911
void relatorioMatriz() {
    int l, k, soma;

    if (totalEmprestimos == 0) {
        printf("\nNenhum emprestimo registrado.\n");
        return;
    }

    montarMatriz();

    printf("\n--- MATRIZ DE EMPRESTIMOS (linha = leitor, coluna = ID do livro) ---\n");
    printf("%-24s", "Leitor");
    for (k = 0; k < totalLivros; k++) {
        if (acervo[k].ativo == 1) {
            printf("%4d", acervo[k].id);
        }
    }
    printf("%7s\n", "Total");

    for (l = 0; l < totalLeitores; l++) {
        if (listaLeitores[l].ativo == 1) {
            printf("%-24s", listaLeitores[l].nome);
            soma = 0;
            for (k = 0; k < totalLivros; k++) {
                if (acervo[k].ativo == 1) {
                    printf("%4d", matrizEmprestimos[l][k]);
                    soma = soma + matrizEmprestimos[l][k];
                }
            }
            printf("%7d\n", soma);
        }
    }
}

// MODULO 4: SUBMENU DE EMPRESTIMOS E RELATORIOS
// Autor: Carlos Eduardo Silveira Atner 49572911
void moduloEmprestimosRelatorios() {
    int opcaoEmprestimo;
    do {
        printf("\n--- [MODULO 4: EMPRESTIMOS E RELATORIOS (Carlos Eduardo Silveira Atner)] ---\n");
        printf("1. Registrar Novo Emprestimo (Cruza Leitor + Livro)\n");
        printf("2. Listar Emprestimos\n");
        printf("3. Registrar Devolucao\n");
        printf("4. Cancelar Emprestimo\n");
        printf("5. [RELATORIO] Ranking de Livros Mais Emprestados\n");
        printf("6. [RELATORIO] Leitor com Mais Emprestimos\n");
        printf("7. [RELATORIO] Emprestimos de um Leitor (busca por nome)\n");
        printf("8. [RELATORIO] Matriz Leitor x Livro\n");
        printf("0. Voltar ao Menu Principal\n");
        printf("Escolha uma acao: ");
        scanf("%d", &opcaoEmprestimo);

        switch (opcaoEmprestimo) {
            case 1:
                cadastrarEmprestimo();
                break;
            case 2:
                listarEmprestimos();
                break;
            case 3:
                devolverEmprestimo();
                break;
            case 4:
                cancelarEmprestimo();
                break;
            case 5:
                relatorioRankingLivros();
                break;
            case 6:
                relatorioLeitorMaisEmprestimos();
                break;
            case 7:
                relatorioEmprestimosDoLeitor();
                break;
            case 8:
                relatorioMatriz();
                break;
            case 0:
                printf("\nRetornando ao menu principal...\n");
                break;
            default:
                printf("\n[ERRO] Opcao invalida no Modulo de Emprestimos!\n");
        }
    } while (opcaoEmprestimo != 0);
}

#endif
