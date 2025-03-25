#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <locale.h>
#include <iomanip>
#include <ctime>
#include <climits> // Para INT_MAX
#include <map>

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
    float subtotal;
};

struct DadosRelatorio {
    int quantidadeTotal = 0;
    float receitaTotal = 0;
    string maisVendido = " ";
    int maisUnidades = 0;
    string menosVendido = " ";
    int menosUnidades = INT_MAX;
};

struct produtosListados {
    string data;
    int id_produto;
    string nome_produto;
    float preco_unitario;
    int quantidade;
    float subtotal;
};

// Protótipos de funções
void contLinhas(const char* arquivo, int& linhas);
void lerDadosVendas(const char* arquivo, int linhas, string vendasCadastradas[]);
void converteDados(string vendasCadastradas[], int linhas, InfoVenda &vendas, InfoItens itens[], int &numItens, DadosRelatorio &dado);
void carregarDadosRelatorio(DadosRelatorio &dado);
void vendasTotaisRel(DadosRelatorio &dado, InfoVenda &vendas, InfoItens itens[], int numItens);
void vendasListaRel(InfoVenda &vendas, InfoItens itens[], int numItens);
bool conversaoData(const string &dataStr, tm &dataTm);
int compararDatas(const tm& data1, const tm& data2);
void lerVendasLista(produtosListados vendasListadas[], int& totalVendas);
void gerarRelatorioIntervalo(const produtosListados vendasListadas[], int totalVendas);
void limpaArquivo(char reseta);
void menu();

