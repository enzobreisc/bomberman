# Testes fora do Windows. Rodar da raiz do projeto.
CXX      = c++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
CABECALHOS = src/constantes.h src/estruturas.h src/vetor_dinamico.h src/mapa.h src/tempo.h src/partida.h src/regras.h

FONTES = src/main.cpp src/menu.cpp src/laco.cpp src/partida.cpp src/regras.cpp src/mapa.cpp src/tempo.cpp \
         src/tela.cpp src/entrada.cpp src/historico.cpp

.PHONY: testes vazamentos verificar-windows limpar

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

build/teste_tempo: testes/teste_tempo.cpp src/tempo.cpp $(CABECALHOS) src/tempo.h
	@mkdir -p build
	$(CXX) $(CXXFLAGS) testes/teste_tempo.cpp src/tempo.cpp -o $@

build/teste_regras: testes/teste_regras.cpp src/regras.cpp src/partida.cpp src/mapa.cpp src/tempo.cpp $(CABECALHOS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) testes/teste_regras.cpp src/regras.cpp src/partida.cpp src/mapa.cpp src/tempo.cpp -o $@

build/teste_historico: testes/teste_historico.cpp src/historico.cpp $(CABECALHOS) src/historico.h
	@mkdir -p build
	$(CXX) $(CXXFLAGS) testes/teste_historico.cpp src/historico.cpp -o $@

build/teste_vetor: testes/teste_vetor.cpp $(CABECALHOS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) testes/teste_vetor.cpp -o $@

build/teste_mapa: testes/teste_mapa.cpp src/mapa.cpp $(CABECALHOS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) testes/teste_mapa.cpp src/mapa.cpp -o $@

# Confere so a sintaxe do jogo completo com declaracoes falsas de windows.h e conio.h.
verificar-windows:
	@for f in $(FONTES); do $(CXX) $(CXXFLAGS) -Itestes/stub -fsyntax-only $$f || exit 1; done
	@echo "sintaxe ok"

limpar:
	rm -rf build
