#include <stdio.h>

unsigned int SetBit(unsigned int value, int position) {
    if (position < 0 || position >= 32) return value;
    return value | (1u << position);
}

unsigned int ClearBit(unsigned int value, int position) {
    if (position < 0 || position >= 32) return value;
    return value & ~(1u << position);
}

unsigned int ToggleBit(unsigned int value, int position) {
    if (position < 0 || position >= 32) return value;
    return value ^ (1u << position);
}

int GetBit(unsigned int value, int position) {
    if (position < 0 || position >= 32) return 0;
    return (value >> position) & 1u;
}

int CountBits(unsigned int value) {
    int count = 0;.
    while (value) {
        count += value & 1u;
        value >>= 1;}
    return count;
}

unsigned int ShiftLeft(unsigned int value, int positions) {
    if (positions < 0) return value;
    return value << positions;
}

unsigned int ShiftRight(unsigned int value, int positions) {
    if (positions < 0) return value;
    return value >> positions;
}

void PrintBinary(unsigned int value) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", (value >> i) & 1u);
        if (i % 4 == 0) printf(" ");
    }
    printf("\n");
}

void PrintHex(unsigned int value) {
    printf("0x%08X\n", value);
}

int main() {
    unsigned int data = 0;
    printf("Initial value:\n");
    PrintBinary(data);
    data = SetBit(data, 3);
    printf("\nAfter SetBit(3):\n");
    PrintBinary(data);
    printf("\nBit 3 value: %d\n", GetBit(data, 3));
    printf("Number of 1s: %d\n", CountBits(data));

    data = ToggleBit(data, 3);
    printf("\nAfter ToggleBit(3):\n");
    PrintBinary(data);
    data = SetBit(data, 7);
    printf("\nAfter SetBit(7):\n");
    PrintBinary(data);

    data = ClearBit(data, 7);
    printf("After ClearBit(7):\n");
    PrintBinary(data);
    unsigned int move = ShiftLeft(1, 4);
    printf("\nShiftLeft(1,4): ");
    PrintBinary(move);

    move = ShiftRight(move, 2);
    printf("ShiftRight(result,2): ");
    PrintBinary(move);
    printf("\nHexadecimal output: ");
    PrintHex(move);
    return 0;
}