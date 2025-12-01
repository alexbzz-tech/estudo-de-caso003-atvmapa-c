#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LIV 100
#define MAX_USU 100
#define MAX_EMP 100

/*STRUCTS -----*/

struct Livro {
    int cod;
    char titulo[100];
    char autor[80];
    int ano;
    int exemplares;
};

typedef struct {
    int matricula;
    char nome[100];
    char curso[50];
} Usuario;

struct Emprestimo {
    int codEmp;
    int matricula;
    int codLivro;
    int ativo; /* 1 = ativo */
};

/*VETORES ------------------------------------------------ */

struct Livro livros[MAX_LIV];
Usuario usuarios[MAX_USU];
struct Emprestimo emprestimos[MAX_EMP];

int qtdLiv = 0, qtdUsu = 0, qtdEmp = 0;

/* SALVAR ------------------------------------------------ */

void salvarDados() {
    FILE *f;
    int i;

    f = fopen("livros.txt", "w");
    if (f) {
        for (i = 0; i < qtdLiv; i++) {
            fprintf(f, "%d;%s;%s;%d;%d\n",
                livros[i].cod, livros[i].titulo, livros[i].autor,
                livros[i].ano, livros[i].exemplares);
        }
        fclose(f);
    }

    f = fopen("usuarios.txt", "w");
    if (f) {
        for (i = 0; i < qtdUsu; i++) {
            fprintf(f, "%d;%s;%s\n",
                usuarios[i].matricula, usuarios[i].nome, usuarios[i].curso);
        }
        fclose(f);
    }

    f = fopen("emprestimos.txt", "w");
    if (f) {
        for (i = 0; i < qtdEmp; i++) {
            fprintf(f, "%d;%d;%d;%d\n",
                emprestimos[i].codEmp,
                emprestimos[i].matricula,
                emprestimos[i].codLivro,
                emprestimos[i].ativo);
        }
        fclose(f);
    }
}

/* CARREGAR ------------------------------------------------------ */

void carregarDados() {
    FILE *f;
    char linha[300];
    int r;

    qtdLiv = qtdUsu = qtdEmp = 0;

    f = fopen("livros.txt", "r");
    if (f) {
        while (fgets(linha, 299, f)) {
            r = sscanf(linha, "%d;%99[^;];%79[^;];%d;%d",
                &livros[qtdLiv].cod,
                livros[qtdLiv].titulo,
                livros[qtdLiv].autor,
                &livros[qtdLiv].ano,
                &livros[qtdLiv].exemplares);
            if (r == 5)
                qtdLiv++;
        }
        fclose(f);
    }

    f = fopen("usuarios.txt", "r");
    if (f) {
        while (fgets(linha, 299, f)) {
            r = sscanf(linha, "%d;%99[^;];%49[^;]",
                &usuarios[qtdUsu].matricula,
                usuarios[qtdUsu].nome,
                usuarios[qtdUsu].curso);
            if (r == 3)
                qtdUsu++;
        }
        fclose(f);
    }

    f = fopen("emprestimos.txt", "r");
    if (f) {
        while (fgets(linha, 299, f)) {
            r = sscanf(linha, "%d;%d;%d;%d",
                &emprestimos[qtdEmp].codEmp,
                &emprestimos[qtdEmp].matricula,
                &emprestimos[qtdEmp].codLivro,
                &emprestimos[qtdEmp].ativo);
            if (r == 4)
                qtdEmp++;
        }
        fclose(f);
    }
}

/*CADASTROS ----------------------------------------------------- */

void cadastrarLivro() {
    int i;
    if (qtdLiv >= MAX_LIV) {
        printf("Limite de livros atingido!\n");
        return;
    }

    printf("Codigo: ");
    scanf("%d", &livros[qtdLiv].cod);
    getchar();

    printf("Titulo: ");
    fgets(livros[qtdLiv].titulo, 100, stdin);
    livros[qtdLiv].titulo[strcspn(livros[qtdLiv].titulo, "\n")] = 0;

    printf("Autor: ");
    fgets(livros[qtdLiv].autor, 80, stdin);
    livros[qtdLiv].autor[strcspn(livros[qtdLiv].autor, "\n")] = 0;

    printf("Ano: ");
    scanf("%d", &livros[qtdLiv].ano);

    printf("Exemplares: ");
    scanf("%d", &livros[qtdLiv].exemplares);

    qtdLiv++;

    printf("Livro cadastrado!\n");
}

