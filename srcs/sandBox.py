import argparse
import subprocess
import sys
import time
from constants import *
from game_logic import GomokuLogic
from colorama import Fore, Style
import matplotlib.pyplot as plt

def is_gom_file(fileName):
    if not fileName.endswith(".gom"):
        raise argparse.ArgumentTypeError("Le fichier doit avoir l'extension '.gom'")
    return fileName

def play_stat(fileName, ia):
    game_logic = init_game(fileName, ia, False)
    game_logic.stat = True
    result = CONTINUE_GAME
    while result == CONTINUE_GAME:
        result, _ = game_logic.play_IA()

def create_log(fileName,ia):
    nom_fichier = f"stats/level{ia}.log"
    if fileName is None:
        fileName = ""
    print(f"Création de {nom_fichier}... ", end='', flush=True)
    commande = f"python3 srcs/sandBox.py -c -i {ia} {fileName}"
    with open(nom_fichier, 'w') as fichier_log:
        subprocess.run(commande, shell=True, stdout=fichier_log, stderr=subprocess.STDOUT)
    print("Terminé.", flush=True)
    time.sleep(1)

def compter_points_par_bloc(ia):
    nom_fichier = f"stats/level{ia}.log"
    resultats = []  # Liste pour stocker les résultats de chaque bloc
    compteur_points = 0  # Compteur pour les points du bloc actuel
    dans_un_bloc = False  # Indicateur de la présence dans un bloc
    nb_b = 0
    try:
        print(f"Ouverture de {nom_fichier}")
        with open(nom_fichier, 'r') as fichier:
            for ligne in fichier:
                ligne = ligne.strip()  # Nettoyer les espaces et sauts de ligne
                if ligne == "#":  # Détecter les marqueurs de bloc
                    nb_b += 1
                    # Si on était déjà dans un bloc, sauvegarder le résultat et réinitialiser le compteur
                    if dans_un_bloc:
                        resultats.append(compteur_points)
                        compteur_points = 0  # Réinitialiser pour le nouveau bloc
                    else:
                        dans_un_bloc = True  # Activer la lecture du bloc
                    continue
                
                # Compter les points si dans un bloc
                if dans_un_bloc:
                    compteur_points += ligne.count('.')

        # Ajouter le dernier bloc s'il existe
        if compteur_points > 0:
            resultats.append(compteur_points)

    except FileNotFoundError:
        print(f"Le fichier '{nom_fichier}' n'a pas été trouvé.")
        return
    except Exception as e:
        print(f"Une erreur est survenue lors de l'analyse du fichier : {e}")
        return
    return resultats

def graph(tab_time, coups, nb_coup, threads):
    plt.plot(coups, tab_time, marker='o')  # 'o' pour afficher les points de données

    # Ajout de titres aux axes et au graphique
    plt.xlabel('Nombre de tests')
    plt.ylabel('Temps (s)')
    thread = "\n[Threads]" if threads else ""
    plt.title(f'Partie de {nb_coup} coup(s)\nTemps en fonction du nombre de tests{thread}')

    # Affichage du graphique
    plt.show()

def play_game(game_logic):
    result = CONTINUE_GAME
    temps_total = 0
    nb_coup = 0
    max_time = 0
    min_time = float('inf')
    tab_time =[]
    while result == CONTINUE_GAME:
        print(f"{game_logic.current_player[0]}",end='', flush=True)
        result, t = game_logic.play_IA()
        tab_time.append(t)
        print(".",end='', flush=True)
        temps_total += t
        nb_coup += 1
        max_time = max(max_time, t)
        min_time = min(min_time, t)
    if result == WIN_GAME:
         print(f"\nWinner : {Fore.GREEN}{game_logic.current_player}{Style.RESET_ALL}")
    moyenne = temps_total/nb_coup
    print(f"\nTemps moyen pour une partie de {Fore.BLUE}{nb_coup}{Style.RESET_ALL} coups (ia={Fore.YELLOW}{game_logic.ia_level}{Style.RESET_ALL}) Threads={Fore.BLUE if game_logic.threads else Fore.MAGENTA}{game_logic.threads}{Style.RESET_ALL} => {Fore.GREEN if moyenne < 1 else Fore.RED}{moyenne:.02f}{Style.RESET_ALL} s/coup Max = {Fore.RED}{max_time:.02f}{Style.RESET_ALL}s Min = {Fore.GREEN}{min_time:.02f}{Style.RESET_ALL}s")
    return tab_time, nb_coup

def init_game(filepath, ia, threads):
    game_logic = GomokuLogic(debug=False)
    if filepath is not None:
       if game_logic.load(filepath):
                game_logic.saved = True
                game_logic.path = filepath
    game_logic.ia = {"black":True, "white":True}
    game_logic.ia_level = int(ia)
    game_logic.threads = threads
    return game_logic

def main(filepath, ia, threads):
    create_log(filepath, ia)
    game_logic = init_game(filepath, ia, threads)
    print(f"{''if filepath is None else filepath}Partie en cours")
    tab_time, nb_coup = play_game(game_logic)
    print("Terminée")
    coups = compter_points_par_bloc(ia)
    graph(tab_time, coups, nb_coup, threads)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=f"Gomoku : Gomoku 42 Game. ({VERSION})")
    parser.add_argument("load", nargs='?', type=is_gom_file, help="load file *.go", default=None)
    parser.add_argument("-i","--ia",type=int, help="Level of IA", default=1)
    parser.add_argument("-t","--threads", action="store_true", help="Ia with threads")
    parser.add_argument("-c","--count", action="store_true", help="To count only the test stone")
    
    args = parser.parse_args()
    if args.count and not args.threads:
        #  On crée le fichier level.log
        play_stat(args.load,args.ia)
        #  new game
        sys.exit(0)
    main(args.load, args.ia, args.threads)