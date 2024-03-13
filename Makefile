# Détecter le système d'exploitation
UNAME_S := $(shell uname -s)

# paramètre par defaut
PATH_LIB := lib/
PATH_SRCS := c/

SRC := utils.c game_logic.c minmax.c sandBox.c
SRCS := $(addprefix $(PATH_SRCS),$(SRC))
OBJ := $(SRCS:.c=.o)

# Paramètres par défaut pour Linux
CC := gcc
CFLAGS := -Wall -g -O0 -fno-omit-frame-pointer -fPIC
# Définition des flags d'optimisation
OPTI_FLAGS := -O3 -march=native -flto -funroll-loops
LDFLAGS := -shared -fPIC
TARGET := $(PATH_LIB)libgame.so



# Ajuster les paramètres pour macOS
ifeq ($(UNAME_S),Darwin)
	LDFLAGS := -dynamiclib
	TARGET := $(PATH_LIB)libgame.dylib
endif

# Règle par défaut
all: $(TARGET)

# Compilation des fichiers objet
$(PATH_SRCS)%.o: $(PATH_SRCS)%.c
	$(CC) $(CFLAGS) -c $< -o $@


# Construction de la bibliothèque à partir des fichiers objet
$(TARGET): $(OBJ)
	mkdir -p $(PATH_LIB)
	$(CC) $(LDFLAGS) -o $@ $^
# $(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^


# Règle de compilation
build: $(SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRCS)

opti: CFLAGS = -O3 -march=native -flto -funroll-loops -fPIC
opti: $(TARGET)

run: $(TARGET)
	-srcs/Gomoku.py
# Règle pour nettoyer les fichiers compilés
clean:
	rm -f $(TARGET) $(OBJ)

re: clean
	make all

run_valgrind:
	@echo "Lancement de valgrind... 🍺";
	@valgrind --leak-check=full --show-leak-kinds=all --trace-children=yes python3 Gomoku.py > leaks.txt 2>&1
	@if grep -q "libgame.so" leaks.txt; then \
		grep "libgame.so" leaks.txt; \
	else \
		echo "No leaks of libgame.so "; \
	fi

.PHONY: all build clean re