void cadastrarUsuario() {
    int i;
    if (qtdUsu >= MAX_USU) {
        printf("Limite de usuarios atingido!\n");
        return;
    }

    usuarios[qtdUsu].matricula = 1000 + qtdUsu;
    printf("Matricula gerada automaticamente: %d\n",
    usuarios[qtdUsu].matricula);
    getchar();

    printf("Nome: ");
    fgets(usuarios[qtdUsu].nome, 100, stdin);
    usuarios[qtdUsu].nome[strcspn(usuarios[qtdUsu].nome, "\n")] = 0;

    printf("Curso: ");
    fgets(usuarios[qtdUsu].curso, 50, stdin);
    usuarios[qtdUsu].curso[strcspn(usuarios[qtdUsu].curso, "\n")] = 0;

    qtdUsu++;

    printf("Usuario cadastrado!\n");
}

/*FUNÇÕES DE BUSCA --------------------------------------------------- */

int buscarLivro(int cod) {
    int i;
    for (i = 0; i < qtdLiv; i++)
        if (livros[i].cod == cod)
            return i;
    return -1;
}

int buscarUsuario(int mat) {
    int i;
    for (i = 0; i < qtdUsu; i++)
        if (usuarios[i].matricula == mat)
            return i;
    return -1;
}

/*EMPRESTIMO ------------------------------------------------------------ */

void emprestarLivro() {
    int mat, cod, i;

    printf("Matricula: ");
    scanf("%d", &mat);

    printf("Codigo do livro: ");
    scanf("%d", &cod);

    if (buscarUsuario(mat) < 0) {
        printf("Usuario nao encontrado!\n");
        return;
    }

    i = buscarLivro(cod);
    if (i < 0) {
        printf("Livro nao encontrado!\n");
        return;
    }

    if (livros[i].exemplares <= 0) {
        printf("Sem exemplares disponiveis!\n");
        return;
    }

    emprestimos[qtdEmp].codEmp = 1000 + qtdEmp;
    emprestimos[qtdEmp].matricula = mat;
    emprestimos[qtdEmp].codLivro = cod;
    emprestimos[qtdEmp].ativo = 1;

    livros[i].exemplares--;

    printf("Emprestimo realizado!\n");
    printf("Codigo do emprestimo gerado: %d\n", emprestimos[qtdEmp].codEmp);
    qtdEmp++;
}

/*DEVOLUÇÃO ------------------------------------------- */

void devolverLivro() {
    int codEmp, i, j;

    printf("Codigo do emprestimo: ");
    scanf("%d", &codEmp);

    for (i = 0; i < qtdEmp; i++) {
        if (emprestimos[i].codEmp == codEmp && emprestimos[i].ativo == 1) {
            j = buscarLivro(emprestimos[i].codLivro);
            if (j >= 0)
                livros[j].exemplares++;

            emprestimos[i].ativo = 0;

            printf("Devolvido!\n");
            return;
        }
    }

    printf("Emprestimo nao encontrado.\n");
}

/*MENU ----------------------------------------------------------- */

void menu() {
    int op;
    do {
        printf("\n===== BIBLIOTECA =====\n");
        printf("1 - Cadastrar livro\n");
        printf("2 - Cadastrar usuario\n");
        printf("3 - Emprestimo\n");
        printf("4 - Devolucao\n");
        printf("5 - Salvar dados\n");
        printf("6 - Sair\n");
        printf("Opcao: ");
        scanf("%d", &op);

        switch (op) {
            case 1: cadastrarLivro(); break;
            case 2: cadastrarUsuario(); break;
            case 3: emprestarLivro(); break;
            case 4: devolverLivro(); break;
            case 5: salvarDados(); printf("Salvo.\n"); break;
            case 6: printf("Saindo...\n"); break;
            default: printf("Opcao invalida!\n");
        }

    } while (op != 6);
}

/* MAIN ------------------------------------------------- */

int main() {
    carregarDados();
    menu();
    return 0;
}




