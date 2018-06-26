#! /usr/bin/env python

"""
    Per non bloccare la grafica viene creato un task
    che aspetta i comandi e li esegue
"""

from __future__ import print_function

import threading

import gui_support


class taskEsecutore(threading.Thread):

    def __init__(self, codaEXE, codaGUI):
        threading.Thread.__init__(self)

        self.coda_exe = codaEXE
        self.coda_gui = codaGUI

        self.dispo = None

        self.comando = {
            'ecoFinito': self._eco_limite,
            'ecoInfinito': self._eco_8,
            'ecoFinePerErrore': self._eco_fine_x_errore,

            'eco': self._eco,

            'leggi_cp': self._cod_prod_l,
            'scrivi_cp': self._cod_prod_s,
            'leggi_cs': self._cod_schd_l,
            'scrivi_cs': self._cod_schd_s
        }

    def run(self):
        while True:
            lavoro = self.coda_exe.get()
            if "esci" == lavoro[0]:
                break
            elif "Dispositivo" == lavoro[0]:
                self.dispo = lavoro[1]
            elif not lavoro[0] in self.comando:
                pass
            else:
                self.comando[lavoro[0]](lavoro)

    def _manda_alla_grafica(self, x, y=None):
        if y is None:
            self.coda_gui.put((x))
        else:
            self.coda_gui.put((x, y))

    # Invocati dall'eco
    def _eco_8(self, prm):
        prm[1].ecoInfinito(self.dispo)

    def _eco_limite(self, prm):
        prm[1].ecoFinito(prm[2], self.dispo)

    def _eco_fine_x_errore(self, prm):
        prm[1].ecoFinePerErrore(prm[2], self.dispo)

    # ========== VARIE ========================================================

    def _eco(self, dummy):
        if self.dispo.Eco():
            gui_support.Messaggio.set("Eco: OK")
        else:
            gui_support.Messaggio.set("Eco: ERRORE")

    # ========== PRODUZIONE ===================================================

    def _cod_prod_l(self, _):
        ns = self.dispo.leggi_prodotto()
        if ns is None:
            gui_support.Messaggio.set("Cod. prodotto: ERRORE")
        else:
            gui_support.cp.set(ns)
            gui_support.Messaggio.set("Cod. prodotto: OK")

    def _cod_prod_s(self, prm):
        if self.dispo.scrivi_prodotto(prm[1]):
            gui_support.Messaggio.set("Cod. prodotto: OK")
        else:
            gui_support.Messaggio.set("Cod. prodotto: ERRORE")

    def _cod_schd_l(self, _):
        ns = self.dispo.leggi_scheda()
        if ns is None:
            gui_support.Messaggio.set("Cod. scheda: ERRORE")
        else:
            gui_support.cs.set(ns)
            gui_support.Messaggio.set("Cod. scheda: OK")

    def _cod_schd_s(self, prm):
        if self.dispo.scrivi_scheda(prm[1]):
            gui_support.Messaggio.set("Cod. scheda: OK")
        else:
            gui_support.Messaggio.set("Cod. scheda: ERRORE")