#pragma once
#include <cstdint>
#include <iostream>

namespace m6502 {
    using UI8 = uint8_t;
    using UI16 = uint16_t;
    using UI32 = uint32_t;
    using SI32 = int32_t;

    union FLAG;

    struct MEMORY;
    struct CPU;
}

union m6502::FLAG {
    UI8 Flags;

    struct {
        UI8 C: 1; //Carryover Flag    //LSB
        UI8 Z: 1; //Zero Flag
        UI8 I: 1; //Interrupt Flag
        UI8 D: 1; //Decimal Flag
        UI8 B: 1; //Break Flag (Only for Stack Values)
        UI8 RU:1; //Reserved
        UI8 V: 1; //Overflow Flag
        UI8 N: 1; //Negative Flag     //MSB
    };
};

struct m6502::MEMORY {
    static constexpr UI32 MAX_MEM = 1024 * 64;
    UI8 Data[MAX_MEM];

    void Initialise();

    //Write 2 Bytes
    void WriteUI16(SI32 &cycles, UI16 value, UI32 address);
};

struct m6502::CPU {
    UI16 PC; //Program Counter
    UI8 SP; //Stack Pointer
    UI8 A; //Accumulator or Main register
    UI8 X; //Index register X
    UI8 Y; //Index register Y
    FLAG SF; //Status Flag Register
    void Reset(MEMORY &memory);

    UI8 FetchUI8(SI32 &cycles, const MEMORY &memory);

    UI16 FetchUI16(SI32 &cycles, const MEMORY &memory);


    //OPCODES
    static constexpr UI8
    //LDA
    INS_LDA_IM = 0xA9,
    INS_LDA_ZP = 0xA5,
    INS_LDA_ZPX = 0xB5,
    INS_LDA_ABS = 0xAD,
    INS_LDA_ABSX = 0xBD,
    INS_LDA_ABSY = 0xB9,
    INS_LDA_INDX = 0xA1,
    INS_LDA_INDY = 0xB1,
    //LDX
    INS_LDX_IM = 0xA2,
    INS_LDX_ZP = 0xA6,
    INS_LDX_ZPY = 0xB6,
    INS_LDX_ABS = 0xAE,
    INS_LDX_ABSY = 0xBE,
    //LDY
    INS_LDY_IM = 0xA0,
    INS_LDY_ZP = 0xA4,
    INS_LDY_ZPX = 0xB4,
    INS_LDY_ABS = 0xAC,
    INS_LDY_ABSX = 0xBC,
    //JSR
    INS_JSR = 0x20;

    //Status
    void LoadRegistersSetStatus(UI8 Register);

    [[nodiscard]] SI32 Execute(SI32 cycles, MEMORY &memory);

    UI16 AddrZeroPage(SI32 &cycles, const MEMORY &memory);
    UI16 AddrZeroPageX(SI32 &cycles, const MEMORY &memory);
    UI16 AddrZeroPageY(SI32 &cycles, const MEMORY &memory);
    UI16 AddrAbsolute(SI32 &cycles, const MEMORY &memory);
    UI16 AddrAbsoluteX(SI32 &cycles, const MEMORY &memory);
    UI16 AddrAbsoluteY(SI32 &cycles, const MEMORY &memory);
};
