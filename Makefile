# Rodar da raiz do projeto. "make jogar" compila e abre o jogo.
CXX      = c++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
CABECALHOS = src/dados/constantes.h src/dados/estruturas.h src/dados/vetor_dinamico.h src/arquivos/mapa.h src/jogo/tempo.h src/jogo/partida.h src/jogo/regras.h

FONTES = src/main.cpp src/interface/menu.cpp src/jogo/laco.cpp src/jogo/partida.cpp src/jogo/regras.cpp src/arquivos/mapa.cpp src/jogo/tempo.cpp \
         src/interface/tela.cpp src/jogo/entrada.cpp src/arquivos/historico.cpp

FONTES_JOGO = src/main.cpp src/jogo/laco.cpp src/jogo/partida.cpp src/jogo/regras.cpp \
              src/jogo/tempo.cpp src/jogo/entrada.cpp src/arquivos/mapa.cpp \
              src/arquivos/historico.cpp src/interface/menu.cpp src/interface/tela.cpp

.PHONY: testes vazamentos verificar-windows jogo jogar limpar

testes: build/teste_vetor build/teste_mapa build/teste_tempo build/teste_regras build/teste_historico
	./build/teste_vetor
	./build/teste_mapa testes/mapas/*.txt
	./build/teste_tempo
	./build/teste_regras
	cd build && ./teste_historico

# Vazamentos de memoria (macOS)
vazamentos: build/teste_vetor build/teste_mapa build/teste_tempo build/teste_regras build/teste_historico
	leaks --atExit -- ./build/teste_vetor
	leaks --atExit -- ./build/teste_mapa testes/mapas/*.txt
	leaks --atExit -- ./build/teste_tempo
	leaks --atExit -- ./build/teste_regras
	cd build && leaks --atExit -- ./teste_historico

build/teste_tempo: testes/teste_tempo.cpp src/jogo/tempo.cpp $(CABECALHOS) src/jogo/tempo.h
	@mkdir -p build
	$(CXX) $(CXXFLAGS) testes/teste_tempo.cpp src/jogo/tempo.cpp -o $@

build/teste_regras: testes/teste_regras.cpp src/jogo/regras.cpp src/jogo/partida.cpp src/arquivos/mapa.cpp src/jogo/tempo.cpp $(CABECALHOS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) testes/teste_regras.cpp src/jogo/regras.cpp src/jogo/partida.cpp src/arquivos/mapa.cpp src/jogo/tempo.cpp -o $@

build/teste_historico: testes/teste_historico.cpp src/arquivos/historico.cpp $(CABECALHOS) src/arquivos/historico.h
	@mkdir -p build
	$(CXX) $(CXXFLAGS) testes/teste_historico.cpp src/arquivos/historico.cpp -o $@

build/teste_vetor: testes/teste_vetor.cpp $(CABECALHOS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) testes/teste_vetor.cpp -o $@

build/teste_mapa: testes/teste_mapa.cpp src/arquivos/mapa.cpp $(CABECALHOS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) testes/teste_mapa.cpp src/arquivos/mapa.cpp -o $@

# Confere so a sintaxe do jogo completo com declaracoes falsas de windows.h e conio.h.
verificar-windows:
	@for f in $(FONTES); do $(CXX) $(CXXFLAGS) -DJOGO_WINDOWS -Itestes/stub -fsyntax-only $$f || exit 1; done
	@echo "sintaxe ok"

# Jogo completo. No macOS/Linux gera ./bomberman (usa termios e ANSI).
# No Windows (MinGW) gera bomberman.exe (usa windows.h e conio.h).
ifeq ($(OS),Windows_NT)
EXECUTAVEL = bomberman.exe
else
EXECUTAVEL = bomberman
endif

jogo:
	$(CXX) $(CXXFLAGS) $(FONTES_JOGO) -o $(EXECUTAVEL)

jogar: jogo
	./$(EXECUTAVEL)

limpar:
	rm -rf build bomberman bomberman.exe
