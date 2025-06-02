# Diretórios
PREFIX = $(HOME)/allegro_install
PKG_CONFIG_PATH := $(PREFIX)/lib/pkgconfig

# Ferramentas
CC = gcc
PKG_CONFIG = PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config

# Flags
CFLAGS := -Wall -Wextra -O2 $(shell $(PKG_CONFIG) --cflags allegro-5 allegro_font-5 allegro_image-5 allegro_primitives-5 allegro_ttf-5 allegro_audio-5 allegro_acodec-5 allegro_main-5)
LDFLAGS := $(shell $(PKG_CONFIG) --libs allegro-5 allegro_font-5 allegro_image-5 allegro_primitives-5 allegro_ttf-5 allegro_audio-5 allegro_acodec-5 allegro_main-5)

# Alvo final
TARGET = jogo

# Fontes
SRC = main.c

# Regra padrão
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
