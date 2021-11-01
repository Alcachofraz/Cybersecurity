import numpy as np


def imprimir_matriz(matriz):
    print('\n'.join([''.join(['{:4}'.format(item) for item in row])
                     for row in matriz]))


def inversa_modn(mod, matriz):
    print("Matriz a inverter:")
    imprimir_matriz(matriz)
    print()
    matriz_invertida = []  # Onde será colocada a matriz invertida
    try:
        # Verificar se a matriz é invetível módulo 'mod':
        det = int(np.linalg.det(matriz))  # Calcular o determinante da matriz.
        print("Determinate da matriz a inverter:", det)
        print()

        # Para verificar se a matriz é invertível, verifica-se se o seu determinante
        # é invertível. A linha a seguir calcula o inverso do determinante módulo
        # 'mod', ou levanta uma excepção caso não seja possível:
        pow(det, -1, mod)
        print("A matriz é invertível módulo", mod)
        print()

        # Tendo em conta que a matriz a inverter consiste numa lista de listas,
        # para inverter elemento a elemento, percorrem-se todas as listas horizontais:
        for nextList in matriz:
            # Onde serão colocados os valores invertidos da lista horizontal atual.
            aux = []

            # Percorrer todos os elementos da lista horizontal atual:
            for next in nextList:
                # Adicionar elemento invertido a 'aux'.
                aux.append(pow(next, -1, mod))

            # Adicionar lista horizontal com elementos invertidos a 'ret'.
            matriz_invertida.append(aux)

        # Imprimir matriz invertida:
        print("Matriz invertida:")
        imprimir_matriz(matriz_invertida)
    except ValueError:
        # Caso o determinante não existe
        print("A matriz não é invertível módulo", mod)


inversa_modn(17, [[6, 8],
                  [4, 12]])

print("--------------------------------------------")

inversa_modn(3, [[1, 2],
                 [2, 1]])

print("--------------------------------------------")

# Como 23 é número primo, todos os naturais inferiores
# a 23 são invertíveis módulo 23.
inversa_modn(23, [[1, 5, 4, 17],
                  [11, 7, 22, 20],
                  [2, 9, 16, 12],
                  [15, 6, 8, 13]])

print("--------------------------------------------")

# Não consegue gerar a matriz inversa porque 9 não é
# invertível módulo 18.
inversa_modn(18, [[5, 11],
                  [7, 9]])
