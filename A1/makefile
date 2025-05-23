# Nome do executável
TARGET = vinac

# Fontes
SRCS = main.c lz.c archive.c directory.c vinac.c

# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -g -fsanitize=address

# Regra padrão
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

# Limpeza
clean:
	rm -f $(TARGET)