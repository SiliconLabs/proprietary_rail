# DSSS chip sequence generation

VALID_SEQUENCES = ((4,4),(8,8),(16,16),(32,32),(2,4),(2,8),(4,8),(4,16),(8,16),(8,32), (16,32)) # according to AN971

SF =  4     # Spreading factor
CL = 16     # Code length

assert (SF,CL) in VALID_SEQUENCES

BCS = CL // SF              # Bit / Chip sequence
BCRS = CL // 2**(BCS-1)     # Binary Cyclic Right Shift

is_OQPSK = False            # set this flag if for getting OQPSK chip sequences

BASE = int("0xF649", 16)    # give it in the representation
                          # as it is in the configurator GUI

BASE = BASE if is_OQPSK else int('{:0{l}b}'.format(BASE, l=CL)[::-1],2)

CONJ = int((CL//2)*"10", 2) if is_OQPSK else int((CL//2)*"11", 2)

# right hand shifting upon the configured values
ror = lambda val, r_bits: \
    ((val & (2**CL-1)) >> r_bits%CL) | \
    (val << (CL-(r_bits%CL)) & (2**CL-1))

def print_dsss_sequences(BASE, form='b'):
    NOCS = 2**BCS   # number of chip sequences.

    ror_direction = -1 if is_OQPSK else 1

    for i in range(2**(BCS - 1)):
        if form == 'h':
            print(f'{ror(BASE,ror_direction*i*BCRS):0{CL//4}x}')
        elif form == 'b':
            print(f'{ror(BASE,ror_direction*i*BCRS):0{CL}b}')
    for i in range(2**(BCS - 1)):
        if form == 'h':
            print(f'{ror(BASE^CONJ,ror_direction*i*BCRS):0{CL//4}x}')
        elif form == 'b':
            print(f'{ror(BASE^CONJ,ror_direction*i*BCRS):0{CL}b}')


if __name__ == '__main__':
    print_dsss_sequences(BASE, form='h')
