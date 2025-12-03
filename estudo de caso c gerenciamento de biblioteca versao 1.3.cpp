
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define MAX_LIV 200
#define MAX_USU 200
#define MAX_EMP 500

typedef struct {
    int cod;
    char titulo[100];
    char autor[80];
    char editora[60];
    int ano;
    int exemplares;
    int status; /* 1 = disponivel, 0 = indisponivel */
    int totalEmprestimos;
} Livro;

typedef struct {
    int matricula;
    char nome[100];
    char curso[50];
    char telefone[15];
    int dia, mes, ano; /* data de cadastro */
} Usuario;

typedef struct {
    int codEmp;
    int matricula;
    int codLivro;
    int diaEmp, mesEmp, anoEmp;
    int diaPrev, mesPrev, anoPrev;
    int ativo; /* 1 = ativo, 0 = devolvido */
    int renovacoes;
} Emprestimo;

/* Vetores globais */
static Livro livros[MAX_LIV];
static Usuario usuarios[MAX_USU];
static Emprestimo emprestimos[MAX_EMP];

static int qtdLiv = 0, qtdUsu = 0, qtdEmp = 0; /* qtdEmp = total de registros gravados no vetor */

/* UTILITÁRIOS ----------------------------------------- */

void flush_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int lerInt(const char *prompt, int *out) {
    int ok = 0;
    char linha[64];
    if (prompt && prompt[0]) printf("%s", prompt);
    if (!fgets(linha, sizeof(linha), stdin)) return 0;
    if (sscanf(linha, "%d", out) == 1) ok = 1;
    return ok;
}

/* Implementação portável de strcasestr */
char *my_strcasestr(const char *haystack, const char *needle) {
    if (!haystack || !needle) return NULL;
    size_t nlen = strlen(needle);
    if (nlen == 0) return (char *)haystack;
    for (; *haystack; haystack++) {
        size_t i;
        for (i = 0; i < nlen; i++) {
            char a = haystack[i];
            char b = needle[i];
            if (!b) break;
            if (tolower((unsigned char)a) != tolower((unsigned char)b)) break;
        }
        if (i == nlen) return (char *)haystack;
    }
    return NULL;
}

/* Adicionar dias, diasEntre, gerarTimestamp */
void adicionarDias(int dia, int mes, int ano, int dias, int *rdia, int *rmes, int *rano) {
    struct tm t = {0};
    t.tm_mday = dia;
    t.tm_mon = mes - 1;
    t.tm_year = ano - 1900;
    time_t tt = mktime(&t);
    if (tt == (time_t)-1) {
        *rdia = dia + dias; *rmes = mes; *rano = ano;
        return;
    }
    tt += (time_t) dias * 24 * 3600;
    struct tm *r = localtime(&tt);
    *rdia = r->tm_mday;
    *rmes = r->tm_mon + 1;
    *rano = r->tm_year + 1900;
}

int diasEntre(int dia, int mes, int ano) {
    struct tm t = {0};
    t.tm_mday = dia; t.tm_mon = mes - 1; t.tm_year = ano - 1900;
    time_t tt = mktime(&t);
    time_t nn = time(NULL);
    if (tt == (time_t)-1) return 0;
    double diff = difftime(nn, tt);
    return (int)(diff / (24 * 3600));
}

void gerarTimestamp(char *buf, size_t len) {
    time_t t = time(NULL);
    struct tm *tmv = localtime(&t);
    snprintf(buf, len, "%04d%02d%02d_%02d%02d%02d",
             tmv->tm_year + 1900, tmv->tm_mon + 1, tmv->tm_mday,
             tmv->tm_hour, tmv->tm_min, tmv->tm_sec);
}

/* reporta mensagem de erro em stderr em caso de falha */
int copiarArquivo(const char *src, const char *dest) {
    FILE *fs = fopen(src, "rb");
    if (!fs) {
        return 0;
    }
    FILE *fd = fopen(dest, "wb");
    if (!fd) { fclose(fs); return 0; }
    char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), fs)) > 0) {
        fwrite(buf, 1, n, fd);
    }
    fclose(fs);
    fclose(fd);
    return 1;
}

