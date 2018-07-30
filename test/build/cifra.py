import os
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.backends import default_backend

orig = None
with open('test.bin', 'rb') as pt:
    orig = pt.read()

if orig is not None:
    mancano = len(orig) % 16
    while mancano:
        orig += bytearray([0xFF])
        mancano -= 1

    backend = default_backend()

    key = bytearray([0x57, 0xF4, 0x0F, 0xF2, 0x91, 0xBF, 0xDC, 0x8E, 0x69, 0x12, 0x1C, 0xC4, 0xE3, 0x99, 0x05, 0x05, 0xEA, 0xEA, 0x82, 0x3A, 0x15, 0x1A, 0x39, 0x6B, 0xA9, 0xFE, 0xE4, 0x68, 0x18, 0x75, 0xF4, 0x08])
    iv = os.urandom(16)

    cipher = Cipher(algorithms.AES(key), modes.CBC(iv), backend=backend)
    encryptor = cipher.encryptor()

    cif = encryptor.update(orig) + encryptor.finalize()

    with open('test.agg', 'wb') as ct:
        ct.write(iv)
        ct.write(cif)

#decryptor = cipher.decryptor()
#decryptor.update(ct) + decryptor.finalize()