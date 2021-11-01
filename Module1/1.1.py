import math


def citala_espartana(texto_claro: str, n: int):
    # Verificar espessura da cítala
    if n < 2:
        return 'Espessura da cítala incorreta'

    # Remover espaços e converter para maiúsculas
    texto_claro = texto_claro.replace(' ', '').upper()

    # Obter comprimento do texto
    texto_claro_size = len(texto_claro)

    # Obter largura da cítala (dividir o texto em 'n' partes)
    citala_size = math.ceil(texto_claro_size/n)

    # Percorrer do texto como se fosse uma matriz dividida em 'n' linhas, de cima para baixo, da esquerda para a direita
    texto_cifrado = ''
    for i in range(citala_size):
        for j in range(n):
            k = i + j * citala_size

            # Verificar se há texto (pode acontecer na última linha da matriz)
            if k < texto_claro_size:
                texto_cifrado += texto_claro[k]

    return texto_cifrado

#
# Testes
#


TEXTO_CLARO = 'abatalhacomospersasteralugarnodesfiladeirodastermopilas'

# i     AENDBROAASDSTAETASSELTFRHEIMARLOCAAPOLDIMUELOGIASARSPRO
print(citala_espartana(TEXTO_CLARO, 4))

# ii    AMEDRPBOREOIASASDLTPLFAAAEUISSLRGLTHSAAEAARDRCSNEMOTOIO
print(citala_espartana(TEXTO_CLARO, 6))


# EX 1  AAERNLDOBCRAOAAPAOSLDDSITMAUEETLAOSGSIEALSTAFRRSHPERIOM
print(citala_espartana(TEXTO_CLARO, 8))

# EX 2  AHSSUDADMSBAPTGEDAOACEEASESPTORRRFITIAMSANIRELLOALOLORA
print(citala_espartana(TEXTO_CLARO, 10))
