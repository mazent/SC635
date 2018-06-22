#! /usr/bin/env python

"""
    Implementa i metodi ereditati dalla grafica
"""

import sys
import time

try:
    # 2
    import Queue as coda
    import tkFileDialog as dialogo
except ImportError:
    # 3
    import queue as coda
    import tkinter.filedialog as dialogo

import gui
import gui_support

import esegui
import eco
import dispositivo
import utili

NOME_UART = None
if sys.platform.startswith("win32"):
    NOME_UART = "COM"
else:
    NOME_UART = "/dev/ttyUSB"

INDIRIZZO_IP = "192.168.4.1"

TAB_CHIUSA = {1: False }
TAB_APERTA = {1: True }


class SC635(gui.New_Toplevel_1):
    def __init__(self, master=None):
        self.master = master
        gui.New_Toplevel_1.__init__(self, master)

        gui_support.portaSeriale.set(NOME_UART)
        gui_support.indirizzoIP.set(INDIRIZZO_IP)

        self._imposta_tab(TAB_CHIUSA)

        self.dispo = None

        self.crome = None

        # Code per la comunicazione fra grafica e ciccia
        self.codaEXE = coda.Queue()
        self.codaGUI = coda.Queue()

        self.task = esegui.taskEsecutore(self.codaEXE, self.codaGUI)
        self.task.start()

        self.eco = eco.ECO(self.Button3,
                           gui_support.numEco,
                           gui_support.dimEco,
                           gui_support.Messaggio,
                           self.TProgressbar1,
                           self.codaEXE)

        # Comandi dall'esecutore
        self.cmd = {
        }
        self._esegui_GUI()

    def __del__(self):
        pass

    def chiudi(self):
        self.codaEXE.put(("esci",))
        self.task.join()

        if self.dispo is not None:
            self.dispo.Chiudi()
            self.dispo = None

    def _imposta_tab(self, lista):
        for tab in lista:
            stato = 'disabled'
            if lista[tab]:
                stato = 'normal'

            self.TNotebook1.tab(tab, state=stato)

    def _esegui_GUI(self):
        try:
            msg = self.codaGUI.get(0)

            if msg[0] in self.cmd:
                if 2 == len(msg):
                    self.cmd[msg[0]](msg[1])
                else:
                    self.cmd[msg[0]]()
        except coda.Empty:
            pass

        self.master.after(300, self._esegui_GUI)

    #========== SERIALE ======================================================

    def apriFTDI(self):
        if self.dispo is None:
            self.dispo = dispositivo.DISPOSITIVO(vid='0403', pid='6010')
            if not self.dispo.a_posto():
                del self.dispo
                self.dispo = None
            else:
                self.codaEXE.put(("Dispositivo", self.dispo))

                self.Button47['text'] = 'Mollala'

                self.Entry1['state'] = 'readonly'
                self.Button1['state'] = 'disabled'
                self.Entry4['state'] = 'readonly'
                self.Button4['state'] = 'disabled'

                self._imposta_tab(TAB_APERTA)
        else:
            self.dispo.Chiudi()
            self.dispo = None
            self.codaEXE.put(("Dispositivo", self.dispo))

            self.Button47['text'] = 'Usa FTDI'

            self.Button1['state'] = 'normal'
            self.Entry1['state'] = 'normal'
            self.Button4['state'] = 'normal'
            self.Entry4['state'] = 'normal'

            self._imposta_tab(TAB_CHIUSA)

    def apriSeriale(self):
        if self.dispo is None:
            porta = gui_support.portaSeriale.get()
            if porta is None:
                gui_support.portaSeriale.set(NOME_UART)
            elif 0 == len(porta):
                gui_support.portaSeriale.set(NOME_UART)
            else:
                self.dispo = dispositivo.DISPOSITIVO(porta=porta)
                if not self.dispo.a_posto():
                    del self.dispo
                    self.dispo = None
                    gui_support.portaSeriale.set(NOME_UART)
                else:
                    self.codaEXE.put(("Dispositivo", self.dispo))

                    self.Entry1['state'] = 'readonly'
                    self.Button1['text'] = 'Mollala'

                    self.Button47['state'] = 'disabled'
                    self.Entry4['state'] = 'readonly'
                    self.Button4['state'] = 'disabled'

                    self._imposta_tab(TAB_APERTA)
        else:
            self.dispo.Chiudi()
            self.dispo = None
            self.codaEXE.put(("Dispositivo", self.dispo))

            self.Button1['text'] = 'Usa questa'
            self.Entry1['state'] = 'normal'

            self.Button47['state'] = 'normal'
            self.Button4['state'] = 'normal'
            self.Entry4['state'] = 'normal'

            gui_support.portaSeriale.set(NOME_UART)
            self._imposta_tab(TAB_CHIUSA)

    def apriSocket(self):
        if self.dispo is None:
            indip = gui_support.indirizzoIP.get()
            if indip is None:
                gui_support.indirizzoIP.set(INDIRIZZO_IP)
            elif len(indip) < 7:
                gui_support.indirizzoIP.set(INDIRIZZO_IP)
            else:
                self.dispo = dispositivo.DISPOSITIVO(indip=indip)
                if not self.dispo.a_posto():
                    del self.dispo
                    self.dispo = None
                    gui_support.indirizzoIP.set(INDIRIZZO_IP)
                else:
                    self.codaEXE.put(("Dispositivo", self.dispo))

                    self.Entry4['state'] = 'readonly'
                    self.Button4['text'] = 'Mollalo'

                    self.Button47['state'] = 'disabled'
                    self.Entry1['state'] = 'readonly'
                    self.Button1['state'] = 'disabled'

                    self._imposta_tab(TAB_APERTA)
        else:
            self.dispo.Chiudi()
            self.dispo = None
            self.codaEXE.put(("Dispositivo", self.dispo))

            self.Button4['text'] = 'TCP'
            self.Entry4['state'] = 'normal'

            self.Button47['state'] = 'normal'
            self.Button1['state'] = 'normal'
            self.Entry1['state'] = 'normal'

            gui_support.indirizzoIP.set(INDIRIZZO_IP)
            self._imposta_tab(TAB_CHIUSA)

    # ========== VARIE ========================================================

    def Eco(self):
        gui_support.Messaggio.set("Aspetta ...")
        self.codaEXE.put(("eco",))

    def ecoProva(self, dummy):
        self.eco.Bottone()



if __name__ == '__main__':
    ROOT = gui.Tk()
    ROOT.title('Test SC635')
    ROOT.geometry("603x581+292+128")

    gui_support.set_Tk_var()

    gui_support.Messaggio.set("Pronto!")

    FINESTRA = SC635(ROOT)
    gui_support.init(ROOT, FINESTRA)
    ROOT.mainloop()

    FINESTRA.chiudi()