void backupAutomatico() {
    char ts[64];
    gerarTimestamp(ts, sizeof(ts));
    char dst[128];

    snprintf(dst, sizeof(dst), "backup_livros_%s.txt", ts);
    copiarArquivo("livros.txt", dst);

    snprintf(dst, sizeof(dst), "backup_usuarios_%s.txt", ts);
    copiarArquivo("usuarios.txt", dst);

    snprintf(dst, sizeof(dst), "backup_emprestimos_%s.txt", ts);
    copiarArquivo("emprestimos.txt", dst);
}

/* ---------------- ARQUIVOS ---------------- */

/* verifica fopen e informa (stdout) caso haja problema */
void salvarDados(Livro *lv, int qtdL, Usuario *us, int qtdU, Emprestimo *em, int qtdE) {
    FILE *f;
    int i;

    f = fopen("livros.txt", "w");
    if (!f) {
        printf("Erro: nao foi possivel abrir livros.txt para escrita.\n");
    } else {
        for (i = 0; i < qtdL; i++) {
            fprintf(f, "%d;%s;%s;%s;%d;%d;%d;%d\n",
                lv[i].cod,
                lv[i].titulo,
                lv[i].autor,
                lv[i].editora,
                lv[i].ano,
                lv[i].exemplares,
                lv[i].status,
                lv[i].totalEmprestimos
            );
        }
        fclose(f);
    }

    f = fopen("usuarios.txt", "w");
    if (!f) {
        printf("Erro: nao foi possivel abrir usuarios.txt para escrita.\n");
    } else {
        for (i = 0; i < qtdU; i++) {
            fprintf(f, "%d;%s;%s;%s;%d;%d;%d\n",
                us[i].matricula,
                us[i].nome,
                us[i].curso,
                us[i].telefone,
                us[i].dia,
                us[i].mes,
                us[i].ano
            );
        }
        fclose(f);
    }

    f = fopen("emprestimos.txt", "w");
    if (!f) {
        printf("Erro: nao foi possivel abrir emprestimos.txt para escrita.\n");
    } else {
        for (i = 0; i < qtdE; i++) {
            fprintf(f, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d\n",
                em[i].codEmp,
                em[i].matricula,
                em[i].codLivro,
                em[i].diaEmp,
                em[i].mesEmp,
                em[i].anoEmp,
                em[i].diaPrev,
                em[i].mesPrev,
                em[i].anoPrev,
                em[i].ativo
            );
            fprintf(f, "R;%d\n", em[i].renovacoes);
        }
        fclose(f);
    }

    backupAutomatico();
}

/* CARREGAR DADOS ----------------------------------------*/

