# Bomberman: Duelo em Tempo Real

Trabalho M3 - Algoritmos e Programação II (22817)
Universidade do Vale do Itajaí - Escola Politécnica - Ciência da Computação

Jogo de Bomberman para dois jogadores no mesmo teclado, em tempo real, feito em
C++ para console do Windows (`conio.h` e `windows.h`), sem bibliotecas gráficas.

> Este README é escrito junto com cada etapa do projeto. Seções marcadas com
> _(pendente)_ ainda serão preenchidas nas próximas etapas.

## (a) Como compilar e executar

## Estrutura de pastas

```
src/main.cpp
src/dados/       constantes, structs e vetor dinâmico (template)
src/jogo/        regras, partida, laço, relógio e teclado
src/arquivos/    leitura de mapas e histórico.csv
src/interface/   tela e menu
testes/          testes portáveis, mapas de teste e declarações falsas do Windows
mapaN.txt        mapas (na raiz, a pasta de execução)
README.md, Makefile
```

O jogo usa `conio.h` e `windows.h`, então compila no Windows (g++/MinGW), na
pasta do projeto (onde ficam os `mapaN.txt`):

```
g++ -std=c++17 -Isrc -o bomberman.exe src/main.cpp src/jogo/laco.cpp src/jogo/partida.cpp src/jogo/regras.cpp src/jogo/tempo.cpp src/jogo/entrada.cpp src/arquivos/mapa.cpp src/arquivos/historico.cpp src/interface/menu.cpp src/interface/tela.cpp
bomberman.exe
```

O `historico.csv` é criado na pasta em que o jogo é executado.

Testes das regras, mapas, relógio, vetor e histórico (portáveis, rodam também
fora do Windows) e conferência de sintaxe do jogo completo com declarações
falsas de `windows.h`/`conio.h`:

