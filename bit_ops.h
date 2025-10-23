#ifndef BIT_OPS_H
#define BIT_OPS_H

#include <stdio.h>

typedef unsigned long long U64;

U64 SetBit(U64 value, int position);
U64 ClearBit(U64 value, int position);
U64 ToggleBit(U64 value, int position);
int GetBit(U64 value, int position);
int CountBits(U64 value);
U64 ShiftLeft(U64 value, int positions);
U64 ShiftRight(U64 value, int positions);
void PrintBinary(U64 value);
void PrintHex(U64 value);
U64 CreateMask(int position);

#endif