void carregarDados(Livro *lv, int *pQtdL, Usuario *us, int *pQtdU, Emprestimo *em, int *pQtdE) {
    FILE *f;
    char linha[512];
    int r;

    *pQtdL = *pQtdU = *pQtdE = 0;

    f = fopen("livros.txt", "r");
    if (f) {
        while (fgets(linha, sizeof(linha), f)) {
            int cod, ano, exemplares, status, totalEmp = 0;
            char titulo[100] = {0}, autor[80] = {0}, editora[60] = {0};
            r = sscanf(linha, "%d;%99[^;];%79[^;];%59[^;];%d;%d;%d;%d",
                       &cod, titulo, autor, editora, &ano, &exemplares, &status, &totalEmp);
            if (r >= 7) {
                lv[*pQtdL].cod = cod;
                strncpy(lv[*pQtdL].titulo, titulo, sizeof(lv[*pQtdL].titulo)-1);
                lv[*pQtdL].titulo[sizeof(lv[*pQtdL].titulo)-1] = '\0';
                strncpy(lv[*pQtdL].autor, autor, sizeof(lv[*pQtdL].autor)-1);
                lv[*pQtdL].autor[sizeof(lv[*pQtdL].autor)-1] = '\0';
                strncpy(lv[*pQtdL].editora, editora, sizeof(lv[*pQtdL].editora)-1);
                lv[*pQtdL].editora[sizeof(lv[*pQtdL].editora)-1] = '\0';
                lv[*pQtdL].ano = ano;
                lv[*pQtdL].exemplares = exemplares;
                lv[*pQtdL].status = status;
                lv[*pQtdL].totalEmprestimos = (r == 8) ? totalEmp : 0;
                (*pQtdL)++;
                if (*pQtdL >= MAX_LIV) break;
            }
        }
        fclose(f);
    }

    f = fopen("usuarios.txt", "r");
    if (f) {
        while (fgets(linha, sizeof(linha), f)) {
            int mat, dia, mes, ano;
            char nome[100] = {0}, curso[50] = {0}, telefone[15] = {0};
            r = sscanf(linha, "%d;%99[^;];%49[^;];%14[^;];%d;%d;%d",
                       &mat, nome, curso, telefone, &dia, &mes, &ano);
            if (r == 7) {
                us[*pQtdU].matricula = mat;
                strncpy(us[*pQtdU].nome, nome, sizeof(us[*pQtdU].nome)-1);
                us[*pQtdU].nome[sizeof(us[*pQtdU].nome)-1] = '\0';
                strncpy(us[*pQtdU].curso, curso, sizeof(us[*pQtdU].curso)-1);
                us[*pQtdU].curso[sizeof(us[*pQtdU].curso)-1] = '\0';
                strncpy(us[*pQtdU].telefone, telefone, sizeof(us[*pQtdU].telefone)-1);
                us[*pQtdU].telefone[sizeof(us[*pQtdU].telefone)-1] = '\0';
                us[*pQtdU].dia = dia;
                us[*pQtdU].mes = mes;
                us[*pQtdU].ano = ano;
                (*pQtdU)++;
                if (*pQtdU >= MAX_USU) break;
            }
        }
        fclose(f);
    }

    f = fopen("emprestimos.txt", "r");
    if (f) {
        while (fgets(linha, sizeof(linha), f)) {
            if (linha[0] == 'R') {
                int ren = 0;
                if (sscanf(linha, "R;%d", &ren) == 1 && *pQtdE > 0) {
                    em[*pQtdE - 1].renovacoes = ren;
                }
                continue;
            }
            int codEmp, mat, codLivro, dE, mE, aE, dP, mP, aP, ativo;
            r = sscanf(linha, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d",
                       &codEmp, &mat, &codLivro, &dE, &mE, &aE, &dP, &mP, &aP, &ativo);
            if (r == 10) {
                em[*pQtdE].codEmp = codEmp;
                em[*pQtdE].matricula = mat;
                em[*pQtdE].codLivro = codLivro;
                em[*pQtdE].diaEmp = dE;
                em[*pQtdE].mesEmp = mE;
                em[*pQtdE].anoEmp = aE;
                em[*pQtdE].diaPrev = dP;
                em[*pQtdE].mesPrev = mP;
                em[*pQtdE].anoPrev = aP;
                em[*pQtdE].ativo = ativo;
                em[*pQtdE].renovacoes = 0;
                (*pQtdE)++;
                if (*pQtdE >= MAX_EMP) break;
            }
        }
        fclose(f);
    }
}

/* ---------- BUSCAS / UTILIDADES DE NEGÓCIO ---------- */

int buscarLivroPorCodigo(Livro *lv, int qtdL, int cod) {
    int i;
    for (i = 0; i < qtdL; i++)
        if (lv[i].cod == cod) return i;
    return -1;
}

int buscarUsuarioPorMatricula(Usuario *us, int qtdU, int mat) {
    int i;
    for (i = 0; i < qtdU; i++)
        if (us[i].matricula == mat) return i;
    return -1;
}

void pesquisarPorTitulo(Livro *lv, int qtdL, const char *termo) {
    int i;
    if (!termo || termo[0]==0) return;
    for (i = 0; i < qtdL; i++) {
        if (my_strcasestr(lv[i].titulo, termo)) {
            printf("[C:%d] %s — %s (%d) Ex.: %d Status: %s\n",
                lv[i].cod, lv[i].titulo, lv[i].autor, lv[i].ano, lv[i].exemplares,
                lv[i].status ? "Disponivel" : "Indisponivel");
        }
    }
}

