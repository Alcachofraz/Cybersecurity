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

outter_disk = collections.deque(
    ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'I', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'V', 'X', 'Z', '1', '2', '3', '4'])
inner_disk = collections.deque(
    ['a', 'c', 'e', 'g', 'k', 'l', 'n', 'p', 'r', 't', 'v', 'z', '&', 'x', 'y', 's', 'o', 'm', 'q', 'i', 'h', 'f', 'd', 'b'])


def decifra_alberti(texto_cifrado, letra_chave):
    ret = ""

    # Key is lower case:
    key = letra_chave.lower()

    # Find index of key in inner_disk:
    key_index = inner_disk.index(key)

    for next in texto_cifrado:
        # Check for disks rotation (if 'next' is an 'outter_disk' character):
        try:
            pos = outter_disk.index(next)
            # Rotate disk:
            outter_disk.rotate(key_index - pos)
        except:
            # Keep decyphering:
            ret += outter_disk[inner_disk.index(next)].lower()
    return ret


print(decifra_alberti("AlvrMrqlrZsysVhvq", 'g'))
