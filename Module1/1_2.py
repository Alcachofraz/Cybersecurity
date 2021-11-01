# a)
#   Chave = "g"
#   Texto limpo: "cifraoriginal"
#   Texto cifrado: "Deplyazypnpvar"
#
#   Ao começar o texto cifrado com "D" indica-se que
#   os discos alinham em "g" -> "D", isto é, ao "D"
#   cifrado corresponde o "g" limpo.
#
# b)
#   Chave = "g"
#   Texo cifrado: "AlvrMrqlrZsysVhvq"
#   Texto limpo: "cifraoriginal"

import collections


# Para os discos, utilizar-se-á o tipo collections.deque, de forma a
# tirar partido da função rotate().

# Disco exterior:
disco_exterior = collections.deque(
    ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'I', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'V', 'X', 'Z', '1', '2', '3', '4'])
# Disco interior:
disco_interior = collections.deque(
    ['a', 'c', 'e', 'g', 'k', 'l', 'n', 'p', 'r', 't', 'v', 'z', '&', 'x', 'y', 's', 'o', 'm', 'q', 'i', 'h', 'f', 'd', 'b'])


def decifra_alberti(texto_cifrado, letra_chave):
    # String onde será colocado o texto decifrado (texto limpo)
    texto_decifrado = ""

    # A chave é letra minúscula (disco interior):
    letra_chave = letra_chave.lower()

    # Encontrar o índice da chave no disco interior:
    letra_chave_ind = disco_interior.index(letra_chave)

    for next in texto_cifrado:
        # Verificar se houveram alterações na rotação dos discos (isto é,
        # se 'next' é um caractere contido em 'disco_exterior'):
        try:
            # A próxima linha levanta uma excepção, caso 'next' não exista em
            # 'disco_exterior'. Isso significa que 'next' se trata de um
            # caractere contido em 'disco_interior' (para decifrar):
            pos = disco_exterior.index(next)

            # Rodar o disco exterior até a letra chave coincidir com 'next':
            disco_exterior.rotate(letra_chave_ind - pos)
        except:
            # Continuar a decifrar:
            texto_decifrado += disco_exterior[disco_interior.index(
                next)].lower()
    return texto_decifrado


print(decifra_alberti("AlvrMrqlrZsysVhvq", 'g'))
print(decifra_alberti("FxDg2cNp", 'n'))
print(decifra_alberti("Etzkpbc&fAtlrc", 'c'))
print(decifra_alberti("I&m&ssrz&PkhvgnepGvxrtmqe", 'r'))