void pesquisarPorAutor(Livro *lv, int qtdL, const char *termo) {
    int i;
    if (!termo || termo[0]==0) return;
    for (i = 0; i < qtdL; i++) {
        if (my_strcasestr(lv[i].autor, termo)) {
            printf("[C:%d] %s — %s (%d) Ex.: %d Status: %s\n",
                lv[i].cod, lv[i].titulo, lv[i].autor, lv[i].ano, lv[i].exemplares,
                lv[i].status ? "Disponivel" : "Indisponivel");
        }
    }
}

void pesquisarUsuarioPorNome(Usuario *us, int qtdU, const char *nome) {
    int i;
    if (!nome || nome[0]==0) return;
    for (i = 0; i < qtdU; i++) {
        if (my_strcasestr(us[i].nome, nome)) {
            printf("[M:%d] %s — %s Tel:%s\n", us[i].matricula, us[i].nome, us[i].curso, us[i].telefone);
        }
    }
}

void listarEmprestimosAtivos(Emprestimo *em, int qtdE, Usuario *us, int qtdU, Livro *lv, int qtdL) {
    int i;
    printf("== Empréstimos Ativos ==\n");
    for (i = 0; i < qtdE; i++) {
        if (em[i].ativo == 1) {
            int posU = buscarUsuarioPorMatricula(us, qtdU, em[i].matricula);
            int posL = buscarLivroPorCodigo(lv, qtdL, em[i].codLivro);
            printf("CodEmp: %d | Usuario: %s | Livro: %s | Data: %02d/%02d/%04d | Prev: %02d/%02d/%04d | Renov: %d\n",
                   em[i].codEmp,
                   (posU >= 0) ? us[posU].nome : "Usuario nao encontrado",
                   (posL >= 0) ? lv[posL].titulo : "Livro nao encontrado",
                   em[i].diaEmp, em[i].mesEmp, em[i].anoEmp,
                   em[i].diaPrev, em[i].mesPrev, em[i].anoPrev,
                   em[i].renovacoes);
        }
    }
}

void relatorioLivrosMaisEmprestados(Livro *lv, int qtdL, int topN) {
    if (qtdL <= 0) { printf("Nenhum livro cadastrado.\n"); return; }
    Livro temp[MAX_LIV];
    int n = (qtdL < MAX_LIV) ? qtdL : MAX_LIV;
    memcpy(temp, lv, sizeof(Livro) * n);
    int i, j;
    for (i = 0; i < n - 1; i++)
        for (j = i + 1; j < n; j++)
            if (temp[j].totalEmprestimos > temp[i].totalEmprestimos) {
                Livro t = temp[i]; temp[i] = temp[j]; temp[j] = t;
            }
    printf("== Top %d Livros Mais Emprestados ==\n", topN);
    for (i = 0; i < n && i < topN; i++) {
        if (temp[i].totalEmprestimos > 0)
            printf("%d) [%d] %s — %s (emprestimos: %d)\n",
                   i+1, temp[i].cod, temp[i].titulo, temp[i].autor, temp[i].totalEmprestimos);
    }
}

void relatorioUsuariosAtraso(Emprestimo *em, int qtdE, Usuario *us, int qtdU) {
    int i;
    int achou = 0;
    printf("== Usuarios com emprestimos em atraso ==\n");
    for (i = 0; i < qtdE; i++) {
        if (em[i].ativo == 1) {
            int dias = diasEntre(em[i].diaPrev, em[i].mesPrev, em[i].anoPrev);
            if (dias > 0) {
                int pos = buscarUsuarioPorMatricula(us, qtdU, em[i].matricula);
                if (pos >= 0) {
                    printf("Mat:%d Nome:%s — atraso de %d dias (CodEmp:%d)\n",
                           us[pos].matricula, us[pos].nome, dias, em[i].codEmp);
                    achou = 1;
                }
            }
        }
    }
    if (!achou) printf("Nenhum usuario com emprestimos em atraso.\n");
}

