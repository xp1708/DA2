.data
array: .byte 128

.text
.globl main

main:
    # Kh?i t?o gi� tr? v?i c�c l?nh kh�c nhau
    addi x1, x0, 5          # x1 = 5
    add x2, x1, x1          # x2 = x1 + x1 = 10
    lw x1, 8(x3)
    or x3, x1, x2           # x3 = x1 | x2 = 15 (OR logic)
    xor x4, x3, x1          # x4 = x3 ^ x1 = 10 (XOR logic)
    lui x5, 10000           # x5 = 0x10000
    addi x6, x5, 1          # x6 = 0x10000 - 1 = 0xFFFF
    and x7, x5, x6          # x7 = x5 & x6 = 0x10000
    sll x8, x1, x2          # x8 = x1 << x2 (shift left logical)
    srl x9, x8, x2          # x9 = x8 >> x2 (shift right logical)
    slt x10, x1, x2         # x10 = (x1 < x2) = 1 (set less than)
    add x11, x6, x7         # x11 = x6 + x7
    xor x12, x11, x8        # x12 = x11 ^ x8
    and x13, x4, x6         # x13 = x4 & x6
    or x14, x8, x9          # x14 = x8 | x9
    sub x15, x14, x11       # x15 = x14 - x11
    beq x5, x6, end
    
    # L?nh nh?y ??n nh�n n?m tr??c ?o?n l?nh
start:
    jal x1, target           # Jump to target
    j start                  # Jump ng??c l?i nh�n start

end: 
    auipc x17, 10000

target: 
    add x1, x2, x5
    sub x5, x4, x7	

    # L?u c�c gi� tr? t? x1 ??n x15 v�o v�ng nh? array
    la x31, array           # L?y ??a ch? v�ng nh? array v�o x31

    sw x1, 0(x31)           # L?u x1 v�o array[0]
    sw x2, 4(x31)           # L?u x2 v�o array[1]
    sw x3, 8(x31)           # L?u x3 v�o array[2]
    sw x4, 12(x31)          # L?u x4 v�o array[3]
    sw x5, 16(x31)          # L?u x5 v�o array[4]
    sw x6, 20(x31)          # L?u x6 v�o array[5]
    sw x7, 24(x31)          # L?u x7 v�o array[6]
    sw x8, 28(x31)          # L?u x8 v�o array[7]
    sw x9, 32(x31)          # L?u x9 v�o array[8]
    sw x10, 36(x31)         # L?u x10 v�o array[9]
    sw x11, 40(x31)         # L?u x11 v�o array[10]
    sw x12, 44(x31)         # L?u x12 v�o array[11]
    sw x13, 48(x31)         # L?u x13 v�o array[12]
    sw x14, 52(x31)         # L?u x14 v�o array[13]
    sw x15, 56(x31)         # L?u x15 v�o array[14]
    sw x16, 60(x31)         # L?u x16 v�o array[15]
    sw x17, 64(x31)         # L?u x17 v�o array[16]
    sw x18, 68(x31)         # L?u x18 v�o array[17]
    sw x19, 72(x31)         # L?u x19 v�o array[18]
    sw x20, 76(x31)         # L?u x20 v�o array[19]
    sw x21, 80(x31)         # L?u x21 v�o array[20]
    sw x22, 84(x31)         # L?u x22 v�o array[21]
    sw x23, 88(x31)         # L?u x23 v�o array[22]
    sw x24, 92(x31)         # L?u x24 v�o array[23]
    sw x25, 96(x31)         # L?u x25 v�o array[24]
    sw x26, 100(x31)        # L?u x26 v�o array[25]
    sw x27, 104(x31)        # L?u x27 v�o array[26]
    sw x28, 108(x31)        # L?u x28 v�o array[27]
    sw x29, 112(x31)        # L?u x29 v�o array[28]
    sw x30, 116(x31)        # L?u x30 v�o array[29]

    # K?t th�c ch??ng tr�nh