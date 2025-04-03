# Relatórios de Vendas

 **Descrição:** Este sistema, desenvolvido em C++, permite o gerenciamento de relatórios de vendas com persistência em arquivos. Ele oferece diferentes tipos de relatórios e facilita a visualização de dados importantes sobre as vendas realizadas.

## Funcionalidades Principais
O sistema oferece três tipos de relatórios para atender a diferentes necessidades:  
1. **Relatório de Vendas Totais:** Exibe um resumo geral das vendas realizadas.
2. **Relatório de Vendas Listadas:** Apresenta uma lista detalhada de todas as vendas.
3. **Relatório por Intervalo de Datas:** Permite visualizar as vendas dentro de um período específico.

## Operações Realizadas:
* Visualização dos relatórios gerados.
* Reset dos arquivos.
* Menu de interface para interação com usuário.
* Salvamento dos dados de registros de vendas anteriores.

## Como utilizar
### Passo 1: Preparar o Arquivo de Dados
Crie um arquivo chamado vendas.txt com os dados das vendas no seguinte formato:
```cpp
Número da venda;
Data;
Código do cliente;
Quantidade de IDs na venda;
Valor total;
ID do produto;
Nome;
Preço unitário;
Quantidade vendida;
Subtotal;
```
**Observação:** Os dados dos produtos podem se repetir no mesmo arquivo (**de ID à Subtotal**).

### Passo 2: Compilar e Executar 
1. Compile o código-fonte em C++.
2. Execute o programa para gerar os relatórios na pasta de saída.

### Passo 3: Interagir com o Programa
Após a execução, o sistema apresentará um menu interativo no terminal. Siga as instruções no menu para gerar e visualizar os relatórios conforme necessário.

##
Este sistema de Relatórios de Vendas oferece uma solução simples e eficiente para o gerenciamento de dados de vendas, permitindo a geração de relatórios úteis para análise e tomada de decisão. Ele é fácil de usar, bastando preparar o arquivo de dados e interagir com o menu para visualizar as informações relevantes.