void buscaAvancada(Livro *lv, int qtdL) {
    char input[200];
    char termoTitulo[100] = "";
    char termoAutor[80] = "";
    int anoMin = 0, anoMax = 0;
    int statusFiltro = -1;

    printf("Busca avancada — deixe campo vazio para ignorar.\n");

    printf("Titulo (substring): ");
    if (fgets(input, sizeof(input), stdin)) {
        input[strcspn(input, "\n")] = 0;
        if (input[0] != 0) strcpy(termoTitulo, input);
    }

    printf("Autor (substring): ");
    if (fgets(input, sizeof(input), stdin)) {
        input[strcspn(input, "\n")] = 0;
        if (input[0] != 0) strcpy(termoAutor, input);
    }

    printf("Ano minimo (0=ignorar): ");
    if (!lerInt("", &anoMin)) anoMin = 0;
    flush_stdin();
    printf("Ano maximo (0=ignorar): ");
    if (!lerInt("", &anoMax)) anoMax = 0;
    flush_stdin();

    printf("Status (1=disponivel,0=indisponivel,-1=qualquer) : ");
    if (!lerInt("", &statusFiltro)) statusFiltro = -1;
    flush_stdin();

    int i;
    printf("== Resultados da busca avancada ==\n");
    for (i = 0; i < qtdL; i++) {
        int ok = 1;
        if (termoTitulo[0] && !my_strcasestr(lv[i].titulo, termoTitulo)) ok = 0;
        if (termoAutor[0] && !my_strcasestr(lv[i].autor, termoAutor)) ok = 0;
        if (anoMin > 0 && lv[i].ano < anoMin) ok = 0;
        if (anoMax > 0 && lv[i].ano > anoMax) ok = 0;
        if (statusFiltro != -1 && lv[i].status != statusFiltro) ok = 0;
        if (ok) {
            printf("[%d] %s — %s (%d) Ex.:%d Status:%s\n",
                   lv[i].cod, lv[i].titulo, lv[i].autor, lv[i].ano, lv[i].exemplares,
                   lv[i].status ? "Disponivel" : "Indisponivel");
        }
    }
}

/* CADASTROS E AÇÕES ----------------------------------------------------- */

void cadastrarLivro(Livro *lv, int *pQtdL) {
    if (*pQtdL >= MAX_LIV) {
        printf("Limite de livros atingido!\n");
        return;
    }
    Livro novo;
    printf("Codigo (inteiro): ");
    if (!lerInt("", &novo.cod)) { printf("Codigo invalido.\n"); flush_stdin(); return; }
    flush_stdin();

    if (buscarLivroPorCodigo(lv, *pQtdL, novo.cod) >= 0) {
        printf("Codigo ja cadastrado.\n");
        return;
    }

    printf("Titulo: ");
    fgets(novo.titulo, sizeof(novo.titulo), stdin);
    novo.titulo[strcspn(novo.titulo, "\n")] = 0;

    printf("Autor: ");
    fgets(novo.autor, sizeof(novo.autor), stdin);
    novo.autor[strcspn(novo.autor, "\n")] = 0;

    printf("Editora: ");
    fgets(novo.editora, sizeof(novo.editora), stdin);
    novo.editora[strcspn(novo.editora, "\n")] = 0;

    printf("Ano publicacao: ");
    if (!lerInt("", &novo.ano)) { printf("Ano invalido.\n"); flush_stdin(); return; }
    flush_stdin();

    printf("Numero de exemplares: ");
    if (!lerInt("", &novo.exemplares)) { printf("Valor invalido.\n"); flush_stdin(); return; }
    flush_stdin();

    novo.status = (novo.exemplares > 0) ? 1 : 0;
    novo.totalEmprestimos = 0;

    lv[*pQtdL] = novo;
    (*pQtdL)++;
    printf("Livro cadastrado com sucesso.\n");
}

