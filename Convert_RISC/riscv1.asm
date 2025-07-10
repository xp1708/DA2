# phep toan
# loi so hoc
# cac phep toan so h?c
add x18, x19, x20 #thuc hien phep cong
sub x5, x6, x7
sll x29, x30, x31
slt x8,x9, x10
sltu x2, x3, x4
xor x15, x16,x17
srl x22, x23, x24 #thuc hien shift right logical
sra x25,x26, x27
or x5,x8,x10
and x5, x6, x20 # thuc hien phep tinh and
addi x2, x1, 10   # x2 = x1 + 10
slti x3, x1, 5    # x3 = (x1 < 5) ? 1 : 0
xori x4, x1, 3 # x4 = x1 ^ 0xFF
ori x5, x1, 4  # x5 = x1 | 0x0F
andi x6, x1, 7 # x6 = x1 & 0xF0
slli x1, x2, 3  # Shift left logical immediate: x1 = x2 << 3
srli x3, x4, 2  # Shift right logical immediate: x3 = x4 >> 2
srai x5, x6, 1  # Shift right arithmetic immediate: x5 = x6 >>> 1
lb x1, 0(x2)    # Load byte t? ??a ch? trong x2 vào thanh ghi x1
lh x3, 4(x4)    # Load halfword t? ??a ch? trong x4 + 4 vào thanh ghi x3
lw x5, 8(x6)    # Load word t? ??a ch? trong x6 + 8 vào thanh ghi x5
lbu x7, 12(x8)  # Load byte unsigned t? ??a ch? trong x8 + 12 vào thanh ghi x7
lhu x9, 16(x10) # Load halfword unsigned t? ??a ch? trong x10 + 16 vào thanh ghi x9
sltiu x3, x2, 5  # ??t x3 = 1 n?u x2 < 5 (unsigned), ng??c l?i x3 = 0
