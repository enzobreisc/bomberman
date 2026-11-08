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

_(pendente: rascunho na Etapa 1)_

## (d) Pontos de alocação dinâmica (quem aloca / quem libera)

_(pendente: preenchido a cada etapa)_

## (e) Leitura dos dois formatos de mapa

_(pendente: Etapa 3)_

## (f) Desafios extras

Nenhum implementado até o momento.

## Integrantes

_(pendente: nomes, mês/ano e professor/disciplina)_
