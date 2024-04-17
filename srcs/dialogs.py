import tkinter as tk

def center_window(window, parent):
    window.update_idletasks()  # Met à jour la disposition de la fenêtre pour obtenir des dimensions précises
    parent_x = parent.winfo_rootx()
    parent_y = parent.winfo_rooty()
    parent_width = parent.winfo_width()
    parent_height = parent.winfo_height()
    my_width = window.winfo_width()
    my_height = window.winfo_height()
    center_x = parent_x + (parent_width - my_width) // 2
    center_y = parent_y + (parent_height - my_height) // 2
    window.geometry(f"+{center_x}+{center_y}")
    window.deiconify()  # Rend la fenêtre à nouveau visible après le centrage

def hsl_to_rgb(h, s, l):
        c = (1 - abs(2*l - 1)) * s
        x = c * (1 - abs((h / 60) % 2 - 1))
        m = l - c/2
        r = g = b = 0
        if 0 <= h < 60:
            r, g, b = c, x, 0
        elif 60 <= h < 120:
            r, g, b = x, c, 0
        elif 120 <= h < 180:
            r, g, b = 0, c, x
        elif 180 <= h < 240:
            r, g, b = 0, x, c
        elif 240 <= h < 300:
            r, g, b = x, 0, c
        elif 300 <= h < 360:
            r, g, b = c, 0, x
        r, g, b = (r + m), (g + m), (b + m)
        return f'#{int(r*255):02x}{int(g*255):02x}{int(b*255):02x}'

class CustomDialog(tk.Toplevel):

    def __init__(self, parent, title="Dialog", message="Message", alert_type="info"):
        super().__init__(parent)
        self.title(title)

        # Configuration de la fenêtre de dialogue
        self.transient(parent)  # La dialogue est liée à la fenêtre parente
        
        self.configure(background=self.get_background_color(alert_type))
        self.message = tk.Label(self, text=message, background=self.get_background_color(alert_type), foreground=self.get_foreground_color(alert_type))
        self.message.pack(pady=20)
        self.button_frame = tk.Frame(self, background=self.get_background_color(alert_type))
        self.button_frame.pack(fill=tk.X, padx=20, pady=10)

        self.ok_button = tk.Button(self.button_frame, text="OK", command=self.destroy)
        self.ok_button.pack(side=tk.RIGHT)
        
        center_window(self, parent)  # Centrez la fenêtre de dialogue
        
        self.grab_set()  # La fenêtre de dialogue capte l'attention après avoir été rendue visible et centrée
        self.wait_window(self)  # Attendre que la fenêtre de dialogue soit fermée


    def get_background_color(self, alert_type):
        colors = {
            "info": "#D9EDF7",
            "warning": "#FCF8E3",
            "error": "#F2DEDE"
        }
        return colors.get(alert_type, "#D9EDF7")  # Retourne bleu clair par défaut pour info

    def get_foreground_color(self, alert_type):
        colors = {
            "info": "#31708F",
            "warning": "#8A6D3B",
            "error": "#A94442"
        }
        return colors.get(alert_type, "#31708F")  # Retourne bleu foncé par défaut pour info

class EndGameDialog(tk.Toplevel):
    def __init__(self, parent, message, replay_callback, quit_callback):
        super().__init__(parent)
        self.replay_callback = replay_callback
        self.quit_callback = quit_callback
        self.title("Fin de la partie")
        self.geometry("300x150")  # Ajustez la taille selon vos besoins
        self.message_label = tk.Label(self, text=message)
        self.message_label.pack(pady=10)
        
        self.replay_button = tk.Button(self, text="Rejouer", command=self.replay)
        self.replay_button.pack(side=tk.LEFT, padx=10, pady=10)
        
        self.quit_button = tk.Button(self, text="Quitter", command=self.quit)
        self.quit_button.pack(side=tk.RIGHT, padx=10, pady=10)

        # self.protocol("WM_DELETE_WINDOW", self.destroy)  # Gérer la fermeture de la fenêtre
        self.transient(parent)  # Assurer que la boîte de dialogue est modale

        center_window(self, parent)
        self.grab_set()  # Capturer tous les événements vers cette fenêtre
        self.wait_window(self)  # Attendre que la fenêtre soit fermée avant de continuer

    def replay(self):
        # Appel de la fonction callback externe pour rejouer
        self.replay_callback()
        self.destroy()  # Ferme la fenêtre de dialogue

    def quit(self):
        # Appel de la fonction callback externe pour quitter
        self.quit_callback()
        self.destroy()  # Ferme la fenêtre de dialogue