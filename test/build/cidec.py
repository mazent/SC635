import sys
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.primitives import hashes, hmac
from cryptography.hazmat.backends import default_backend

def _cifra_3(ptext, iv, key):
    if isinstance(ptext, str):
        ptext = bytes(ptext.encode('ascii'))

    TESTA = b'208 - SC635'
    mancano = (len(ptext) + len(TESTA)) % 16
    if mancano:
        mancano = 16 - mancano
        testa = b'208 *'
        for _ in range(mancano):
            testa = testa + b'*'
        testa = testa + b' SC635'
        ptext = testa + ptext
    else:
        ptext = TESTA + ptext

    cipher = Cipher(algorithms.AES(bytes(key)), modes.CBC(iv), backend=default_backend())
    encryptor = cipher.encryptor()

    return encryptor.update(ptext) + encryptor.finalize()

def _cifra_2(ptext, iv, key):
    TESTA = '208 - SC635'
    mancano = (len(ptext) + len(TESTA)) % 16
    if mancano:
        mancano = 16 - mancano
        testa = '208 *'
        for _ in range(mancano):
            testa = testa + '*'
        testa = testa + ' SC635'
        ptext = testa + ptext
    else:
        ptext = TESTA + ptext

    cipher = Cipher(algorithms.AES(bytes(key)), modes.CBC(iv), backend=default_backend())
    encryptor = cipher.encryptor()

    return encryptor.update(ptext) + encryptor.finalize()


def cifra(ptext, iv, key):
    if sys.version_info.major == 3:
        return _cifra_3(ptext, iv, key)
    else:
        return _cifra_2(ptext, iv, key)

def _elimina_3(dec):
    if dec.startswith(b'208 '):
        dec = dec[4:]
        while dec[0] == 42:
            dec = dec[1:]
        if dec.startswith(b' SC635'):
            return dec[6:]
        else:
            return None
    else:
        return None

def _elimina_2(dec):
    if dec.startswith('208 '):
        dec = dec[4:]
        while dec[0] == '*':
            dec = dec[1:]
        if dec.startswith(' SC635'):
            return dec[6:]
        else:
            return None
    else:
        return None

def decifra(ctext, key):
    iv = ctext[:16]
    ctext = ctext[16:]

    cipher = Cipher(algorithms.AES(bytes(key)), modes.CBC(iv), backend=default_backend())
    decryptor = cipher.decryptor()
    dec = decryptor.update(ctext) + decryptor.finalize()

    if sys.version_info.major == 3:
        return _elimina_3(dec)
    else:
        return _elimina_2(dec)


def firma(cosa, key):
    h = hmac.HMAC(bytes(key), hashes.SHA256(), backend=default_backend())
    h.update(bytes(cosa))
    return h.finalize()

def verifica(cosa, mac, key):
    h = hmac.HMAC(bytes(key), hashes.SHA256(), backend=default_backend())
    h.update(bytes(cosa))
    try:
        h.verify(mac)
        return True
    except:
        return False