```
make testes
make verificar-windows
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
| `BOMBAS_POR_JOGADOR` | 3        | bombas ativas ao mesmo tempo por jogador      |
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
  (`BOMBAS_POR_JOGADOR - bombasAtivas`).
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

### Acoes e EstadoTeclas
- **Por que existem:** `lerAcoes` só lê o teclado e preenche `Acoes` (direção
  desejada de cada jogador, pedido de bomba, pausa e abandono); as regras é que
  decidem o que fazer. `EstadoTeclas` guarda o estado anterior das teclas para
  detectar o instante em que são apertadas (bomba, P e ESC acontecem uma vez por
  aperto).
- **Decisão:** o estado anterior fica numa struct local passada por ponteiro, e
  não numa variável `static`, porque variáveis globais/estáticas são proibidas.
- **Direção:** `-1`, `0` ou `1` em cada eixo, então movimento diagonal não
  existe (vertical tem prioridade) e teclas opostas se anulam.

### Partida
- **Por que existe:** reúne o estado de uma partida para ser passada por
  ponteiro (`partida->`) às sub-rotinas.
- **Campos:** `mapa`; `celulasIniciais` (cópia da matriz para reiniciar a
  rodada); `jogadores[2]` (sempre dois, então vetor fixo de verdade);
  `bombas` e `chamas` (`VetorDinamico`, quantidade imprevisível); `relogio`;
  `pausado`; `numeroRodada`; `numeroMapa` (para o histórico); `instanteInicioRodada`.
- **Decisão (reiniciar rodada):** guardar uma cópia do mapa em memória em vez
  de reler o arquivo. _Custo:_ uma matriz extra por partida. _Vantagem:_ regras
  não acessam arquivo e o arquivo apagado no meio da partida não quebra o jogo.
  _Risco:_ mais uma alocação para liberar (feita no fim da partida).
- **Calculado, não guardado:** o tempo restante da rodada
  (`RODADA_MS - (agoraJogo - instanteInicioRodada)`).

### RegistroHistorico
- **Por que existe:** uma linha do `historico.csv`.
- **Campos:** `nome1`, `nome2` (`std::string`), `mapa` (`int`, o número N do
  `mapaN.txt`, como no exemplo do enunciado), `vitorias1`, `vitorias2`,
  `segundos` (tempo de jogo, sem contar a pausa).
- **Calculado, não guardado:** o título do mapa (basta abrir o `mapaN.txt`).

### VetorDinamico<T> (`vetor_dinamico.h`)
- Controle do vetor genérico: `dados`, `quantidade`, `capacidade` (começa em 2).
- **Dobrar a capacidade:** até 1000 elementos são feitas cerca de 9 cópias
  inteiras e o custo total é linear; somando 1 seriam centenas de milhares de
  cópias.
- **Remoção por índice:** troca com o último elemento, custo O(1) (a ordem não
  importa). Deslocar os elementos custaria O(n).
- **Ponteiros inválidos:** ao crescer, o vetor antigo é apagado, então o código
  guarda **índices**, nunca ponteiros para elementos.

### Relógio do jogo e ritmo do ciclo (`tempo.h` / `tempo.cpp`)
- `atualizarRelogio` (uma vez por ciclo) mede o tempo real desde o ciclo
  anterior com `steady_clock` e soma ao relógio do jogo **só se não estiver
  pausado**. Assim uma bomba com 1 s restante continua com 1 s depois de uma
  pausa de 2 s ou de 2 minutos.
- `completarCiclo` espera só o que falta para 33 ms (`sleep_until`), e é o
  **único** lugar do programa que dorme. Se o ciclo demorou mais, não espera.
- Cronômetro regressivo: `tempoRestante` + `formatarMMSS` (mm:ss).
- _E se um ciclo demorar 500 ms?_ O passo é limitado por jogador a um por vez e
  o próximo só é liberado 150 ms depois do instante do passo anterior, então o
  jogador não anda várias casas de uma vez.
- Teste: `testes/teste_tempo.cpp` mostra o relógio parado durante a pausa e
  continuando exatamente de onde parou.

## (d) Pontos de alocação dinâmica (quem aloca / quem libera)

| O que                         | Quem aloca            | Quem libera        |
|-------------------------------|-----------------------|--------------------|
| `VetorDinamico<T>` (bombas, chamas) | `iniciarVetor`, `crescerVetor` | `liberarVetor` |
| Matriz `Mapa.celulas` (`char**`)    | `carregarMapa` (mapa.cpp) | `liberarMapa`; se a leitura falha no meio, o próprio `carregarMapa` libera o que já alocou |
| Vetor temporário `int[colunas]` (leitura formato N) | `lerLinhaMapa` | a mesma função, antes de retornar |
| Cópia do mapa `Partida.celulasIniciais` | `iniciarPartida` (via `copiarCelulas`) | `encerrarPartida` (via `liberarCelulas`) |
| Vetores `Partida.bombas` e `Partida.chamas` | `iniciarRodada` (`iniciarVetor`) | `encerrarRodada` (`liberarVetor`), ao fim de cada rodada; `encerrarPartida` também chama, para o caso do ESC |
| Fila da cadeia e lista de blocos destruídos | `processarExplosoes` (vetores locais) | a mesma função, antes de retornar |
| Vetor `bool validos[]` da lista de mapas | `escolherMapa` (menu.cpp) | a mesma função |
| Vetor de `RegistroHistorico` | `carregarHistorico` (tamanho exato) | `liberarHistorico`, chamada em `executarHistorico` |

`encerrarRodada` roda ao fim de cada rodada (normal ou ESC) e `encerrarPartida`
é chamada em todos os caminhos de saída de `executarJogar`:
fim normal, ESC com confirmação e falha de carregamento (nesse caso o próprio
`iniciarPartida` já não deixa nada alocado).

## (e) Leitura dos dois formatos de mapa

Arquivo (`mapaN.txt`): primeira linha `L C formato` (`C` = caracteres,
`N` = números), segunda linha o título, depois `L` linhas com `C` valores.

- **Descoberta:** `descobrirMapas()` tenta abrir `mapa1.txt`, `mapa2.txt`... com
  `is_open()` até um não existir. Nenhum número de mapas está escrito no código.
- **Função template única:** `lerCelulas<T>` lê a linha inteira com `getline`,
  extrai valores com `>>` numa variável do tipo `T` (`char` no formato C, `int`
  no formato N) e confere a quantidade. Não há duas funções duplicadas.
- **Tradução do formato N:** acontece num único ponto (`traduzirNumero`, chamada
  por `lerLinhaMapa` logo após a leitura): `1`→`#`, `2`→`+`, `0`→`.`, `3`→`1`,
  `4`→`2`. Depois disso o jogo só conhece o formato C.
- **Validações** (com mensagem do motivo, sem imprimir nada): primeira linha
  ausente, L ou C não numéricos ou menores que 5, formato diferente de C/N,
  título ausente, menos linhas que L, linha com quantidade de valores diferente
  de C, valor fora do conjunto e ausência de exatamente um jogador 1 e um
  jogador 2. Se falhar no meio, a memória já alocada é liberada.
- **Posições iniciais:** `extrairPosicoesIniciais` troca os símbolos `1`/`2` por
  célula vazia e devolve as posições (que ficam guardadas em `Jogador`).
- **Testes:** `make testes` roda `testes/teste_mapa.cpp` com mapas válidos e
  inválidos de `testes/mapas/` (inclusive 5x5, 25x60, CRLF, vazio e quebrados
  de cada tipo). `make vazamentos` confirma 0 vazamentos no macOS.
- **Formato C:** os símbolos de uma linha vêm juntos, sem separador (como no
  enunciado); o `>>` sobre `char` também aceita separados por espaço.
- `mapa1.txt` e `mapa2.txt` são os exemplos do enunciado (11x13, C e N).

