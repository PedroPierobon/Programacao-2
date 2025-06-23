# Este Makefile é projetado para a seguinte estrutura de pastas:
# .
# ├── Makefile
# ├── bin/      (Onde o executável final será colocado)
# ├── build/    (Onde os arquivos objeto .o intermediários serão colocados)
# ├── include/  (Onde seus arquivos .h ficam)
# └── src/      (Onde seus arquivos .c ficam)

CC = gcc

TARGET = jogo

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin

EXEC = $(BIN_DIR)/$(TARGET)

SOURCES = $(wildcard $(SRC_DIR)/*.c)

OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

LDFLAGS = $(shell pkg-config --libs allegro-5 allegro_main-5 allegro_font-5 allegro_primitives-5 allegro_image-5 allegro_ttf-5)

CFLAGS = -Wall -g -I$(INCLUDE_DIR)

RM = rm -f

.PHONY: all
all: $(EXEC)


$(EXEC): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJECTS) -o $(EXEC) $(LDFLAGS)
	@echo "Compilação concluída! Executável criado em: $(EXEC)"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compilado $< -> $@"

.PHONY: clean
clean:
	$(RM) -r $(BUILD_DIR) $(BIN_DIR)
	@echo "Arquivos de compilação e executável removidos."

.PHONY: run
run: all
	@echo "Iniciando o jogo..."
	./$(EXEC)