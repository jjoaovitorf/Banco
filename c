#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int numero;
    int tipo;
    double saldo;
    double historico[50];
    int qtdHistorico;
} Conta;

typedef struct {
    char nome[50];
    Conta contas[10];
    int qtdContas;
    int emprestimo;
} Cliente;

Cliente clientes[100];
int qtdClientes = 0;
int proximoNumero = 1000;
int bancoAberto = 0;
int ganhador = -1;
double premio = 0;

int fibonacci(int num) {
    if (num == 0)
        return 0;
    if (num == 1)
        return 1;
    return fibonacci(num - 1) + fibonacci(num - 2);
}

int abrirConta(int c, int tipo) {
    for (int i = 0; i < clientes[c].qtdContas; i++) {
        if ((tipo == 1 && clientes[c].contas[i].tipo == 2) ||
            (tipo == 2 && clientes[c].contas[i].tipo == 1)) {
            printf("Nao pode ter conta corrente e salario juntas.\n");
            return 0;
        }
    }
    int pos = clientes[c].qtdContas;
    clientes[c].contas[pos].numero = proximoNumero;
    clientes[c].contas[pos].tipo = tipo;
    clientes[c].qtdContas++;
    proximoNumero++;
    printf("Conta %d-%02d aberta!\n", clientes[c].contas[pos].numero, tipo);
    return 1;
}

void abrirBanco() {
    if (bancoAberto == 1) {
        printf("O banco ja esta aberto.\n");
        return;
    }

    printf("Quantos clientes? ");
    scanf("%d", &qtdClientes);

    for (int i = 0; i < qtdClientes; i++) {
        printf("\nNome do cliente %d: ", i + 1);
        scanf("%s", clientes[i].nome);

        int quantas;
        printf("Quantas contas? ");
        scanf("%d", &quantas);

        int abertas = 0;
        while (abertas < quantas) {
            int tipo;
            printf("Tipo da conta %d (1 corrente, 2 salario, 3 poupanca): ", abertas + 1);
            scanf("%d", &tipo);

            if (tipo < 1 || tipo > 3)
                printf("Tipo invalido.\n");
            else if (abrirConta(i, tipo) == 1)
                abertas++;
        }
    }

    bancoAberto = 1;
    printf("\nBanco aberto!\n");
}

int buscar(int c, int tipo) {
    for (int i = 0; i < clientes[c].qtdContas; i++) {
        if (clientes[c].contas[i].tipo == tipo)
            return i;
    }
    return -1;
}

double total(int c) {
    double soma = 0;
    for (int i = 0; i < clientes[c].qtdContas; i++) {
        soma = soma + clientes[c].contas[i].saldo;
    }
    return soma;
}

void guardar(int c, int k, double valor) {
    clientes[c].contas[k].saldo += valor;
    clientes[c].contas[k].historico[clientes[c].contas[k].qtdHistorico] = valor;
    clientes[c].contas[k].qtdHistorico++;
}

int escolherCliente() {
    int c;
    for (int i = 0; i < qtdClientes; i++) {
        printf("%d - %s\n", i + 1, clientes[i].nome);
    }
    printf("Qual cliente? ");
    scanf("%d", &c);
    return c - 1;
}

int podeEmprestimo(int c) {
    if (clientes[c].emprestimo == 1)
        return 0;
    int corrente = buscar(c, 1);
    int poupanca = buscar(c, 3);
    if (corrente == -1 || poupanca == -1)
        return 0;
    if (clientes[c].contas[poupanca].saldo <= 0)
        return 0;
    return 1;
}

int podeTitulo(int c) {
    int salario = buscar(c, 2);
    if (salario == -1)
        return 0;
    if (clientes[c].contas[salario].saldo >= 1000 && clientes[c].contas[salario].saldo <= 1500)
        return 1;
    return 0;
}

int posicao(int c) {
    int qtd = 0;
    for (int i = 0; i < qtdClientes; i++) {
        if (total(i) > total(c))
            qtd++;
    }
    return qtd;
}

void alerta() {
    for (int i = 0; i < qtdClientes; i++) {
        int corrente = buscar(i, 1);
        if (corrente != -1 && clientes[i].contas[corrente].saldo < 0)
            printf("ALERTA: %s esta com saldo negativo na conta corrente!\n", clientes[i].nome);
    }
}