## Regras do jogo (`regras.cpp`)

Nenhuma função de `regras.cpp` imprime, lê arquivo ou espera.

- **Passo:** só anda se `agoraJogo >= proximoPasso` do próprio jogador; ao andar,
  `proximoPasso = agoraJogo + PASSO_MS` (e não `+= PASSO_MS`), então um ciclo
  lento ou um jogador parado não acumula passos. Só a célula de destino é
  conferida (parede, bloco, bomba, outro jogador vivo): quem coloca a bomba pode
  sair da célula dela, mas não consegue voltar.
- **Bomba:** no máximo `BOMBAS_POR_JOGADOR` ativas por jogador e nunca em célula
  que já tem bomba. Explode quando `agoraJogo - instanteColocada >= PAVIO_MS`.
- **Explosão em cadeia:** uma fila de índices (`VetorDinamico<int>`) recebe as
  bombas vencidas; cada bomba é marcada `explodindo` ao entrar na fila, então
  nunca explode duas vezes. A fila cresce enquanto é percorrida (por isso o laço
  usa índice). As bombas só são removidas do vetor depois da cadeia, de trás
  para frente, porque a remoção troca com o último elemento.
- **Blocos:** um bloco atingido interrompe a explosão naquela direção e só vira
  vazio depois da cadeia inteira, para o resultado não depender da ordem em que
  as bombas são processadas. Parede não é atingida.
- **Chamas:** cada célula atingida ganha uma chama de `CHAMA_MS`; se já havia uma,
  só renova o fim. _Decisão:_ a célula de um bloco destruído pode ser pisada
  durante as chamas, mas quem entra numa chama morre.
- **Morte:** depois de mover e explodir, quem está em célula com chama morre.
- **Fim da rodada:** `avaliarRodada` devolve o índice do sobrevivente, empate
  (mortes simultâneas ou tempo esgotado) ou "em andamento".

## Rodadas, partida e laço (`partida.cpp`, `laco.cpp`)

- **Reiniciar a rodada:** `iniciarRodada` copia `celulasIniciais` de volta para o
  mapa (sem reler o arquivo), aloca vetores novos de bombas e chamas
  (liberados por `encerrarRodada`) e recoloca os jogadores. O relógio do jogo **não** é zerado: o
  cronômetro da rodada usa `instanteInicioRodada`, e o tempo total da partida
  para o histórico é simplesmente `agoraJogo` (sem pausas).
- **Ordem do laço** (`executarRodada`): ler entrada, tratar abandono e pausa,
  atualizar relógio, mover/colocar bombas, explodir, apagar chamas, matar,
  verificar fim da rodada, desenhar e completar os 33 ms.
- **Telas bloqueantes** (confirmação do ESC, resultados): depois delas
  `reiniciarCiclo` descarta o tempo real gasto esperando o jogador, senão a
  próxima atualização do relógio somaria esse tempo de uma vez.
- **Abandono (ESC):** pede confirmação; se confirmar, a partida não é gravada no
  histórico. Fim de partida: 2 rodadas vencidas ou 3 rodadas disputadas.
- **Tela** (`tela.cpp`): cada quadro é montado em uma `std::string` (uma letra
  por célula, linhas completadas com espaços) e impresso de uma vez após
  `SetConsoleCursorPosition(0,0)`, com o cursor oculto. `system("cls")` só é
  usado em trocas de tela, nunca dentro do laço.
- **Histórico** (`historico.cpp`): `historico.csv` com
  `nome1,nome2,mapa,vitorias1,vitorias2,segundos`, em que `mapa` é o número do
  mapa (ex.: `Ana,Bruno,1,2,0,171`). A primeira passada conta só as linhas
  válidas, para alocar exatamente o necessário. Linhas inválidas são ignoradas.
- **Menu** (`menu.cpp`, fora da lista sugerida): mantém `main` curta; lista os
  mapas pelo título (inválidos aparecem como indisponíveis com o motivo), deixa
  escolher ou sortear e valida os nomes.

## Mapas

| Arquivo      | Tamanho | Formato | Observação                              |
|--------------|---------|---------|-----------------------------------------|
| `mapa1.txt`  | 11x13   | C       | exemplo do enunciado                    |
| `mapa2.txt`  | 11x13   | N       | exemplo do enunciado                    |
| `mapa3.txt`  | 7x9     | C       | pequeno                                 |
| `mapa4.txt`  | 17x31   | N       | grande                                  |
| `mapa5.txt`  | 13x15   | C       | dimensão diferente de 11x13             |

Os mapas autorais têm simetria de ponto (rotação de 180°) e cada jogador começa
com 3 células livres ao redor, então as chances são iguais.

## (f) Desafios extras

Nenhum implementado até o momento.

## Integrantes

_(pendente: nomes, mês/ano e professor/disciplina)_
