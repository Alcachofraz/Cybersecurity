import numpy as np


def inversa_modn(n, matriz):
    print("Matriz a inverter:", matriz)
    ret = []
    try:
        det = int(np.linalg.det(matriz))
        print("Determinate da matriz:", det)
        pow(det, -1, n)
        print("A matriz é invertível módulo", n)
    except ValueError:
        print("A matriz não é invertível módulo", n)
        return "Não existe"
    for nextList in matriz:
        aux = []
        for next in nextList:
            aux.append(pow(next, -1, n))
        ret.append(aux)
    return ret


print("Matriz invertida:", inversa_modn(18, [[6, 8],
                                             [4, 12]]))
