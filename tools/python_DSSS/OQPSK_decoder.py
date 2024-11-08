# The following code will take out OQPSK sequences from DOUT signal 
# (deviation -> phase transformation)

dout = 

length = len(f'{dout:0x}') * 4 # number of nibbles in dout
dout = int('{:0{length}b}'.format(dout, length=length)[::-1], 2)

CL = 16 # Chip sequence length

c = dout & 1
lb = c
dout >>= 1

for i in range(length-1):
    nb = lb ^ (dout & 1) ^ ((i + 1) & 1)
    c <<= 1
    dout >>= 1
    c |= nb
    lb = nb

c = int('{:0{length}b}'.format(c, length=length)[::-1], 2)

mask = int((CL)*"1", 2)

for _ in range(int(length // CL)):
    print(f'{c & mask:02x}')
    c >>= CL
