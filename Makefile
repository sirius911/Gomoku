# Détecter le système d'exploitation
UNAME_S := $(shell uname -s)

# paramètre par defaut
PATH_LIB := lib/
PATH_SRCS := c/

SRC := utils.c game_logic.c minmax.c sandBox.c threads.c heuristic.c evaluator.c
SRCS := $(addprefix $(PATH_SRCS),$(SRC))
OBJ := $(SRCS:.c=.o)

# Paramètres par défaut pour Linux
CC := gcc
CFLAGS := -Wall -g -O3 -march=native -flto -funroll-loops -fno-omit-frame-pointer -fPIC
#CFLAGS := -Wall -g -O0 -fno-omit-frame-pointer -fPIC

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

run: $(TARGET)
	-python3 srcs/Gomoku.py

# Règle pour nettoyer les fichiers compilés
clean:
	rm -f $(TARGET) $(OBJ) main

re: clean
	make all

# Règle pour compiler et exécuter le programme de test
test: c/main.c $(TARGET)
	$(CC) $(CFLAGS) -o main c/main.c -L$(PATH_LIB) -lgame -Wl,-rpath,$(PATH_LIB)


.PHONY: all build clean re test
