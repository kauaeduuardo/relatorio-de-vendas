#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <locale.h>
#include <iomanip>
#include <ctime>
#include <climits>
#include <map>
#include <cstdio>

#define TAMANHO_NOME 48
#define MAX_ITENS 100

using namespace std;

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

void configurarLocalizacao();
void contLinhas(int &linhas);
void lerDadosVendas(int linhas, string vendasCadastradas[]);
void converteDados(string vendasCadastradas[], int linhas, InfoVenda &vendas, InfoItens itens[], int &numItens, DadosRelatorio &dado);
void carregarDadosRelatorio(DadosRelatorio &dado);
void vendasTotaisRel(DadosRelatorio &dado, InfoVenda &vendas, InfoItens itens[], int numItens);
void vendasListaRel(InfoVenda &vendas, InfoItens itens[], int numItens);
bool conversaoData(const string &dataStr, tm &dataTm);
int compararDatas(const tm& data1, const tm& data2);
void lerVendasLista(produtosListados vendasListadas[], int& totalVendas);
void gerarRelatorioIntervalo(const produtosListados vendasListadas[], int totalVendas);
void limpaArquivo();
void exibirTotal();
void exibirLista();
void exibirIntervalo();
void menu(char &opcao);

int main() {
    configurarLocalizacao();
    int linhas = 0;

    DadosRelatorio dado;
    carregarDadosRelatorio(dado);

    contLinhas(linhas);

    string* vendasCadastradas = new string[linhas]; // Alocação de Memória - array dinamico
    lerDadosVendas(linhas, vendasCadastradas);

    InfoItens itens[MAX_ITENS];
    InfoVenda vendas;
    int numItens = 0;

    converteDados(vendasCadastradas, linhas, vendas, itens, numItens, dado);
    vendasTotaisRel(dado, vendas, itens, numItens);
    vendasListaRel(vendas, itens, numItens);

    produtosListados vendasListadas[MAX_ITENS];
    int totalVendas = 0;
    lerVendasLista(vendasListadas, totalVendas);

    char opcao;
    menu(opcao);
    while ( (opcao=='1') || (opcao=='2') || (opcao=='3') ||(opcao=='4') ||(opcao == '5') ) {
            switch (opcao){
                case '1':
                    cout << "Relatorio 'vendasTotais.txt' gerado!\n";
                    exibirTotal();
                break;
                case '2':
                    cout << "Relatorio 'vendasLista.txt' gerado!\n";
                    exibirLista();
                    cout << setfill('-') << setw(60) << "" << setfill(' ') << endl;
                break;
                case '3':
                    gerarRelatorioIntervalo(vendasListadas, totalVendas);
                    exibirIntervalo();
                    break;
                case '4':
                    limpaArquivo();
                break;
                case '5':
                    cout << "[Programa encerrado]";
                    return 0;
                break;
                default:
                    cout << "Escolha Invalida!\n";
                break;
            }
            cout << "Pressione Qualquer Tecla Para Retornar [MENU].\n";
            getchar();
            cin.ignore();
            menu(opcao);
    }

    delete[] vendasCadastradas;
    return 0;
}

void configurarLocalizacao()
{
    setlocale(LC_ALL, "Portuguese");
    locale::global(locale(""));
    cout.imbue(locale());
    cin.imbue(locale());
}

// Menu e exibição de relatórios
void menu(char &opcao) {
    system("cls");
    cout << "MENU DE RELATORIOS\n" << endl;
    cout << "1. Relatorio: Vendas Totais. " << endl;
    cout << "2. Relatorio: Vendas Listadas. " << endl;
    cout << "3. Relatorio: Vendas Por Intervalo. " << endl;
    cout << "4. Resetar Relatorio. " << endl;
    cout << "5. Encerrar Execucao. " << endl;
    cout << "Escolha uma opcao: ";
    cin >> opcao;
    system("cls");
}

