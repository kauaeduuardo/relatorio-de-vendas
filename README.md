# Relatórios de Vendas

 **Descrição:** Sistema implementado em C++ para o gerenciamento de relatórios de vendas com persistência em arquivos.

## Funcionalidades Principais
 - Geração de três tipos de relatórios:
    * Relatório de vendas totais (resumo geral);
    * Relatório de vendas listadas (todas as vendas detalhadas);
    * Relatório por intervalo de datas.
- Operações Realizadas:
    * Visualização dos relatórios gerados;
    * Reset dos arquivos;
    * Menu de interface para interação com usuário;
    * Salvamento dos dados de registros de vendas anteriores.

## Como utilizar
- Preparar um arquivo "vendas.txt" com os dados no formato:
    1 - Número da venda;
    2 - Data;
    3 - Código do cliente;
    4 - Quantidade de IDs na venda;
    5 - Valor total;
    6 - ID do produto;
    7 - Nome;
    8 - Preço unitário;
    9 - Quantidade vendida;
    10 - Subtotal;
    obs.: Os dados referentes aos produtos podem se repetir no documento (6 à 10).
- Compilar e executar para gerar os relatórios na pasta
- Interagir com o programa através do menu.

