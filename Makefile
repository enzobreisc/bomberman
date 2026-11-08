# Testes fora do Windows. Rodar da raiz do projeto.
CXX      = c++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
CABECALHOS = constantes.h estruturas.h vetor_dinamico.h mapa.h

.PHONY: testes vazamentos limpar

testes: build/teste_vetor build/teste_mapa build/teste_tempo
	./build/teste_vetor
	./build/teste_mapa testes/mapas/*.txt
	./build/teste_tempo

# Vazamentos de memoria (macOS)
vazamentos: build/teste_vetor build/teste_mapa build/teste_tempo
	leaks --atExit -- ./build/teste_vetor
	leaks --atExit -- ./build/teste_mapa testes/mapas/*.txt
	leaks --atExit -- ./build/teste_tempo

build/teste_tempo: testes/teste_tempo.cpp tempo.cpp $(CABECALHOS) tempo.h
	@mkdir -p build
	$(CXX) $(CXXFLAGS) testes/teste_tempo.cpp tempo.cpp -o $@

build/teste_vetor: testes/teste_vetor.cpp $(CABECALHOS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) testes/teste_vetor.cpp -o $@

build/teste_mapa: testes/teste_mapa.cpp mapa.cpp $(CABECALHOS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) testes/teste_mapa.cpp mapa.cpp -o $@

limpar:
	rm -rf build
