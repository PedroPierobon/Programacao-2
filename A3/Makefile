# Makefile para Jogo em C com Allegro 5
# Este Makefile é projetado para a seguinte estrutura de pastas:
# .
# ├── Makefile
# ├── bin/      (Onde o executável final será colocado)
# ├── build/    (Onde os arquivos objeto .o intermediários serão colocados)
# ├── include/  (Onde seus arquivos .h ficam)
# └── src/      (Onde seus arquivos .c ficam)

# --- Variáveis de Configuração ---

# Nome do compilador C
CC = gcc

# Nome do executável final
TARGET = jogo

# Diretórios do projeto
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin

# O caminho completo para o executável final
EXEC = $(BIN_DIR)/$(TARGET)

# Encontra automaticamente todos os arquivos fonte .c na pasta src
# A função wildcard cria uma lista de todos os arquivos que correspondem ao padrão
SOURCES = $(wildcard $(SRC_DIR)/*.c)

# Gera os nomes dos arquivos objeto (.o) correspondentes, trocando o diretório de 'src' para 'build'
# Ex: src/main.c se torna build/main.o
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

# Flags de linkagem (bibliotecas). Usa pkg-config para pegar as flags do Allegro automaticamente.
# Isso torna o Makefile mais portável entre sistemas.
LDFLAGS = $(shell pkg-config --libs allegro-5 allegro_main-5 allegro_font-5 allegro_primitives-5 allegro_image-5 allegro_ttf-5)

# Flags de compilação
# -Wall: Ativa todos os avisos (warnings), uma boa prática.
# -g:    Inclui símbolos de depuração (essencial para usar um debugger como o GDB).
# -I...: Diz ao compilador para procurar por arquivos de cabeçalho (.h) na pasta 'include'.
CFLAGS = -Wall -g -I$(INCLUDE_DIR)

# Comando para remover arquivos/diretórios
RM = rm -f


# --- Regras do Makefile ---

# A regra 'all' é a regra padrão, executada quando você digita apenas 'make'.
# Ela depende do executável final.
.PHONY: all
all: $(EXEC)

# Regra para linkar os arquivos objeto e criar o executável final.
# Depende de todos os arquivos objeto (.o).
# O comando @mkdir cria os diretórios 'bin' e 'build' se eles não existirem.
# O '@' no início de um comando impede que ele seja impresso na tela.
$(EXEC): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJECTS) -o $(EXEC) $(LDFLAGS)
	@echo "Compilação concluída! Executável criado em: $(EXEC)"

# Regra de padrão para compilar arquivos .c em arquivos .o
# Isto diz ao make como transformar um arquivo 'src/qualquercoisa.c' em 'build/qualquercoisa.o'.
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compilado $< -> $@"

# Regra 'clean' para limpar os arquivos gerados.
# Útil para forçar uma recompilação completa do zero.
.PHONY: clean
clean:
	$(RM) -r $(BUILD_DIR) $(BIN_DIR)
	@echo "Arquivos de compilação e executável removidos."

# Regra 'run' para compilar e executar o jogo com um único comando.
# Depende da regra 'all' para garantir que tudo esteja compilado antes de rodar.
.PHONY: run
run: all
	@echo "Iniciando o jogo..."
	./$(EXEC)