/* CADASTRAR USUARIO ---------------------------------------------------*/
void cadastrarUsuario(Usuario *us, int *pQtdU) {
    if (*pQtdU >= MAX_USU) {
        printf("Limite de usuarios atingido!\n");
        return;
    }
    Usuario novo;
    int i, maxMat = 999;
    for (i = 0; i < *pQtdU; i++) if (us[i].matricula > maxMat) maxMat = us[i].matricula;
    /* garante que a primeira matricula seja 1000 */
    novo.matricula = maxMat + 1;
    printf("Matricula gerada: %d\n", novo.matricula);
    flush_stdin();

    printf("Nome completo: ");
    fgets(novo.nome, sizeof(novo.nome), stdin);
    novo.nome[strcspn(novo.nome, "\n")] = 0;

    printf("Curso: ");
    fgets(novo.curso, sizeof(novo.curso), stdin);
    novo.curso[strcspn(novo.curso, "\n")] = 0;

    printf("Telefone: ");
    fgets(novo.telefone, sizeof(novo.telefone), stdin);
    novo.telefone[strcspn(novo.telefone, "\n")] = 0;

    printf("Dia do cadastro: ");
    if (!lerInt("", &novo.dia)) { printf("Dia invalido.\n"); flush_stdin(); return; }
    flush_stdin();
    printf("Mes do cadastro: ");
    if (!lerInt("", &novo.mes)) { printf("Mes invalido.\n"); flush_stdin(); return; }
    flush_stdin();
    printf("Ano do cadastro: ");
    if (!lerInt("", &novo.ano)) { printf("Ano invalido.\n"); flush_stdin(); return; }
    flush_stdin();

    us[*pQtdU] = novo;
    (*pQtdU)++;
    printf("Usuario cadastrado com sucesso.\n");
}

/* EMPRESTAR LIVRO-----------------------------------------------------*/

void emprestarLivro(Livro *lv, int *pQtdL, Usuario *us, int qtdU, Emprestimo *em, int *pQtdE) {
    int mat;
    if (!lerInt("Matricula: ", &mat)) { printf("Entrada invalida.\n"); flush_stdin(); return; }
    flush_stdin();

    int posU = buscarUsuarioPorMatricula(us, qtdU, mat);
    if (posU < 0) { printf("Usuario nao encontrado.\n"); return; }

    int cod;
    if (!lerInt("Codigo do livro: ", &cod)) { printf("Entrada invalida.\n"); flush_stdin(); return; }
    flush_stdin();

    int posL = buscarLivroPorCodigo(lv, *pQtdL, cod);
    if (posL < 0) { printf("Livro nao encontrado.\n"); return; }

    if (lv[posL].exemplares <= 0) { printf("Sem exemplares disponiveis.\n"); return; }

    if (*pQtdE >= MAX_EMP) { printf("Limite de emprestimos atingido.\n"); return; }

    Emprestimo novo;
    novo.codEmp = 1000 + (*pQtdE);
    novo.matricula = mat;
    novo.codLivro = cod;

    time_t t = time(NULL);
    struct tm data = *localtime(&t);
    novo.diaEmp = data.tm_mday;
    novo.mesEmp = data.tm_mon + 1;
    novo.anoEmp = data.tm_year + 1900;

    adicionarDias(novo.diaEmp, novo.mesEmp, novo.anoEmp, 7, &novo.diaPrev, &novo.mesPrev, &novo.anoPrev);

    novo.ativo = 1;
    novo.renovacoes = 0;

    em[*pQtdE] = novo;
    (*pQtdE)++;

    lv[posL].exemplares--;
    lv[posL].status = (lv[posL].exemplares > 0) ? 1 : 0;
    lv[posL].totalEmprestimos++;

    printf("Emprestimo realizado. Codigo do emprestimo: %d\n", novo.codEmp);
}

/* DEVOLVER LIVRO ---------------------------------------------------*/

void devolverLivro(Livro *lv, int qtdL, Emprestimo *em, int qtdE) {
    int codEmp;
    if (!lerInt("Codigo do emprestimo: ", &codEmp)) { printf("Entrada invalida.\n"); flush_stdin(); return; }
    flush_stdin();

    int i;
    for (i = 0; i < qtdE; i++) {
        if (em[i].codEmp == codEmp && em[i].ativo == 1) {
            int posL = buscarLivroPorCodigo(lv, qtdL, em[i].codLivro);
            if (posL >= 0) {
                lv[posL].exemplares++;
                lv[posL].status = 1;
            }
            em[i].ativo = 0;
            printf("Devolucao efetuada.\n");
            return;
        }
    }
    printf("Emprestimo nao encontrado ou ja devolvido.\n");
}

