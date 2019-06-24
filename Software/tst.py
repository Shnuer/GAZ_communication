import numpy as np


class tst(object):
    def __init__(self, a = 5):
        print(a)



first = tst()
second = tst(6)

x = (np.uint8(5),np.uint8(6),np.uint8(7))
pkg = bytes([ord('#'),
    np.uint8(34), 
                            np.uint8(63), 
    np.uint8(129) 
    ])

pkg1 = bytes([ord('#'), np.uint8(34),np.uint8(63), np.uint8(129) ])
print(type(x[1]))
print(pkg)
print(pkg1)