int main() {
    setlocale(LC_ALL, "portuguese");
    int linhas = 0;

    DadosRelatorio dado;
    carregarDadosRelatorio(dado);

    contLinhas(arquivo, linhas);

    string* vendasCadastradas = new string[linhas];
    lerDadosVendas(arquivo, linhas, vendasCadastradas);

    InfoItens itens[MAX_ITENS];
    InfoVenda vendas;
    int numItens = 0;

    converteDados(vendasCadastradas, linhas, vendas, itens, numItens, dado);
    vendasTotaisRel(dado, vendas, itens, numItens);
    vendasListaRel(vendas, itens, numItens);

    // Parte nova para o relatório por intervalo
    produtosListados vendasListadas[MAX_ITENS];
    int totalVendas = 0;
    lerVendasLista(vendasListadas, totalVendas);

    menu(); // Mostra o menu de opções

    char opcao;
    cin >> opcao;

    if (opcao == '3') {
        gerarRelatorioIntervalo(vendasListadas, totalVendas);
    } else if (opcao == '4') {
        cout << "Digite '1' para limpar vendas totais ou '2' para limpar lista de vendas: ";
        char reseta;
        cin >> reseta;
        limpaArquivo(reseta);
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

bool conversaoData(const string &dataStr, tm &dataTm) {
    istringstream ss(dataStr);
    char delimiter;

    ss >> dataTm.tm_mday >> delimiter;
    if (delimiter != '/') return false;

    ss >> dataTm.tm_mon >> delimiter;
    if (delimiter != '/') return false;

    ss >> dataTm.tm_year;

    // Ajustes para struct tm
    dataTm.tm_mon -= 1;    // Meses são 0-11
    dataTm.tm_year -= 1900; // Ano é contado a partir de 1900

    return true;
}

int compararDatas(const tm& data1, const tm& data2) {
    if (data1.tm_year < data2.tm_year) return -1;
    if (data1.tm_year > data2.tm_year) return 1;

    if (data1.tm_mon < data2.tm_mon) return -1;
    if (data1.tm_mon > data2.tm_mon) return 1;

    if (data1.tm_mday < data2.tm_mday) return -1;
    if (data1.tm_mday > data2.tm_mday) return 1;

    return 0;
}

void lerVendasLista(produtosListados vendasListadas[], int& totalVendas) {
    ifstream arquivo(vendasLista);
    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo de vendas." << endl;
        return;
    }

    string linha;
    totalVendas = 0;  // Inicializa o contador

    while (getline(arquivo, linha)) {
        // Verifica se ainda há espaço no array
        if (totalVendas >= MAX_ITENS) {
            cerr << "Aviso: Limite máximo de vendas atingido (" << MAX_ITENS << ")" << endl;
            break;
        }

        // Preenche a estrutura da venda atual
        vendasListadas[totalVendas].data = linha;

        if (!getline(arquivo, linha)) break;
        vendasListadas[totalVendas].id_produto = stoi(linha);

        if (!getline(arquivo, linha)) break;
        vendasListadas[totalVendas].nome_produto = linha;

        if (!getline(arquivo, linha)) break;
        vendasListadas[totalVendas].preco_unitario = stof(linha);

        if (!getline(arquivo, linha)) break;
        vendasListadas[totalVendas].quantidade = stoi(linha);

        if (!getline(arquivo, linha)) break;
        vendasListadas[totalVendas].subtotal = stof(linha);

        totalVendas++;  // Incrementa o contador após ler todos os campos
    }

    arquivo.close();
}

void gerarRelatorioIntervalo(const produtosListados vendasListadas[], int totalVendas) {
    string dataInicioStr, dataFimStr;
    cout << "Digite a data de inicio (dd/mm/aaaa): ";
    cin >> dataInicioStr;
    cout << "Digite a data de fim (dd/mm/aaaa): ";
    cin >> dataFimStr;

    tm dataInicio = {0}, dataFim = {0};
    if (!conversaoData(dataInicioStr, dataInicio)) {
        cerr << "Formato de data de inicio inválido." << endl;
        return;
    }
    if (!conversaoData(dataFimStr, dataFim)) {
        cerr << "Formato de data de fim inválido." << endl;
        return;
    }

    ofstream saida(intervaloVendas, ios::out | ios::trunc);
    if (!saida.is_open()) {
        cerr << "Erro ao criar arquivo de relatório por intervalo." << endl;
        return;
    }

    saida << fixed << setprecision(2);
    saida << "Relatório de Vendas - Período: " << dataInicioStr << " a " << dataFimStr << endl;
    saida << "------------------------------------------------------" << endl;

    float totalPeriodo = 0.0;
    bool encontrouVendas = false;

    // Variáveis para armazenar os produtos mais e menos vendidos
    string produtoMaisVendido = "";
    int unidadesMaisVendido = 0;
    string produtoMenosVendido = "";
    int unidadesMenosVendido = INT_MAX;

    // Mapa para armazenar o total de unidades por produto
    map<string, int> unidadesPorProduto;

    for (int i = 0; i < totalVendas; i++) {
        const produtosListados& venda = vendasListadas[i];
        tm dataVenda = {0};

        if (!conversaoData(venda.data, dataVenda)) {
            cerr << "Data inválida no arquivo: " << venda.data << endl;
            continue;
        }

        if (compararDatas(dataVenda, dataInicio) >= 0 &&
            compararDatas(dataVenda, dataFim) <= 0) {
            saida << "Data: " << venda.data << endl;
            saida << "- ID: " << venda.id_produto
                 << " | Produto: " << venda.nome_produto
                 << " | Quantidade: " << venda.quantidade
                 << " | Subtotal: R$ " << venda.subtotal << endl;
            totalPeriodo += venda.subtotal;
            encontrouVendas = true;

            // Atualiza o contador de unidades por produto
            unidadesPorProduto[venda.nome_produto] += venda.quantidade;
        }
    }

    // Encontra o produto mais e menos vendido
    for (const auto& par : unidadesPorProduto) {
        if (par.second > unidadesMaisVendido) {
            unidadesMaisVendido = par.second;
            produtoMaisVendido = par.first;
        }
        if (par.second < unidadesMenosVendido) {
            unidadesMenosVendido = par.second;
            produtoMenosVendido = par.first;
        }
    }

    if (!encontrouVendas) {
        saida << "Nenhuma venda encontrada no período especificado." << endl;
    } else {
        saida << "------------------------------------------------------" << endl;
        saida << "Total do período: R$ " << totalPeriodo << endl;
        saida << "Produto mais vendido: " << produtoMaisVendido
              << " (" << unidadesMaisVendido << " unidades)" << endl;
        saida << "Produto menos vendido: " << produtoMenosVendido
              << " (" << unidadesMenosVendido << " unidades)" << endl;
        saida << "------------------------------------------------------" << endl;
    }

    saida.close();
    cout << "Relatório por intervalo gerado em '" << intervaloVendas << "'" << endl;
}

void menu() {
    cout << "\nMENU DE RELATORIOS" << endl;
    cout << "1. Relatorio de vendas totais (ja gerado)" << endl;
    cout << "2. Lista completa de vendas (ja gerada)" << endl;
    cout << "3. Relatorio por intervalo de datas" << endl;
    cout << "4. Limpar arquivos de relatorio" << endl;
    cout << "Escolha uma opcao: ";
}
