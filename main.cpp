#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring> // Para strncpy
#include <locale.h>
#include <iomanip>

#define TAMANHO_NOME 48
#define MAX_ITENS 100

using namespace std;

const char* arquivo = "vendas.txt";
const char* vendasTotais = "relatorio_vendasTotais.txt";
const char* vendasLista = "relatorio_vendasLista.txt";
const char* intervaloVendas = "relatorio_intervaloDeVendas.txt";

struct InfoVenda {
    int numeroVenda;
    char data[11];
    int codigoCliente;
    int qntId;
    float totalVenda;
};

struct InfoItens {
    int id;
    char nome[TAMANHO_NOME+1];
    float preco;
    int qnt;
    float subtotal; // pre�o unit�rio x quantidade
};

struct DadosRelatorio {
    int quantidadeTotal = 0;
    float receitaTotal = 0;
    string maisVendido = " ";
    int maisUnidades = 0;
    string menosVendido = " ";
    int menosUnidades = INT_MAX;
};

struct DataInteira {
    int dia;
    int mes;
    int ano;
};

//Fun��es - prot�tipos:
void contLinhas(const char* arquivo, int& linhas);
void lerDadosVendas(const char* arquivo, int linhas, string vendasCadastradas[]);
void converteDados(string vendasCadastradas[], int linhas, InfoVenda &vendas, InfoItens itens[],int &numItens, DadosRelatorio &dado);
void carregarDadosRelatorio(DadosRelatorio &dado);
void vendasTotaisRel(DadosRelatorio &dado, InfoVenda &vendas, InfoItens itens[],int numItens);
void vendasListaRel(InfoVenda &vendas, InfoItens itens[], int numItens);
void limpaArquivo(char reseta);


int main()
{
    setlocale(LC_ALL,"portuguese");
    int linhas = 0;

    DadosRelatorio dado;
    // Carrega dados existentes antes de processar novas vendas
    carregarDadosRelatorio(dado);

    contLinhas(arquivo,linhas);

    string* vendasCadastradas  = new string[linhas];
    lerDadosVendas(arquivo,linhas,vendasCadastradas);

    InfoItens itens[linhas-5];
    InfoVenda vendas;
    int numItens = 0;

    converteDados(vendasCadastradas,linhas,vendas,itens,numItens, dado);
    vendasTotaisRel(dado,vendas,itens,numItens);
    vendasListaRel(vendas,itens,numItens);

    delete[] vendasCadastradas;
    return 0;
}

void contLinhas(const char* arquivo, int& linhas) {
    ifstream leitura(arquivo);
    if(!leitura.is_open()) {
        cerr << "Erro ao tentar abrir o arquivo!" << endl;
        return;
    }
    string s;
    while(getline(leitura,s)) {
        linhas++;
    }
    leitura.close();
}

void lerDadosVendas(const char* arquivo, int linhas, string vendasCadastradas[]) {
    ifstream leitura(arquivo);
    if(!leitura.is_open()) {
        cerr << "Erro ao tentar abrir o arquivo!" << endl;
        return;
    }
    string s;
    for(int i = 0; i < linhas; i++) {
        if(getline(leitura, s)) {
            vendasCadastradas[i] = s;
        }
    }
    leitura.close();
}

