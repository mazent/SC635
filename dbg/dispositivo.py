import sys
import struct

import prot


class DISPOSITIVO(object):

    def __init__(self, **argo):
        self.coda = None
        self.prot = prot.PROT(**argo)

    def __del__(self):
        del self.prot

    def a_posto(self):
        # Controllo una sola volta
        if self.prot is None:
            return False
        else:
            return self.prot.a_posto()

    def Chiudi(self):
        if self.prot is not None:
            self.prot.chiudi()
            del self.prot
            self.prot = None

    def Cambia(self, baud=None, tempo=None):
        if self.prot is not None:
            self.prot.cambia(baud, tempo)

    def Ripristina(self):
        if self.prot is not None:
            self.prot.ripristina()

    # ============================================
    # Varie
    # ============================================

    def dim_max(self, cmd, dati):
        return self.prot.dim_max(cmd, dati)

    def Eco(self, dati=None):
        if dati is None:
            dati = struct.pack('<I', 0xDEADBEEF)

        eco = self.prot.cmdPrmRsp(0x0000, dati, len(dati))
        if eco is None:
            return False
        else:
            return eco == dati

    # ============================================
    # Produzione
    # ============================================

    def leggi_prodotto(self):
        ns = self.prot.cmdVoidRsp(0x0100)
        if ns is not None:
            ns = ns.decode('ascii')
        return ns

    def scrivi_prodotto(self, ns):
        ns = ns.encode('ascii')
        return self.prot.cmdPrmVoid(0x0101, ns)

    def leggi_scheda(self):
        ns = self.prot.cmdVoidRsp(0x0102)
        if ns is not None:
            ns = ns.decode('ascii')
        return ns

    def scrivi_scheda(self, ns):
        ns = ns.encode('ascii')
        return self.prot.cmdPrmVoid(0x0103, ns)

    # ============================================
    # HW
    # ============================================

    def azzera_tasto(self):
        return self.prot.cmdVoidVoid(0x0200)

    def leggi_tasto(self):
        cnt = self.prot.cmdVoidRsp(0x0201, 1)
        if cnt is not None:
            cnt = cnt[0]
        return cnt

    def cavo_in_rj45(self):
        cavo = self.prot.cmdVoidRsp(0x0202, 1)
        if cavo is not None:
            cavo = cavo[0] != 0
        return cavo
