# Détecter le système d'exploitation
UNAME_S := $(shell uname -s)

# paramètre par defaut
PATH_LIB := lib/
PATH_SRCS := c/

# Paramètres par défaut pour Linux
CC := gcc
CFLAGS := -Wall -g -O0 -fno-omit-frame-pointer
LDFLAGS := -shared -fPIC
TARGET := $(PATH_LIB)libgame.so
SRC := $(PATH_SRCS)essais.c
OBJ := $(SRC:.c=.o)

# Ajuster les paramètres pour macOS
ifeq ($(UNAME_S),Darwin)
	LDFLAGS := -dynamiclib
	TARGET := $(PATH_LIB)libgame.dylib
endif

# Règle par défaut
all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

$(TARGET): $(OBJ)
	mkdir -p $(PATH_LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^


# Règle de compilation
build: $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	-./Gomoku.py
# Règle pour nettoyer les fichiers compilés
clean:
	rm -f $(TARGET) $(OBJ)

run_valgrind:
	@echo "Lancement de valgrind... 🍺";
	@valgrind --leak-check=full --show-leak-kinds=all --trace-children=yes python3 Gomoku.py > leaks.txt 2>&1
	@if grep -q "libgame.so" leaks.txt; then \
		grep "libgame.so" leaks.txt; \
	else \
		echo "No leaks of libgame.so "; \
	fi

.PHONY: all build clean
