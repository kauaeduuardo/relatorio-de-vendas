#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring> // Para strncpy
#include <locale.h>
#include <iomanip>

#define TAMANHO_NOME 48

using namespace std;

const char* arquivo = "vendas.txt";
const char* relatorioTotal = "vendasTotais.txt";

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

struct DadosRelatorio {
    int quantidadeTotal = 0;
    float receitaTotal = 0;
    string maisVendido = " ";
    int maisUnidades = 0;
    string menosVendido = " ";
    int menosUnidades = INT_MAX;
};

//Funções - protótipos:

void contLinhas(const char* arquivo, int& linhas);
void lerDadosVendas(const char* arquivo, int linhas, string vendasCadastradas[]);
void converteDados(string vendasCadastradas[], int linhas, InfoVenda &vendas, InfoItens itens[],int &numItens, DadosRelatorio &dado);
void vendasTotaisRel(DadosRelatorio &dado, InfoVenda &vendas, InfoItens itens[],int numItens);

int main()
{
    setlocale(LC_ALL,"portuguese");
    int linhas = 0;

    contLinhas(arquivo,linhas);

    string* vendasCadastradas  = new string[linhas];

    lerDadosVendas(arquivo,linhas,vendasCadastradas);

    InfoItens itens[linhas-5];
    InfoVenda vendas;
    int numItens = 0;
    DadosRelatorio dado;
    converteDados(vendasCadastradas,linhas,vendas,itens,numItens, dado);
    vendasTotaisRel(dado,vendas,itens,numItens);

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

void vendasTotaisRel(DadosRelatorio &dado, InfoVenda &vendas, InfoItens itens[],int numItens) {
    ofstream saida(relatorioTotal, ios::out | ios::app);
    if (!saida.is_open()) {
        cerr << "Erro ao abrir o arquivo de relatório!" << endl;
        return;
    }

    saida << "Data: " << vendas.data << endl
          << "Número da Venda: " << vendas.numeroVenda << endl
          << "Código do Cliente: " << vendas.codigoCliente << endl
          << "Quantidade de IDs: " << vendas.qntId << endl
          << "Total da Venda: " << fixed << setprecision(2) << vendas.totalVenda << endl
          << endl;
    for (int i = 0; i < numItens; i++) {
        saida << "Item " << i + 1 << ":" << endl
              << "  ID: " << itens[i].id << endl
              << "  Nome: " << itens[i].nome << endl
              << "  Preço: " << itens[i].preco << endl
              << "  Quantidade: " << itens[i].qnt << endl
              << "  Subtotal: " << itens[i].subtotal << endl
              << endl;
    }
    saida << "Total de quantidades vendidas: " << dado.quantidadeTotal << endl
          << "Receita total gerada: " << dado.receitaTotal << endl
          << "media de lucro por item vendido: " << dado.receitaTotal/dado.quantidadeTotal << endl
          << "item mais vendido: " << dado.maisVendido << ", "<< dado.maisUnidades << " unidades" << endl
          << "item menos vendido: " << dado.menosVendido << ", "<< dado.menosUnidades << " unidades" << endl
          << endl;

    saida.close();
}
