# Détecter le système d'exploitation
UNAME_S := $(shell uname -s)

# Paramètres par défaut pour Linux
CC := gcc
CFLAGS := -Wall
LDFLAGS := -shared -fPIC
TARGET := libgame.so
SRC := c/essais.c
OBJ := $(SRC:.c=.o)

# Ajuster les paramètres pour macOS
ifeq ($(UNAME_S),Darwin)
	LDFLAGS := -dynamiclib
	TARGET := libgame.dylib
endif

# Règle par défaut
all: build run

# Règle de compilation
build: $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRC)

run: build
	python3 Gomoku.py
# Règle pour nettoyer les fichiers compilés
clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all build clean
