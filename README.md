# Bomberman: Duelo em Tempo Real

Trabalho M3 - Algoritmos e Programação II (22817)
Universidade do Vale do Itajaí - Escola Politécnica - Ciência da Computação

Jogo de Bomberman para dois jogadores no mesmo teclado, em tempo real, feito em
C++ para console do Windows (`conio.h` e `windows.h`), sem bibliotecas gráficas.

> Este README é escrito junto com cada etapa do projeto. Seções marcadas com
> _(pendente)_ ainda serão preenchidas nas próximas etapas.

## (a) Como compilar e executar

_(pendente: comandos finais do jogo completo — Etapa 11/12)_

Etapas já prontas (testes portáveis, rodam também fora do Windows):

```
make testes
```

## (b) Intervalos de tempo e constantes

Todas ficam em `constantes.h` (nenhum valor "solto" no código).

| Constante            | Valor    | Significado                                   |
|----------------------|----------|-----------------------------------------------|
| `CICLO_MS`           | 33 ms    | duração de um ciclo do laço do jogo (~30 fps) |
| `PASSO_MS`           | 150 ms   | intervalo mínimo entre passos de um jogador   |
| `PAVIO_MS`           | 3000 ms  | tempo da bomba até explodir                   |
| `CHAMA_MS`           | 500 ms   | tempo que cada chama fica na arena            |
| `RODADA_MS`          | 120000 ms| duração máxima de uma rodada (2 minutos)      |
| `ALCANCE_BOMBA`      | 2        | células de alcance em cada direção            |
| `MAX_BOMBAS_JOGADOR` | 3        | bombas ativas ao mesmo tempo por jogador      |
| `TOLERANCIA_MS`      | 100 ms   | tolerância aceita na medição do pavio         |
| `MIN_LINHAS_COLUNAS` | 5        | tamanho mínimo de um mapa                     |

## (c) Decisões de projeto das structs

Todas as structs estão em `estruturas.h` e são só dados (sem métodos nem
construtores). Não há variáveis globais.

### Mapa
- **Por que existe:** representa a arena carregada de um arquivo.
- **Campos:** `linhas`/`colunas` (`int`, tamanhos pequenos); `titulo`
  (`std::string`, tamanho variável, só é lido e exibido); `celulas` (`char**`,
  matriz alocada com `new` conforme L e C, sem limite fixo).
- **Decisão:** as células ficam sempre em `char` (símbolos do formato C). O
  formato N é traduzido para esses símbolos logo após a leitura, então o resto
  do jogo ignora o formato do arquivo. _Alternativa descartada:_ guardar os
  números e traduzir na hora de desenhar/aplicar regras (espalharia dois
  formatos pelo código todo).
- **Calculado, não guardado:** o formato do arquivo (só importa na leitura).

### Jogador
- **Por que existe:** estado de cada um dos dois jogadores.
- **Campos:** `nome`; `linha`/`coluna` (posição atual); `linhaInicial`/
  `colunaInicial` (para reiniciar a rodada); `vivo` (`bool`); `rodadasVencidas`;
  `bombasAtivas`; `proximoPasso` (`long long`, instante no relógio do jogo em
  que ele poderá andar de novo).
- **Calculado, não guardado:** quantas bombas ele ainda pode colocar
  (`MAX_BOMBAS_JOGADOR - bombasAtivas`).
- **Decisão:** `proximoPasso` por jogador faz com que um jogador parado não
  atrase o outro.

### Bomba
- **Por que existe:** cada bomba ativa na arena.
- **Campos:** `linha`/`coluna`; `dono` (**índice** 0/1 do jogador, não ponteiro,
  porque o vetor de jogadores é fixo e o índice nunca fica inválido);
  `alcance`; `instanteColocada` (o instante de explosão é calculado:
  `instanteColocada + PAVIO_MS`); `explodindo` (marca usada na reação em cadeia
  para nunca explodir duas vezes).
- **Calculado, não guardado:** o instante da explosão.

### Chama
- **Por que existe:** cada célula em chamas.
- **Campos:** `linha`/`coluna` e `instanteFim` (quando se apaga). Guardar o
  instante de fim (e não "há quanto tempo acendeu") evita somar a cada ciclo.

### Relogio
- **Por que existe:** isola o controle de tempo. `agoraJogo` (ms, 64 bits) só
  avança quando o jogo não está pausado; `inicioCiclo` guarda o começo do ciclo
  atual para calcular o delta e para esperar só o que falta até 33 ms.
- **Decisão:** todos os instantes do jogo (pavio, chamas, passos, fim da
  rodada) usam esse relógio, então a pausa congela tudo sem código extra.
  _Alternativa descartada:_ guardar `time_point` do `steady_clock` em cada
  bomba (a pausa exigiria corrigir todos os instantes).

### Partida
- **Por que existe:** reúne o estado de uma partida para ser passada por
  ponteiro (`partida->`) às sub-rotinas.
- **Campos:** `mapa`; `celulasIniciais` (cópia da matriz para reiniciar a
  rodada); `jogadores[2]` (sempre dois, então vetor fixo de verdade);
  `bombas` e `chamas` (`VetorDinamico`, quantidade imprevisível); `relogio`;
  `pausado`; `numeroRodada`; `instanteInicioRodada`.
- **Decisão (reiniciar rodada):** guardar uma cópia do mapa em memória em vez
  de reler o arquivo. _Custo:_ uma matriz extra por partida. _Vantagem:_ regras
  não acessam arquivo e o arquivo apagado no meio da partida não quebra o jogo.
  _Risco:_ mais uma alocação para liberar (feita no fim da partida).
- **Calculado, não guardado:** o tempo restante da rodada
  (`RODADA_MS - (agoraJogo - instanteInicioRodada)`).

### RegistroHistorico
- **Por que existe:** uma linha do `historico.csv`.
- **Campos:** `nome1`, `nome2`, `mapa` (`std::string`), `vitorias1`,
  `vitorias2`, `segundos` (tempo de jogo, sem contar a pausa).

### VetorDinamico<T> (`vetor_dinamico.h`)
- Controle do vetor genérico: `dados`, `quantidade`, `capacidade` (começa em 2).
- **Dobrar a capacidade:** até 1000 elementos são feitas cerca de 9 cópias
  inteiras e o custo total é linear; somando 1 seriam centenas de milhares de
  cópias.
- **Remoção por índice:** troca com o último elemento, custo O(1) (a ordem não
  importa). Deslocar os elementos custaria O(n).
- **Ponteiros inválidos:** ao crescer, o vetor antigo é apagado, então o código
  guarda **índices**, nunca ponteiros para elementos.

## (d) Pontos de alocação dinâmica (quem aloca / quem libera)

| O que                         | Quem aloca            | Quem libera        |
|-------------------------------|-----------------------|--------------------|
| `VetorDinamico<T>` (bombas, chamas) | `iniciarVetor`, `crescerVetor` | `liberarVetor` |

_(a tabela é completada a cada etapa)_

## (e) Leitura dos dois formatos de mapa

_(pendente: Etapa 3)_

## (f) Desafios extras

Nenhum implementado até o momento.

## Integrantes

_(pendente: nomes, mês/ano e professor/disciplina)_
