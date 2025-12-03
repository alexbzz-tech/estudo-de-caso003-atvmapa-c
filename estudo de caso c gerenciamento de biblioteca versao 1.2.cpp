#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_LIV 100
#define MAX_USU 100
#define MAX_EMP 100

/*STRUCTS -----*/

struct Livro {
    int cod;
    char titulo[100];
    char autor[80];
    char editora[60];
    int ano;
    int exemplares;
    int status;
};

typedef struct {
    int matricula;
    char nome[100];
    char curso[50];
    char telefone[15];
    int dia, mes, ano;
} Usuario;

struct Emprestimo {
    int codEmp;
    int matricula;
    int codLivro;
    int diaEmp, mesEmp, anoEmp;
    int diaPrev, mesPrev, anoPrev;
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

    /* SALVAR LIVROS */
    f = fopen("livros.txt", "w");
    if (f) {
        for (i = 0; i < qtdLiv; i++) {
            fprintf(f, "%d;%s;%s;%s;%d;%d;%d\n",
                livros[i].cod,
                livros[i].titulo,
                livros[i].autor,
                livros[i].editora,
                livros[i].ano,
                livros[i].exemplares,
                livros[i].status
            );
        }
        fclose(f);
    }

    /* SALVAR USUÁRIOS */
    f = fopen("usuarios.txt", "w");
    if (f) {
        for (i = 0; i < qtdUsu; i++) {
            fprintf(f, "%d;%s;%s;%s;%d;%d;%d\n",
                usuarios[i].matricula,
                usuarios[i].nome,
                usuarios[i].curso,
                usuarios[i].telefone,
                usuarios[i].dia,
                usuarios[i].mes,
                usuarios[i].ano
            );
        }
        fclose(f);
    }