/* RENOVAR EMPRESTIMO ---------------------------------------------------*/

void renovarEmprestimo(Emprestimo *em, int qtdE, Livro *lv, int qtdL, Usuario *us, int qtdU) {
    int codEmp;
    if (!lerInt("Codigo do emprestimo para renovacao: ", &codEmp)) { printf("Entrada invalida.\n"); flush_stdin(); return; }
    flush_stdin();

    int i;
    for (i = 0; i < qtdE; i++) {
        if (em[i].codEmp == codEmp && em[i].ativo == 1) {
            int diasAtraso = diasEntre(em[i].diaPrev, em[i].mesPrev, em[i].anoPrev);
            if (diasAtraso > 0) {
                printf("Emprestimo em atraso (%d dias). Renovacao nao permitida.\n", diasAtraso);
                return;
            }
            if (em[i].renovacoes >= 2) {
                printf("Limite de renovacoes atingido (2 vezes).\n");
                return;
            }
            int nd, nm, na;
            adicionarDias(em[i].diaPrev, em[i].mesPrev, em[i].anoPrev, 7, &nd, &nm, &na);
            em[i].diaPrev = nd; em[i].mesPrev = nm; em[i].anoPrev = na;
            em[i].renovacoes++;
            printf("Renovacao realizada. Nova data prevista: %02d/%02d/%04d (renovacoes: %d)\n",
                   em[i].diaPrev, em[i].mesPrev, em[i].anoPrev, em[i].renovacoes);
            return;
        }
    }
    printf("Emprestimo nao encontrado.\n");
}

/*  MENUS ---------------------------------------------------------------- */

void submenuPesquisa(Livro *lv, int qtdL, Usuario *us, int qtdU) {
    int op;
    char termo[200];
    do {
        printf("\n--- Submenu: Pesquisas ---\n");
        printf("1 - Buscar livro por codigo\n");
        printf("2 - Buscar por titulo (substring)\n");
        printf("3 - Buscar por autor (substring)\n");
        printf("4 - Buscar usuario por matricula\n");
        printf("5 - Buscar usuario por nome (substring)\n");
        printf("6 - Busca avancada (multiples criterios)\n");
        printf("0 - Voltar\n");
        printf("Opcao: ");
        if (!lerInt("", &op)) { printf("Entrada invalida.\n"); flush_stdin(); continue; }
        flush_stdin();

        if (op == 1) {
            int cod;
            if (!lerInt("Codigo do livro: ", &cod)) { printf("Entrada invalida.\n"); flush_stdin(); continue; }
            flush_stdin();
            int pos = buscarLivroPorCodigo(lv, qtdL, cod);
            if (pos >= 0) {
                printf("=> [%d] %s — %s (%d) Ex.: %d Status:%s\n",
                       lv[pos].cod, lv[pos].titulo, lv[pos].autor, lv[pos].ano, lv[pos].exemplares,
                       lv[pos].status ? "Disponivel" : "Indisponivel");
            } else printf("Livro nao encontrado.\n");
        } else if (op == 2) {
            printf("Termo titulo: ");
            if (fgets(termo, sizeof(termo), stdin)) {
                termo[strcspn(termo, "\n")] = 0;
                pesquisarPorTitulo(lv, qtdL, termo);
            }
        } else if (op == 3) {
            printf("Termo autor: ");
            if (fgets(termo, sizeof(termo), stdin)) {
                termo[strcspn(termo, "\n")] = 0;
                pesquisarPorAutor(lv, qtdL, termo);
            }
        } else if (op == 4) {
            int mat;
            if (!lerInt("Matricula: ", &mat)) { printf("Entrada invalida.\n"); flush_stdin(); continue; }
            flush_stdin();
            int pos = buscarUsuarioPorMatricula(us, qtdU, mat);
            if (pos >= 0) {
                printf("=> Mat:%d Nome:%s Curso:%s Tel:%s\n", us[pos].matricula, us[pos].nome, us[pos].curso, us[pos].telefone);
            } else printf("Usuario nao encontrado.\n");
        } else if (op == 5) {
            printf("Termo nome: ");
            if (fgets(termo, sizeof(termo), stdin)) {
                termo[strcspn(termo, "\n")] = 0;
                pesquisarUsuarioPorNome(us, qtdU, termo);
            }
        } else if (op == 6) {
            buscaAvancada(lv, qtdL);
        } else if (op == 0) {
            break;
        } else {
            printf("Opcao invalida.\n");
        }

    } while (1);
}


