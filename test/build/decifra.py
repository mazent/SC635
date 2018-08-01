from __future__ import print_function

import sys
import cidec
import chiavi

"""
    Verifica del file di aggiornamento
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
            raise problema('passare il nome del file cifrato e quello in chiaro')

        agg = None
        with open(sys.argv[1], 'rb') as pt:
            agg = pt.read()

        dim = len(agg)
        if dim < 16 + 32:
            raise problema('troppo piccolo')

        cifrato = agg[:dim-32]
        firma = agg[dim-32:]

        if not cidec.verifica(cifrato, firma, chiavi.KEY_MAC):
            raise problema('firma sbagliata')

        chiaro = cidec.decifra(cifrato, chiavi.KEY_CIF)
        if chiaro is None:
            raise problema('cifratura sbagliata')

        with open(sys.argv[2], 'wb') as pt:
            pt.write(chiaro)

    except problema as err:
        print(err)
