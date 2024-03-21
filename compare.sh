#!/bin/bash

REPERTOIRE="c/"

git checkout main

# Lister tous les fichiers .c dans le répertoire spécifié
# et boucler sur chaque fichier trouvé
find $REPERTOIRE -type f -name "*.c" | while read fichier; do
    echo "Comparaison pour $fichier entre main et threads:"
    # Utiliser git diff pour comparer chaque fichier entre les branches main et threads
    git --no-pager diff main threads -- "$fichier"
    echo "------------------------------------------------"
done
