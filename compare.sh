#!/bin/bash

# Définir le répertoire de base pour la recherche des fichiers .c
REPERTOIRE="c/"

# Se déplacer dans la branche contenant les fichiers à comparer initialement
# Ici, on assume que vous voulez partir de la branche 'main'
git checkout main

# Lister tous les fichiers .c dans le répertoire spécifié
# et boucler sur chaque fichier trouvé
find $REPERTOIRE -type f -name "*.c" | while read fichier; do
    echo "Comparaison pour $fichier entre main et threads:"
    # Utiliser git diff pour comparer chaque fichier entre les branches main et threads
    git --no-pager diff main threads -- "$fichier"
    echo "------------------------------------------------"
done
