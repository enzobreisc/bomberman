# Makefile para desenvolver/testar fora do Windows (clang++ ou g++).
# No Windows, o jogo completo compila com g++/MinGW (veja o README).
CXX      = c++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

.PHONY: testes vazamentos limpar

testes: build/teste_vetor
	./build/teste_vetor

# Verifica vazamentos de memoria (macOS: leaks; Linux: valgrind)
vazamentos: build/teste_vetor
	leaks --atExit -- ./build/teste_vetor

build/teste_vetor: testes/teste_vetor.cpp estruturas.h vetor_dinamico.h constantes.h
	@mkdir -p build
	$(CXX) $(CXXFLAGS) testes/teste_vetor.cpp -o $@

limpar:
	rm -rf build