    /* SALVAR EMPRESTIMOS */
    f = fopen("emprestimos.txt", "w");
    if (f) {
        for (i = 0; i < qtdEmp; i++) {
            fprintf(f, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d\n",
                emprestimos[i].codEmp,
                emprestimos[i].matricula,
                emprestimos[i].codLivro,
                emprestimos[i].diaEmp,
                emprestimos[i].mesEmp,
                emprestimos[i].anoEmp,
                emprestimos[i].diaPrev,
                emprestimos[i].mesPrev,
                emprestimos[i].anoPrev,
                emprestimos[i].ativo
            );
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

    /* CARREGAR LIVROS */
    f = fopen("livros.txt", "r");
    if (f) {
        while (fgets(linha, 299, f)) {
            r = sscanf(linha, "%d;%99[^;];%79[^;];%59[^;];%d;%d;%d",
                &livros[qtdLiv].cod,
                livros[qtdLiv].titulo,
                livros[qtdLiv].autor,
                livros[qtdLiv].editora,
                &livros[qtdLiv].ano,
                &livros[qtdLiv].exemplares,
                &livros[qtdLiv].status
);

            if (r == 7)
                qtdLiv++;
        }
        fclose(f);
    }

    /* CARREGAR USUÁRIOS */
    f = fopen("usuarios.txt", "r");
    if (f) {
        while (fgets(linha, 299, f)) {
            r = sscanf(linha, "%d;%99[^;];%49[^;];%14[^;];%d;%d;%d",
        &usuarios[qtdUsu].matricula,
        usuarios[qtdUsu].nome,
        usuarios[qtdUsu].curso,
        usuarios[qtdUsu].telefone,
        &usuarios[qtdUsu].dia,
        &usuarios[qtdUsu].mes,
        &usuarios[qtdUsu].ano
);

            if (r ==7)
                qtdUsu++;
        }
        fclose(f);
    }

    /* CARREGAR EMPRESTIMOS */
    f = fopen("emprestimos.txt", "r");
    if (f) {
        while (fgets(linha, 299, f)) {
            r = sscanf(linha, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d",
        &emprestimos[qtdEmp].codEmp,
   	  &emprestimos[qtdEmp].matricula,
    	  &emprestimos[qtdEmp].codLivro,

  	  &emprestimos[qtdEmp].diaEmp,
	  &emprestimos[qtdEmp].mesEmp,
   	  &emprestimos[qtdEmp].anoEmp,

        &emprestimos[qtdEmp].diaPrev,
        &emprestimos[qtdEmp].mesPrev,
        &emprestimos[qtdEmp].anoPrev,

        &emprestimos[qtdEmp].ativo
);

            if (r == 10)
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
    
    printf("Editora: ");
    fgets(livros[qtdLiv].editora, 60, stdin);
    livros[qtdLiv].editora[strcspn(livros[qtdLiv].editora, "\n")] = 0;
    
    printf("Ano: ");
    scanf("%d", &livros[qtdLiv].ano);

    printf("Exemplares: ");
    scanf("%d", &livros[qtdLiv].exemplares);
    
    livros[qtdLiv].status = (livros[qtdLiv].exemplares > 0) ? 1 : 0;

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
    printf("Matricula gerada automaticamente: %d\n", usuarios[qtdUsu].matricula);
    while (getchar() != '\n');

    printf("Nome: ");
    fgets(usuarios[qtdUsu].nome, 100, stdin);
    usuarios[qtdUsu].nome[strcspn(usuarios[qtdUsu].nome, "\n")] = 0;

    printf("Curso: ");
    fgets(usuarios[qtdUsu].curso, 50, stdin);
    usuarios[qtdUsu].curso[strcspn(usuarios[qtdUsu].curso, "\n")] = 0;

    printf("Telefone: ");
    fgets(usuarios[qtdUsu].telefone, 15, stdin);
    usuarios[qtdUsu].telefone[strcspn(usuarios[qtdUsu].telefone, "\n")] = 0;

    printf("Dia do cadastro: ");
    scanf("%d", &usuarios[qtdUsu].dia);

    printf("Mes do cadastro: ");
    scanf("%d", &usuarios[qtdUsu].mes);

    printf("Ano do cadastro: ");
    scanf("%d", &usuarios[qtdUsu].ano);

    qtdUsu++;
    
    printf("Aluno cadastrado!\n");
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
    int mat, cod, i, posUsu;

    printf("Matricula: ");
    scanf("%d", &mat);

    /* BUSCA O USUÁRIO */
    posUsu = buscarUsuario(mat);
    if (posUsu < 0) {
        printf("Usuario nao encontrado!\n");
        return;
    }

    /* MOSTRA OS DADOS DO USUÁRIO */
    printf("\n--- Dados do Usuario ---\n");
    printf("Nome: %s\n", usuarios[posUsu].nome);
    printf("Curso: %s\n", usuarios[posUsu].curso);
    printf("Telefone: %s\n", usuarios[posUsu].telefone);
    printf("------------------------\n\n");

    /* DEPOIS CONTINUA PEDINDO O LIVRO */
    printf("Codigo do livro: ");
    scanf("%d", &cod);

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
    /* serve para pegar a data atual */
    time_t t = time(NULL);
    struct tm data = *localtime(&t);

    emprestimos[qtdEmp].diaEmp = data.tm_mday;
    emprestimos[qtdEmp].mesEmp = data.tm_mon + 1;
    emprestimos[qtdEmp].anoEmp = data.tm_year + 1900;

    /* serve para caucular a devolucao prevista (7 DIAS) */
    t += 7 * 24 * 3600;  
    struct tm prev = *localtime(&t);

    emprestimos[qtdEmp].diaPrev = prev.tm_mday;
    emprestimos[qtdEmp].mesPrev = prev.tm_mon + 1;
    emprestimos[qtdEmp].anoPrev = prev.tm_year + 1900;

    emprestimos[qtdEmp].matricula = mat;
    emprestimos[qtdEmp].codLivro = cod;
    emprestimos[qtdEmp].ativo = 1;

    livros[i].exemplares--;
    // status: disponível se ainda houver exemplares
    livros[i].status = (livros[i].exemplares > 0) ? 1 : 0;

    printf("Emprestimo realizado!\n");
    printf("Codigo do emprestimo gerado: %d\n", emprestimos[qtdEmp].codEmp);
    printf("Titulo do livro: %s\n", livros[i].titulo);

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
            if (j >= 0) {
                livros[j].exemplares++;
                // Como houve devolução, livro volta a estar disponível
                livros[j].status = 1;
            }

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
        if (scanf("%d", &op) != 1) {
    	  printf("Entrada inválida!\n");
	  while (getchar() != '\n');
   	  continue;
}


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

void inicializarLivros() {
    qtdLiv = 0;
}

void inicializarUsuarios() {
    qtdUsu = 0;
}

void inicializarEmprestimos() {
    qtdEmp = 0;
}

void inicializarSistema() {
    inicializarLivros();
    inicializarUsuarios();
    inicializarEmprestimos();
}

int main() {
    inicializarSistema(); 
    carregarDados();
    menu();
    return 0;
}





