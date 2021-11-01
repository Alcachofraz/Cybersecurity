# 16 -> 4 bit
s = 16
n = 4
l = s - n


def hash_MD_funcaof(m: str, H0: str):
    H = [H0]
    blocks = len(m)//l

    # Split 'm' into blocks
    mi = [m[i*l:i*l+l] for i in range(blocks+1)]

    # Add OneZerosPadding
    mi[blocks] += '1'
    mi[blocks] = mi[blocks].ljust(l, '0')

    # Add bloco de comprimento
    mi += [bin(l).removeprefix('0b').rjust(l, '0')]

    # Apply 'f'
    for i in range(len(mi)):
        # Split each 'm' into blocks of 4 bits
        B = [(H[i] + mi[i])[j*4:j*4+4] for j in range(4)]

        # Sum each block
        for j in range(3):
            B[j+1] = bin(int(B[j], 2)+int(B[j+1], 2)).removeprefix('0b')[-4:]

        # Update next H
        H += [B[3].rjust(4, '0')]

    return H[len(mi)].rjust(4, '0')


m = '010101010101010000'
H0 = '1111'

# 0000
print(hash_MD_funcaof(m, H0))

# EX 1	1011
print(hash_MD_funcaof(m, '1010'))

# EX 2	0001
print(hash_MD_funcaof(m, '0000'))

# EX 3	0011
print(hash_MD_funcaof('111111111111111111111', '0000'))

# EX 4	0100
print(hash_MD_funcaof('0000000000000000', '0000'))