void historico() {
    double soma = 0;
    for (int i = 0; i < qtdClientes; i++) {
        soma = soma + total(i);
    }
    printf("\n== Saldo Total do banco ==\n");
    printf("%d | R$ %.2f\n", qtdClientes, soma);

    printf("\n== Clientes Aptos a Emprestimos ==\n");
    for (int i = 0; i < qtdClientes; i++) {
        if (podeEmprestimo(i) == 1) {
            int poupanca = buscar(i, 3);
            printf("%s|%d-03|%.2f\n", clientes[i].nome, clientes[i].contas[poupanca].numero, 0.25 * 10 * total(i));
        }
    }

    printf("\n== Clientes com Saldo Negativo na Conta Salario ==\n");
    for (int i = 0; i < qtdClientes; i++) {
        int salario = buscar(i, 2);
        if (salario != -1 && clientes[i].contas[salario].saldo < 0)
            printf("%s|%d-02|%.2f\n", clientes[i].nome, clientes[i].contas[salario].numero, clientes[i].contas[salario].saldo);
    }

    printf("\n== Clientes Aptos a Compra de Titulos ==\n");
    for (int i = 0; i < qtdClientes; i++) {
        if (podeTitulo(i) == 1) {
            int salario = buscar(i, 2);
            printf("%s|%d-02|%.2f\n", clientes[i].nome, clientes[i].contas[salario].numero, clientes[i].contas[salario].saldo);
        }
    }

    printf("\n== Clientes Contemplado no Sorteio ==\n");
    if (ganhador != -1) {
        int salario = buscar(ganhador, 2);
        printf("%s|%d-02|%.2f\n", clientes[ganhador].nome, clientes[ganhador].contas[salario].numero, premio);
    }
}

void sorteio() {
    if (qtdClientes < 5) {
        printf("Precisa de pelo menos 5 clientes.\n");
        return;
    }

    int pesoTotal = 0;
    for (int i = 0; i < qtdClientes; i++) {
        if (posicao(i) < 5) {
            if (podeTitulo(i) == 0) {
                printf("%s nao tem titulo. Sorteio cancelado.\n", clientes[i].nome);
                return;
            }
            pesoTotal = pesoTotal + fibonacci(5 - posicao(i));
        }
    }

    int numero = rand() % pesoTotal;
    int soma = 0;
    for (int i = 0; i < qtdClientes; i++) {
        if (posicao(i) < 5) {
            soma = soma + fibonacci(5 - posicao(i));
            if (numero < soma) {
                ganhador = i;
                break;
            }
        }
    }

    int salario = buscar(ganhador, 2);
    premio = 10 * clientes[ganhador].contas[salario].saldo;
    guardar(ganhador, salario, premio);
    printf("Vencedor: %s | Premio: R$ %.2f\n", clientes[ganhador].nome, premio);
}

void adicionarSaldo() {
    int c = escolherCliente();
    int k;
    double valor;

    for (int i = 0; i < clientes[c].qtdContas; i++) {
        printf("%d - %d-%02d\n", i + 1, clientes[c].contas[i].numero, clientes[c].contas[i].tipo);
    }
    printf("Qual conta? ");
    scanf("%d", &k);
    k--;

    printf("Valor: ");
    scanf("%lf", &valor);
    guardar(c, k, valor);
}

void pedirEmprestimo() {
    int c = escolherCliente();
    double valor;

    if (podeEmprestimo(c) == 0) {
        printf("Cliente nao esta apto a emprestimo.\n");
        return;
    }

    double limite = 0.25 * 10 * total(c);
    printf("Limite: R$ %.2f\n", limite);
    printf("Valor: ");
    scanf("%lf", &valor);

    if (valor > limite) {
        printf("Valor acima do limite.\n");
        return;
    }

    guardar(c, buscar(c, 1), valor);
    clientes[c].emprestimo = 1;
    printf("Emprestimo concedido!\n");
}

int main() {
    srand(time(NULL));
    int opcao;
    do {
        printf("\n===== MENU =====\n");
        alerta();
        printf("1 - Abrir o banco\n");
        printf("2 - Consultar historico\n");
        printf("3 - Efetuar sorteio\n");
        printf("4 - Adicionar saldo\n");
        printf("5 - Pedir emprestimo\n");
        printf("6 - Sair\n");
        printf("Opcao: ");
        scanf("%d", &opcao);

        if (opcao == 1)
            abrirBanco();
        else if ((opcao >= 2 && opcao <= 5) && bancoAberto == 0)
            printf("Abra o banco primeiro.\n");
        else if (opcao == 2)
            historico();
        else if (opcao == 3)
            sorteio();
        else if (opcao == 4)
            adicionarSaldo();
        else if (opcao == 5)
            pedirEmprestimo();
    } while (opcao != 6);

    return 0;
}
