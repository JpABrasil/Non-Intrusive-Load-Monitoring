## Brainstorming:
### Ideias de Implementação
1. **Classificação a partir da amplitude dos harmônicos**:
    1.1 **Algoritmos**: Transformação de Fourier (Harmônicos); XGBoost | MLPs (Classificação)
    1.2 **Método:** Aplicação da transformação de fourier na Potência para cálculo das amplitudes do harmônicos associadas a cada exemplo
    1.3**Performances**:
        78% de acerto em identificações individuais (Uso de LabelEncoder)
    1.4**Questões**:

2. **Reconhecimento de Imagem de Gráficos Tensão-Corrente**
### Possíveis Melhorias:
**1:**
* 1. Calcular Potência Ativa e Reativa ao invés de Aparente
* 2. Garantir Distribuição Relativa em Treino e Teste Similar
* 3. Ler mais artigos

## Código:
### Scripts:
1. [utils.py](code/utils.py): Contém funções úteis que são utilizadas recorrentemente em outras partes do processo, como funções de leitura, transformação de dados, aplicação de *Transformação de Fourier*
### Notebooks:
2. [extracao_dados.ipynb](code/extracao_dados.ipynb): Notebook para extração, limpeza e análise dos dados iniciais, assim como a criação de datasets que serão utilizados para criação de modelos.
## Dados
### [raw](data/raw/)
**Aqui ficam os dados do dataset [original](https://www.nature.com/articles/s41597-020-0389-7) e adaptações dos metadados para csv(Algumas colunas foram ignoradas)**
1.  [metadata_individual.json](data/raw/metadata_individual.json): Arquivo Original com informações sobre os equipamentos individuais
2. [metadata_agregado.json](data/raw/metadata_agregado.json): Arquivo Original com informações sobre os experimentos agregados
3. [individual](data/raw/individual/): Pasta em que cada arquivo é uma medição de um equipamento (corrente-A , voltagem-V)
4. [agregado](data/raw/agregado/): Pasta em que cada arquivo é uma medição de um experimento de agregados(corrente-A , voltagem-V)
    * obs: Os nomes dos arquivos nas pastas estão indexados em base 1, e o index do csv quando aberto será baseado em 0 (**Mudar isso futuramente**)
### [processado](data/processado/)
**Aqui ficam os dados em que transformações foram aplicadas, ainda estão sucetiveis a tratamento antes de treinamento de modelo**
Obs: Cada linha se refere a um equipamento e tem colunas para a amplitude da frequência após transformada de fourier, como também informações advindas dos metadados individuais
1. [harmonicos_v1.csv](data/processado/harmonicos_individuais_v1.csv): 
    * Transformação de Fourier em torno da Potência Aparente(Corrente x Voltagem)
    * Sem Normalização  na Transformação
2. [harmonicos_v2.csv](data/processado/harmonicos_individuais_v2.csv): 
    * Transformação de Fourier em torno da Potência Aparente(Corrente x Voltagem)
    * Normalização  na Transformação (Multiplicado por $\left[\frac{2}{N° de Exemplos}\right]$)
3. [harmonicos_v3.csv](data/processado/harmonicos_individuais_v3.csv):
    * Transformação de Fourier em torno da Aparente(Corrente x Voltagem)
    * Normalização  na Transformação (Multiplicado por $\left[\frac{2}{N° de Exemplos}\right]$)
    * Usando a partir da frequência de 120