void converteDados(string vendasCadastradas[], int linhas, InfoVenda &vendas, InfoItens itens[], int &numItens,DadosRelatorio &dado) {
    vendas.numeroVenda = stoi(vendasCadastradas[0]);
    strncpy(vendas.data, vendasCadastradas[1].c_str(), 10);
    vendas.data[10] = '\0';
    vendas.codigoCliente = stoi(vendasCadastradas[2]);
    vendas.qntId = stoi(vendasCadastradas[3]);
    vendas.totalVenda = stof(vendasCadastradas[4]);

    int numVendas = (linhas - 5) / 5; // Ignora as primeiras 5 linhas e calcula o n�mero de vendas restantes
    numItens = 0; // Inicializa o contador de itens

    for (int i = 0; i < numVendas; i++) {
        int baseIndice = 5 + i * 5; // �ndice base para cada venda, come�ando ap�s as 5 primeiras linhas

        // Processa os itens da venda atual
        itens[numItens].id = stoi(vendasCadastradas[baseIndice]);
        strncpy(itens[numItens].nome, vendasCadastradas[baseIndice + 1].c_str(), TAMANHO_NOME);
        itens[numItens].nome[TAMANHO_NOME] = '\0'; // Garante que a string esteja terminada corretamente
        itens[numItens].preco = stof(vendasCadastradas[baseIndice + 2]);
        itens[numItens].qnt = stoi(vendasCadastradas[baseIndice + 3]);
        itens[numItens].subtotal = stof(vendasCadastradas[baseIndice + 4]);

        dado.quantidadeTotal+=itens[numItens].qnt;
        dado.receitaTotal+=itens[numItens].subtotal;

        if (itens[numItens].qnt > dado.maisUnidades) {
            dado.maisUnidades = itens[numItens].qnt;
            dado.maisVendido = itens[numItens].nome;
        }
        if (itens[numItens].qnt < dado.menosUnidades) {
            dado.menosUnidades = itens[numItens].qnt;
            dado.menosVendido = itens[numItens].nome;
        }
        numItens++; // Incrementa o contador de itens
    }
}

void carregarDadosRelatorio(DadosRelatorio &dado) {
    ifstream leitura(vendasTotais);
    if (!leitura.is_open()) {
        // Se o arquivo n�o existir, mantemos os valores padr�o
        return;
    }

    string linha;
    if (getline(leitura, linha)) dado.quantidadeTotal = stoi(linha);
    if (getline(leitura, linha)) dado.receitaTotal = stof(linha);
    // A m�dia n�o precisa ser carregada, pois � calculada
    if (getline(leitura, linha)) {} // Pula a linha da m�dia
    if (getline(leitura, linha)) dado.maisVendido = linha;
    if (getline(leitura, linha)) dado.maisUnidades = stoi(linha);
    if (getline(leitura, linha)) dado.menosVendido = linha;
    if (getline(leitura, linha)) dado.menosUnidades = stoi(linha);

    leitura.close();
}

void vendasTotaisRel(DadosRelatorio &dado, InfoVenda &vendas, InfoItens itens[],int numItens) {
    ofstream saida(vendasTotais, ios::out | ios::trunc);
    if (!saida.is_open()) {
        cerr << "Erro ao abrir arquivo." << endl;
        return;
    }
    saida << fixed << setprecision(2);
    saida << dado.quantidadeTotal << endl // Quantidade (em unidades) de produtos vendidos
          << dado.receitaTotal << endl // Receita total gerada na venda
          << dado.receitaTotal/dado.quantidadeTotal << endl // M�dia de renda por unidade vendida
          << dado.maisVendido << endl // Item mais vendido (nome)
          << dado.maisUnidades << endl // Item mais vendido (quantidade)
          << dado.menosVendido << endl // Item menos vendido (nome)
          << dado.menosUnidades << endl; // Item menos vendido (quantidade)
    saida.close();
}

void vendasListaRel(InfoVenda &vendas, InfoItens itens[], int numItens) {
    ofstream saida(vendasLista, ios::out | ios::app);
    if(!saida.is_open()) {
        cerr << "Erro ao abrir arquivo." << endl;
        return;  // Adicionei return para sair se o arquivo n�o abrir
    }
    saida << fixed << setprecision(2);  // Para consist�ncia com outras sa�das
    for (int i = 0; i < numItens; i++) {
        saida << vendas.data << endl
              << itens[i].id << endl
              << itens[i].nome << endl
              << itens[i].preco << endl
              << itens[i].qnt << endl
              << itens[i].subtotal << endl;
    }
    saida.close();
}

void limpaArquivo(char reseta) {
    ofstream saida;

    switch (reseta) {
        case '1':
            saida.open("relatorio_vendasTotais.txt", ios::trunc); // Limpa vendasTotais.txt
            saida.close();
            break;
        case '2':
            saida.open("relatorio_vendasLista.txt", ios::trunc);  // Limpa vendasLista.txt
            saida.close();
            break;
        default:
            cout << "Op��o inv�lida!" << endl;
            break;
    }
}