void submenuRelatorios(Livro *lv, int qtdL, Emprestimo *em, int qtdE, Usuario *us, int qtdU) {
    int op;
    do {
        printf("\n--- Submenu: Relatorios ---\n");
        printf("1 - Livros mais emprestados (top 10)\n");
        printf("2 - Usuarios com emprestimos em atraso\n");
        printf("0 - Voltar\n");
        printf("Opcao: ");
        if (!lerInt("", &op)) { printf("Entrada invalida.\n"); flush_stdin(); continue; }
        flush_stdin();

        if (op == 1) relatorioLivrosMaisEmprestados(lv, qtdL, 10);
        else if (op == 2) relatorioUsuariosAtraso(em, qtdE, us, qtdU);
        else if (op == 0) break;
        else printf("Opcao invalida.\n");

    } while (1);
}



void menuPrincipal() {
    int op;
    do {
        printf("\n===== BIBLIOTECA - MENU PRINCIPAL =====\n");
        printf("1 - Cadastrar livro\n");
        printf("2 - Cadastrar usuario\n");
        printf("3 - Emprestimo\n");
        printf("4 - Devolucao\n");
        printf("5 - Renovacao de emprestimo\n");
        printf("6 - Pesquisas (submenu)\n");
        printf("7 - Listar emprestimos ativos\n");
        printf("8 - Relatorios (submenu)\n");
        printf("9 - Salvar dados\n");
        printf("0 - Sair\n");
        printf("Opcao: ");
        if (!lerInt("", &op)) { printf("Entrada invalida.\n"); continue; }
        flush_stdin();

        switch (op) {
            case 1: cadastrarLivro(livros, &qtdLiv); break;
            case 2: cadastrarUsuario(usuarios, &qtdUsu); break;
            case 3: emprestarLivro(livros, &qtdLiv, usuarios, qtdUsu, emprestimos, &qtdEmp); break;
            case 4: devolverLivro(livros, qtdLiv, emprestimos, qtdEmp); break;
            case 5: renovarEmprestimo(emprestimos, qtdEmp, livros, qtdLiv, usuarios, qtdUsu); break;
            case 6: submenuPesquisa(livros, qtdLiv, usuarios, qtdUsu); break;
            case 7: listarEmprestimosAtivos(emprestimos, qtdEmp, usuarios, qtdUsu, livros, qtdLiv); break;
            case 8: submenuRelatorios(livros, qtdLiv, emprestimos, qtdEmp, usuarios, qtdUsu); break;
            case 9: salvarDados(livros, qtdLiv, usuarios, qtdUsu, emprestimos, qtdEmp);
                    printf("Dados salvos e backup criado.\n");
                    break;
            case 0: printf("Saindo...\n"); return;
            default: printf("Opcao invalida.\n");
        }

    } while (1);
}

/* ----------- INICIALIZADORES E MAIN ----------- */

void inicializarSistema() {
    qtdLiv = qtdUsu = qtdEmp = 0;
    memset(livros, 0, sizeof(livros));
    memset(usuarios, 0, sizeof(usuarios));
    memset(emprestimos, 0, sizeof(emprestimos));
}

int main() {
    inicializarSistema();
    carregarDados(livros, &qtdLiv, usuarios, &qtdUsu, emprestimos, &qtdEmp);
    printf("Dados carregados: %d livros, %d usuarios, %d emprestimos\n", qtdLiv, qtdUsu, qtdEmp);
    menuPrincipal();
    salvarDados(livros, qtdLiv, usuarios, qtdUsu, emprestimos, qtdEmp);
    return 0;
}

