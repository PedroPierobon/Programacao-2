# Makefile Corrigido e Verificado

# Nome do seu programa executável final
TARGET = game

# O compilador que vamos usar
CC = gcc

# Arquivos de código-fonte (.c).
SRCS = main.c core.c setting.c menu.c assets.c player.c playing.c

# Gera automaticamente os nomes dos arquivos de objeto (.o) a partir dos .c
OBJS = $(SRCS:.c=.o)

# Flags para o compilador. pkg-config encontra as flags da Allegro para nós.
CFLAGS = $(pkg-config --cflags allegro-5 allegro_font-5 allegro_primitives-5 allegro_image-5 allegro_main-5)

# Bibliotecas para linkar no final. pkg-config também as encontra.
LIBS = $(pkg-config --libs allegro-5 allegro_font-5 allegro_primitives-5 allegro_image-5 allegro_main-5)

# --- REGRAS DO MAKEFILE ---

# A regra padrão: chamada quando você digita apenas "make"
all: $(TARGET)

# Regra para criar o executável final (TARGET)
$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIBS)

# Regra genérica para criar um arquivo .o a partir de um .c
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Regra para limpar os arquivos gerados pela compilação
clean:
	rm -f $(OBJS) $(TARGET)