#!/bin/bash

REPERTOIRE="c/"

git checkout $1

# Lister tous les fichiers .c dans le répertoire spécifié
# et boucler sur chaque fichier trouvé
find $REPERTOIRE -type f -name "*.c" | while read fichier; do
    echo "Comparaison pour $fichier entre $1 et $2:"
    # Utiliser git diff pour comparer chaque fichier entre les branches main et threads
    git --no-pager diff $1 $2 -- "$fichier"
    echo "------------------------------------------------"
done
