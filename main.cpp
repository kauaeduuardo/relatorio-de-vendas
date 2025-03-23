#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring> // Para strncpy
#include <locale.h>
#include <iomanip>

#define TAMANHO_NOME 48

using namespace std;

const char* arquivo = "vendas.txt";

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
    float subtotal; // preço unitário x quantidade
};

//Funções - protótipos:

void contLinhas(const char* arquivo, int& linhas);
void lerDadosVendas(const char* arquivo, int linhas, string vendasCadastradas[]);
void converteDados(string vendasCadastradas[], int linhas, InfoVenda &vendas, InfoItens itens[],int &numItens);

int main()
{
    setlocale(LC_ALL,"portuguese");
    int linhas = 0;

    contLinhas(arquivo,linhas);

    string* vendasCadastradas  = new string[linhas];

    lerDadosVendas(arquivo,linhas,vendasCadastradas);

    /*for(int i = 0; i < linhas; i++) {
        cout << vendasCadastradas[i] << endl;
    }*/

    InfoItens itens[linhas-5];
    InfoVenda vendas;
    int numItens = 0;
    converteDados(vendasCadastradas,linhas,vendas,itens,numItens);
    cout << "Número da Venda: " << vendas.numeroVenda << endl;
    cout << "Data: " << vendas.data << endl;
    cout << "Código do Cliente: " << vendas.codigoCliente << endl;
    cout << "Quantidade de IDS: " << vendas.qntId << endl;
    cout << "Total da Venda: " << fixed << setprecision(2) << vendas.totalVenda << endl;
    cout << endl;
    for (int i = 0; i < numItens; i++) {
        cout << "Item " << i + 1 << ":" << endl;
        cout << "  ID: " << itens[i].id << endl;
        cout << "  Nome: " << itens[i].nome << endl;
        cout << "  Preço: " << itens[i].preco << endl;
        cout << "  Quantidade: " << itens[i].qnt << endl;
        cout << "  Subtotal: " << itens[i].subtotal << endl;
        cout << endl;
    }

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

void converteDados(string vendasCadastradas[], int linhas, InfoVenda &vendas, InfoItens itens[], int &numItens) {
    vendas.numeroVenda = stoi(vendasCadastradas[0]);
    strncpy(vendas.data, vendasCadastradas[1].c_str(), 10);
    vendas.data[10] = '\0';
    vendas.codigoCliente = stoi(vendasCadastradas[2]);
    vendas.qntId = stoi(vendasCadastradas[3]);
    vendas.totalVenda = stof(vendasCadastradas[4]);

    int numVendas = (linhas - 5) / 5; // Ignora as primeiras 5 linhas e calcula o número de vendas restantes
    numItens = 0; // Inicializa o contador de itens

    for (int i = 0; i < numVendas; i++) {
        int baseIndice = 5 + i * 5; // Índice base para cada venda, começando após as 5 primeiras linhas

        // Processa os itens da venda atual
        itens[numItens].id = stoi(vendasCadastradas[baseIndice]);
        strncpy(itens[numItens].nome, vendasCadastradas[baseIndice + 1].c_str(), TAMANHO_NOME);
        itens[numItens].nome[TAMANHO_NOME] = '\0'; // Garante que a string esteja terminada corretamente
        itens[numItens].preco = stof(vendasCadastradas[baseIndice + 2]);
        itens[numItens].qnt = stoi(vendasCadastradas[baseIndice + 3]);
        itens[numItens].subtotal = stof(vendasCadastradas[baseIndice + 4]);

        numItens++; // Incrementa o contador de itens
    }
}
