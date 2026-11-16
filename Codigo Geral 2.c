#include <stdio.h>
#include <stdlib.h>  // system()
#include <locale.h>  // setlocale()

// Cada integrante tem o seu modulo (.h)
#include "leitores.h"     // Integrante A
#include "livros.h"       // Integrante B
#include "autores.h"      // Integrante C
#include "emprestimos.h"  // Integrante D

// Limpa o terminal: "cls" no Windows (Dev-C++), "clear" nos demais sistemas
void limparTela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Funcao visual comum da equipe
void exibirCabecalho() {
    printf("\n======================================================\n");
    printf("   BIBLIOTECA CLASSICA - SISTEMA DE GESTAO DE ACERVO   \n");
    printf("======================================================\n");
}

int main() {
    int opcaoPrincipal;

    // So LC_CTYPE (acentos): LC_ALL trocaria o ponto decimal por virgula e quebraria a leitura dos .csv
    setlocale(LC_CTYPE, "Portuguese");

    // Carrega os dados salvos nos arquivos .csv ao abrir o programa
    carregarLeitoresCSV();
    carregarLivrosCSV();
    carregarAutoresCSV();
    carregarEmprestimosCSV();

    do {
        exibirCabecalho();
        printf("SELECIONE A PERSPECTIVA DO SISTEMA:\n");
        printf("1. Modulo de Leitores / Fichario (Integrante A)\n");
        printf("2. Modulo de Livros / Acervo (Integrante B)\n");
        printf("3. Modulo de Autores (Integrante C)\n");
        printf("4. Modulo de Emprestimos e Relatorios (Integrante D)\n");
        printf("0. Encerrar o Sistema\n");
        printf("------------------------------------------------------\n");
        printf("Digite a opcao desejada: ");
        scanf("%d", &opcaoPrincipal);

        switch (opcaoPrincipal) {
            case 1:
                limparTela();
                moduloLeitores();
                limparTela();
                break;
            case 2:
                limparTela();
                moduloAcervo();
                limparTela();
                break;
            case 3:
                limparTela();
                moduloAutores();
                limparTela();
                break;
            case 4:
                limparTela();
                moduloEmprestimosRelatorios();
                limparTela();
                break;
            case 0:
                printf("\nEncerrando o sistema da Biblioteca Classica... Ate logo!\n");
                break;
            default:
                printf("\n[ALERTA] Opcao invalida! Digite um numero de 0 a 4.\n");
        }
    } while (opcaoPrincipal != 0);

    return 0;
}
