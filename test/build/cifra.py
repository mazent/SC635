from __future__ import print_function

import os
import sys
import cidec
import chiavi

"""
    Preparazione del file per l'aggiornamento:
        1) si aggiunge una intestazione che porta la dimensione a multiplo di 16
        2) si cifra con un iv
        3) si firma (iv + cifrato)
        4) il file per l'aggiornamento e' la concatenazione: iv || cifrato || firma
"""

class problema(Exception):

    def __init__(self, msg):
        Exception.__init__(self)
        self.msg = msg

    def __str__(self):
        return self.msg


if __name__ == '__main__':
    try:
        if len(sys.argv) != 3:
            raise problema('passare il nome del file in chiaro e quello cifrato')

        chiaro = None
        with open(sys.argv[1], 'rb') as pt:
            chiaro = pt.read()

        iv = os.urandom(16)
        cifrato = cidec.cifra(chiaro, iv, chiavi.KEY_CIF)

        firma = cidec.firma(iv + cifrato, chiavi.KEY_MAC)

        with open(sys.argv[2], 'wb') as agg:
            agg.write(iv)
            agg.write(cifrato)
            agg.write(firma)

    except problema as err:
        print(err)
