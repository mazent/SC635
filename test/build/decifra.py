import os
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.backends import default_backend

agg = None
with open('test.agg', 'rb') as pt:
    agg = pt.read()

if agg is not None:
	iv = agg[:16]
	agg = agg[16:]

    backend = default_backend()

    key = bytearray([0x57, 0xF4, 0x0F, 0xF2, 0x91, 0xBF, 0xDC, 0x8E, 0x69, 0x12, 0x1C, 0xC4, 0xE3, 0x99, 0x05, 0x05, 0xEA, 0xEA, 0x82, 0x3A, 0x15, 0x1A, 0x39, 0x6B, 0xA9, 0xFE, 0xE4, 0x68, 0x18, 0x75, 0xF4, 0x08])
    key = bytes(key)

    cipher = Cipher(algorithms.AES(key), modes.CBC(iv), backend=backend)
    
	decryptor = cipher.decryptor()
	orig = decryptor.update(agg) + decryptor.finalize()	
	
    with open('test.agg.bin', 'wb') as ct:
        ct.write(orig)

