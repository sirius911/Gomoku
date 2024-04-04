import argparse
import os
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

def file_game(nom_base, extension = "gom"):
    if not nom_base.startswith('parties/'):
        nom_complet = 'parties/' + nom_base
    else:
        nom_complet = nom_base

    if not nom_complet.endswith(f'.{extension}'):
        nom_complet += f'.{extension}'
    return nom_complet

def file_name(nom_base, extension):

    if not nom_base.startswith('stats/'):
        nom_complet = 'stats/' + nom_base
    else:
        nom_complet = nom_base

    if not nom_complet.endswith(f'.{extension}'):
        nom_complet += f'.{extension}'

    # Assurer que le répertoire 'stats/' existe
    os.makedirs(os.path.dirname(nom_complet), exist_ok=True)

    return nom_complet

def prepare_csv(csv_file):
    entetes = ["partie", "Nombre de Coups", "Niveau IA", "Threads", "moyenne", "max", "min", "total"]
    ligne_entetes = ','.join(entetes) + '\n'
    if not os.path.exists(csv_file):
        with open(csv_file, 'w', newline='') as fichier:
            fichier.write(ligne_entetes)
    elif os.path.getsize(csv_file) == 0:
        with open(csv_file, 'a', newline='') as fichier:
            fichier.write(ligne_entetes)

def play_stat(fileName, ia):
    game_logic = init_game(fileName, ia, False)
    game_logic.stat = True
    result = CONTINUE_GAME
    while result == CONTINUE_GAME:
        result, _ = game_logic.play_IA()

def create_log(fileName,ia):
    if fileName is None:
        fileName = ""
        nom_fichier = f"stats/level{ia}.log"
    else:
        nom_fichier = f"stats/{os.path.splitext(os.path.basename(fileName))[0]}{ia}.log"
    print(f"Création de {nom_fichier} ... ", end='', flush=True)
    commande = f"python3 srcs/sandBox.py -c -i {ia} {fileName}"
    with open(nom_fichier, 'w') as fichier_log:
        subprocess.run(commande, shell=True, stdout=fichier_log, stderr=subprocess.STDOUT)
    print("Terminé.", flush=True)
    time.sleep(1)

def compter_points_par_bloc(fileName, ia):
    if fileName is None:
        fileName = ""
        nom_fichier = f"stats/level{ia}.log"
    else:
        nom_fichier = f"stats/{os.path.splitext(os.path.basename(fileName))[0]}{ia}.log"
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

def graph(tab_time, coups, nb_coup, threads, fileName):
    plt.plot(coups, tab_time, marker='o')  # 'o' pour afficher les points de données

    # Ajout de titres aux axes et au graphique
    plt.xlabel('Nombre de tests')
    plt.ylabel('Temps (s)')
    thread = "\n[Threads]" if threads else ""
    name  = os.path.splitext(os.path.basename(fileName))[0] if fileName is not None else ""
    plt.title(f'Partie de {nb_coup} coup(s) {name}\nTemps en fonction du nombre de tests{thread}')

    # Affichage du graphique
    plt.show()

def play_game(game_logic, filepath, log_file, save):
    if filepath is None:
        filepath = f"Normale{game_logic.ia_level}"
    result = CONTINUE_GAME
    temps_total = 0
    nb_coup = 0
    max_time = 0
    min_time = float('inf')
    tab_time =[]
    start_time = time.time()
    try:
        while result == CONTINUE_GAME:
            print(f"{game_logic.current_player[0]}",end='', flush=True)
            result, t = game_logic.play_IA()
            tab_time.append(t)
            print(".",end='', flush=True)
            temps_total += t
            nb_coup += 1
            max_time = max(max_time, t)
            min_time = min(min_time, t)
        end_time = time.time()
    except KeyboardInterrupt:
        end_time = time.time()
        result = INTERRUPTED
    if result == WIN_GAME:
         print(f"\nWinner : {Fore.GREEN}{game_logic.current_player}{Style.RESET_ALL}")
    elif result == INTERRUPTED:
        return None, None, result
    time.sleep(1)
    moyenne = temps_total/nb_coup
    play_time = end_time - start_time
   
    if log_file is not None:
        csv_file = file_name(log_file, "csv")
        log_file = file_name(log_file, "log")
        gom_file = file_game(filepath, "gom")

        texte = (f"Temps moyen pour une partie de {nb_coup} coups (ia={game_logic.ia_level}) Threads={game_logic.threads} => {moyenne:.02f} s/coup Max = {max_time:.02f}s Min = {min_time:.02f}s durée partie = {play_time:.02f}s\n")
        with open(log_file, 'a') as file:
            file.write(texte)
        prepare_csv(csv_file)
        ligne_csv = f"{filepath},{nb_coup},{game_logic.ia_level},{game_logic.threads},{moyenne:.02f},{max_time:.02f},{min_time:.02f},{play_time:.02f}\n"
        with open(csv_file, 'a') as file_csv:
            file_csv.write(ligne_csv)

    else:
        print(f"\nTemps moyen pour une partie de {Fore.BLUE}{nb_coup}{Style.RESET_ALL} coups (ia={Fore.YELLOW}{game_logic.ia_level}{Style.RESET_ALL}) Threads={Fore.BLUE if game_logic.threads else Fore.MAGENTA}{game_logic.threads}{Style.RESET_ALL} => {Fore.GREEN if moyenne < 1 else Fore.RED}{moyenne:.02f}{Style.RESET_ALL} s/coup Max = {Fore.RED}{max_time:.02f}{Style.RESET_ALL}s Min = {Fore.GREEN}{min_time:.02f}{Style.RESET_ALL}s durée partie = {play_time:.02f}s")
        gom_file = file_game(f"normal{game_logic.ia_level}")
    if save:
        game_logic.save(gom_file)
    return tab_time, nb_coup, result

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

def main(filepath, ia, threads, graphic, log_file, save):
    if graphic:
        create_log(filepath, ia)
    game_logic = init_game(filepath, ia, threads)
    print(f"{''if filepath is None else filepath}Partie en cours")
    tab_time, nb_coup, result = play_game(game_logic, filepath ,log_file, save)
    if result == INTERRUPTED:
        print("Interrompue")
    else:
        print("Terminée")
    if graphic:
        coups = compter_points_par_bloc(filepath, ia)
        graph(tab_time, coups, nb_coup, threads, filepath)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=f"Gomoku : Gomoku 42 Game. ({VERSION})")
    parser.add_argument("load", nargs='?', type=is_gom_file, help="load file *.go", default=None)
    parser.add_argument("-i","--ia",type=int, help="Level of IA", default=1)
    parser.add_argument("-t","--threads", action="store_true", help="Ia with threads")
    parser.add_argument("-c","--count", action="store_true", help="To count only the test stone")
    parser.add_argument("-f", "--file", type=str, help="Name of log file")
    parser.add_argument("-g","--graph", action="store_true", help="to graph")
    parser.add_argument("-s","--save", action="store_true", help="to save the game in partie/ *..gom")
    
    args = parser.parse_args()
    if args.count and not args.threads:
        #  On crée le fichier level.log
        play_stat(args.load,args.ia)
        #  new game
        sys.exit(0)
    main(args.load, args.ia, args.threads, args.graph, args.file, args.save)