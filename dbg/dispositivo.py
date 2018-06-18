import sys
import struct

import prot

NOME_UART = None
if sys.platform.startswith("win32"):
    NOME_UART = "COM41"
else:
    NOME_UART = "/dev/ttyUSB1"


class DISPOSITIVO(object):

    def __init__(self, **argo):
        self.coda = None
        try:
            self.prot = prot.PROT(porta=argo['uart'])
        except KeyError:
            try:
                self.prot = prot.PROT(vid=argo['vid'], pid=argo['pid'])
            except KeyError:
                self.prot = None

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


if __name__ == '__main__':
    d = DISPOSITIVO(uart=NOME_UART)
    if d.a_posto():
        print(d.Eco())
    d.Chiudi()