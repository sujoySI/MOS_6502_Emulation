#pragma once
#include <cstdint>
#include <cstring>
#include <iostream>


using UI8 = uint8_t;
using UI16 = uint16_t;
using UI32 = uint32_t;

union FLAG {
    UI8 Flags;

    struct {
        UI8 C: 1; //Carryover Flag    //LSB
        UI8 Z: 1; //Zero Flag
        UI8 I: 1; //Interrupt Flag
        UI8 D: 1; //Decimal Flag
        UI8 B: 1; //Break Flag (Only for Stack Values)
        UI8 :  1; //Reserved
        UI8 V: 1; //Overflow Flag
        UI8 N: 1; //Negative Flag     //MSB
    };
};

struct MEMORY {
    static constexpr UI32 MAX_MEM = 1024 * 64;
    UI8 Data[MAX_MEM];

    void Initialise();

    //Write 2 Bytes
    void WriteUI16(UI32 &cycles, const UI16 value, const UI32 address);
};

struct CPU {
    UI16 PC; //Program Counter
    UI8 SP; //Stack Pointer
    UI8 A; //Accumulator or Main register
    UI8 X; //Index register X
    UI8 Y; //Index register Y
    FLAG SF; //Status Flag Register
    void Reset(MEMORY &memory);

    UI8 FetchUI8(UI32 &cycles, const MEMORY &memory);

    UI16 FetchUI16(UI32 &cycles, const MEMORY &memory);


    //OPCODES
    static constexpr UI8 INS_LDA_IM = 0xA9;
    static constexpr UI8 INS_LDA_ZP = 0xA5;
    static constexpr UI8 INS_LDA_ZPX = 0xB5;
    static constexpr UI8 INS_JSR = 0x20;

    //Status
    void LDASetStatus();

    [[nodiscard]] UI32 Execute(UI32 cycles, MEMORY &memory);
};
