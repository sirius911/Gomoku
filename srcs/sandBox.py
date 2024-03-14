import argparse
from constants import *
from game_logic import GomokuLogic
from colorama import Fore, Style

def is_gom_file(fileName):
    if not fileName.endswith(".gom"):
        raise argparse.ArgumentTypeError("Le fichier doit avoir l'extension '.gom'")
    return fileName

def main(debug, filepath, ia, threads):
    game_logic = GomokuLogic(debug=debug)
    if filepath is not None:
       if game_logic.load(filepath):
                game_logic.saved = True
                game_logic.path = filepath
    game_logic.ia = {"black":True, "white":True}
    game_logic.ia_level = int(ia)
    game_logic.threads = threads
    result = CONTINUE_GAME
    temps_total = 0
    nb_coup = 0
    while result == CONTINUE_GAME:
        print("*",end='', flush=True)
        result, t = game_logic.play_IA()
        print("+",end='', flush=True)
        temps_total += t
        nb_coup += 1
    if result == WIN_GAME:
         print(f"\nWinner : {Fore.GREEN}{game_logic.current_player}{Style.RESET_ALL}")
    moyenne = temps_total/nb_coup
    print(f"\nTemps moyen pour une partie de {Fore.BLUE}{nb_coup}{Style.RESET_ALL} coups (ia={Fore.YELLOW}{game_logic.ia_level}{Style.RESET_ALL}) Threads={Fore.BLUE if threads else Fore.MAGENTA}{threads}{Style.RESET_ALL} => {Fore.GREEN if moyenne < 1 else Fore.RED}{moyenne:.02f}{Style.RESET_ALL} s/coup")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=f"Gomoku : Gomoku 42 Game. ({VERSION})")
    parser.add_argument("-d", "--debug", action="store_true", help="Debug option.")
    parser.add_argument("load", nargs='?', type=is_gom_file, help="load file *.go", default=None)
    parser.add_argument("-i","--ia",type=int, help="Level of IA", default=1)
    parser.add_argument("-t","--threads", action="store_true", help="Ia with threads")
    
    args = parser.parse_args()
    main(args.debug, args.load, args.ia, args.threads)