// Contar linhas do arquivo vendas.txt
void contLinhas(int& linhas) {
    ifstream leitura("vendas.txt");
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

// Ler dados do arquivo vendas.txt
// e armazenar em um array de strings
void lerDadosVendas(int linhas, string vendasCadastradas[]) {
    ifstream leitura("vendas.txt");
    string linha;
    if(!leitura.is_open()) {
        cerr << "Erro ao tentar abrir o arquivo!" << endl;
        return;
    }

    for(int i = 0; i < linhas; i++) {
        if(getline(leitura, linha)) {
            vendasCadastradas[i] = linha;
        }
    }
    leitura.close();
}

// Converter os dados lidos do arquivo vendas.txt
// e armazenar em variaveis do tipo InfoVenda e InfoItens
void converteDados(string vendasCadastradas[], int linhas, InfoVenda &vendas, InfoItens itens[], int &numItens,DadosRelatorio &dado) {
    vendas.numeroVenda = stoi(vendasCadastradas[0]);
    strncpy(vendas.data, vendasCadastradas[1].c_str(), 10);
    vendas.data[10] = '\0';
    vendas.codigoCliente = stoi(vendasCadastradas[2]);
    vendas.qntId = stoi(vendasCadastradas[3]);
    vendas.totalVenda = stof(vendasCadastradas[4]);

    int numVendas = (linhas - 5) / 5; // Ignora as primeiras 5 linhas e calcula o numero de vendas restantes
    numItens = 0; // Inicializa o contador de itens

    for (int i = 0; i < numVendas; i++) {
        int baseIndice = 5 + i * 5; // Indice base para cada venda, comecando apos as 5 primeiras linhas

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
        numItens++;
    }
}

// Carregar dados do arquivo vendasTotais.txt
// e armazenar em variaveis do tipo DadosRelatorio
void carregarDadosRelatorio(DadosRelatorio &dado) {
    ifstream leitura(vendasTotais);
    if (!leitura.is_open()) {
        // Se o arquivo nao existir, mantem os valores padrao
        return;
    }

    string linha;
    if (getline(leitura, linha)) dado.quantidadeTotal = stoi(linha);
    if (getline(leitura, linha)) dado.receitaTotal = stof(linha);
    if (getline(leitura, linha)) {}

    if (getline(leitura, linha)) dado.maisVendido = linha;
    if (getline(leitura, linha)) dado.maisUnidades = stoi(linha);
    if (getline(leitura, linha)) dado.menosVendido = linha;
    if (getline(leitura, linha)) dado.menosUnidades = stoi(linha);

    leitura.close();
}

// Gerar o relatorio de vendas totais
void vendasTotaisRel(DadosRelatorio &dado, InfoVenda &vendas, InfoItens itens[],int numItens) {
    ofstream saida(vendasTotais, ios::out | ios::trunc);
    if (!saida.is_open()) {
        cerr << "Erro ao abrir arquivo." << endl;
        return;
    }
    saida << fixed << setprecision(2);
    saida << dado.quantidadeTotal << endl // Quantidade (em unidades) de produtos vendidos
          << dado.receitaTotal << endl // Receita total gerada na venda
          << dado.receitaTotal/dado.quantidadeTotal << endl // Media de renda por unidade vendida
          << dado.maisVendido << endl // Item mais vendido (nome)
          << dado.maisUnidades << endl // Item mais vendido (quantidade)
          << dado.menosVendido << endl // Item menos vendido (nome)
          << dado.menosUnidades << endl; // Item menos vendido (quantidade)
    saida.close();
}

// Gerar o relatorio de vendas listadas
void vendasListaRel(InfoVenda &vendas, InfoItens itens[], int numItens) {
    ofstream saida(vendasLista, ios::out | ios::app);
    if(!saida.is_open()) {
        cerr << "Erro ao abrir arquivo." << endl;
        return;  // Adicionei return para sair se o arquivo nao abrir
    }
    saida << fixed << setprecision(2);  // Para consistencia com outras saidas
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

// Limpa os dados de um arquivo
void limpaArquivo() {
    ofstream saida;
    char reseta;
    cout << "Qual relatorio voce deseja limpar?" << endl
         << "[1] Vendas Totais \n[2] Lista de Vendas\n[ ] Sair\n> ";
    cin >> reseta;
    switch (reseta) {
        case '1':
            saida.open("relatorio_vendasTotais.txt", ios::trunc); // Limpa vendasTotais.txt
            saida.close();
            cout << "\nVendas totais foi limpo." << endl;
            break;
        case '2':
            saida.open("relatorio_vendasLista.txt", ios::trunc);  // Limpa vendasLista.txt
            saida.close();
            cout << "\nLista de vendas foi limpo." << endl;
            break;
        default:
            //cout << "Opção inválida!" << endl;
            return;
    }
}

// Converte uma string de data no formato "dd/mm/aaaa" para struct tm
// Retorna true se a conversao for bem-sucedida, false caso contrario
bool conversaoData(const string &dataStr, tm &dataTm) {
    istringstream ss(dataStr);
    char delimiter;

    ss >> dataTm.tm_mday >> delimiter;
    if (delimiter != '/') return false;

    ss >> dataTm.tm_mon >> delimiter;
    if (delimiter != '/') return false;

    ss >> dataTm.tm_year;

    // Ajustes para struct tm
    dataTm.tm_mon -= 1;    // Meses sao 0-11
    dataTm.tm_year -= 1900; // Ano e contado a partir de 1900

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

// Ler o arquivo de vendas listadas e armazenar os dados em um array de produtosListados
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

// Gerar o relatorio de vendas por intervalo
void gerarRelatorioIntervalo(const produtosListados vendasListadas[], int totalVendas) {
    string dataInicioStr, dataFimStr;
    cout << "RELATORIO DE VENDAS POR INTERVALO:\n";
    cout << "Digite a data de inicio (dd/mm/aaaa): ";
    cin >> dataInicioStr;
    cout << "Digite a data de fim (dd/mm/aaaa): ";
    cin >> dataFimStr;

    tm dataInicio = {0}, dataFim = {0};
    if (!conversaoData(dataInicioStr, dataInicio)) {
        cerr << "Formato de data de inicio invalido." << endl;
        return;
    }
    if (!conversaoData(dataFimStr, dataFim)) {
        cerr << "Formato de data de fim invalido." << endl;
        return;
    }

    ofstream saida(intervaloVendas, ios::out | ios::trunc);
    if (!saida.is_open()) {
        cerr << "Erro ao criar arquivo de relatorio por intervalo." << endl;
        return;
    }

    saida << fixed << setprecision(2);
    saida << "\nRelatorio de Vendas - Periodo: " << dataInicioStr << " a " << dataFimStr << endl;
    saida << "------------------------------------------------------" << endl;

    float totalPeriodo = 0.0;
    int totalUnidades= 0;
    bool encontrouVendas = false;

    // Variaveis para armazenar os produtos mais e menos vendidos
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
            cerr << "Data invalida no arquivo: " << venda.data << endl;
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
            totalUnidades += venda.quantidade;
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
        saida << "Nenhuma venda encontrada no periodo especificado." << endl;
    } else {
        saida << "---------------------------------------------------------" << endl;
        saida << "Unidades vendidadas nesse periodo: " << totalUnidades << " unidades" << endl;
        saida << "Total do periodo: R$ " << totalPeriodo << endl;
        saida << "Produto mais vendido: " << produtoMaisVendido
              << " (" << unidadesMaisVendido << " unidades)" << endl;
        saida << "Produto menos vendido: " << produtoMenosVendido
              << " (" << unidadesMenosVendido << " unidades)" << endl;
        saida << "---------------------------------------------------------" << endl;
    }

    saida.close();
    cout << "\nRelatorio por intervalo gerado em '" << intervaloVendas << "'" << endl;
}

// Exibir o relatorio de vendas totais
void exibirTotal() {
    ifstream leitura(vendasTotais);
    string s;

    cout << "\nRELATORIO DE VENDAS TOTAIS:\n";
    cout << setfill('-') << setw(60) << "" << setfill(' ') << endl;

    if(getline(leitura, s))
        cout << "Total de itens vendidos: "
             << s << " unidades" << endl;

    if(getline(leitura, s))
        cout << "Renda Bruta: " << "R$ " << stod(s) << endl;

    if(getline(leitura, s))
        cout << "Media por produto vendido:" << "R$ " << stod(s) << endl;

    if(getline(leitura, s))
        cout << "Produto Mais vendido: " << s;
    if(getline(leitura, s))
        cout << " ("<< s << " unidades)"<< endl;

    if(getline(leitura, s))
        cout << "Produto Menos vendido: " << s;
    if(getline(leitura, s))
        cout << " ("<< s << " unidades)"<< endl;

    cout << setfill('-') << setw(60) << "" << setfill(' ') << endl;

    leitura.close();
}

// Exibir o relatorio de vendas listadas
void exibirLista() {
    ifstream leitura(vendasLista);
    string s;
    cout << "\nRELATORIO DE LISTA DE VENDAS:\n";
    cout << setfill('-') << setw(60) << "" << setfill(' ') << endl;

    if(!leitura.is_open()) {
        cerr << "Erro ao abrir arquivo de vendas listadas." << endl;
        return;
    }

    int contador = 0;
    while(getline(leitura, s)) {
        contador++;
        cout << "Data: " << s << endl;

        if(!getline(leitura, s)) break;
        cout << "ID do Produto: " << s << endl;

        if(!getline(leitura, s)) break;
        cout << "Nome do Produto: " << s << endl;

        if(!getline(leitura, s)) break;
        cout << "Preco Unitario: " << s << endl;

        if(!getline(leitura, s)) break;
        cout << "Quantidade Vendida: " << s << endl;

        if(!getline(leitura, s)) break;
        cout << "Subtotal: " << s << "\n\n";
    }

    if(contador == 0) {
        cout << "Nenhuma venda encontrada no arquivo." << endl;
    }

    leitura.close();
}

// Exibir o relatorio de vendas por intervalo
void exibirIntervalo() {
    ifstream leitura(intervaloVendas);
    string s;
    if(!leitura.is_open()) {
        cerr << "Erro ao abrir arquivo de vendas listadas." << endl;
        return;
    }

    while(getline(leitura,s)) {
        cout << s << endl;
    }
    leitura.close();
}
