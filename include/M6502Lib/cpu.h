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
};

struct m6502::CPU {
    UI16 PC; //Program Counter
    UI8 SP; //Stack Pointer
    UI8 A; //Accumulator or Main register
    UI8 X; //Index register X
    UI8 Y; //Index register Y
    FLAG SF; //Status Flag Register
    void Reset(UI16 ResetVector, MEMORY &memory);

    static UI8 ReadUI8(SI32 &cycles, UI16 address, const MEMORY &memory);
    static UI16 ReadUI16(SI32 &cycles, UI16 address, const MEMORY &memory);
    UI8 FetchUI8(SI32 &cycles, const MEMORY &memory);
    UI16 FetchUI16(SI32 &cycles, const MEMORY &memory);
    static void WriteUI8(UI8 value ,SI32 &cycles, UI16 address, MEMORY &memory);
    static void WriteUI16(UI16 value, SI32 &cycles, UI16 address, MEMORY &memory);

    //Return stack pointer as full 16-bit Address in
    [[nodiscard]] UI16 SPtoAddress() const {
        auto value = 0x100 | SP;
        return value;
    }

    void PushPCtoStack(SI32 &cycles, MEMORY &memory) {
        WriteUI16(PC -1, cycles, SPtoAddress() - 1, memory);
        SP -= 2;
    }

    UI16 PopAddressFromStack(SI32 &cycles, const MEMORY &memory) {
        const UI16 ValueFromStack = ReadUI16(cycles, SPtoAddress() + 1, memory);
        SP +=2;
        --cycles;
        return ValueFromStack;
    }

    //OPCODES
    static constexpr UI8
    //LDA aka LoaD A register
    INS_LDA_IM = 0xA9,
    INS_LDA_ZP = 0xA5,
    INS_LDA_ZPX = 0xB5,
    INS_LDA_ABS = 0xAD,
    INS_LDA_ABSX = 0xBD,
    INS_LDA_ABSY = 0xB9,
    INS_LDA_INDX = 0xA1,
    INS_LDA_INDY = 0xB1,
    //LDX aka LoaD X register
    INS_LDX_IM = 0xA2,
    INS_LDX_ZP = 0xA6,
    INS_LDX_ZPY = 0xB6,
    INS_LDX_ABS = 0xAE,
    INS_LDX_ABSY = 0xBE,
    //LDY aka LoaD Y register
    INS_LDY_IM = 0xA0,
    INS_LDY_ZP = 0xA4,
    INS_LDY_ZPX = 0xB4,
    INS_LDY_ABS = 0xAC,
    INS_LDY_ABSX = 0xBC,
    //STA aka STore A register
    INS_STA_ZP = 0x85,
    INS_STA_ZPX = 0x95,
    INS_STA_ABS = 0x8D,
    INS_STA_ABSX = 0x9D,
    INS_STA_ABSY = 0x99,
    INS_STA_INDX = 0x81,
    INS_STA_INDY = 0x91,
    //STX aka STore X register
    INS_STX_ZP = 0x86,
    INS_STX_ZPY = 0x96,
    INS_STX_ABS = 0x8E,
    //STY aka STore Y register
    INS_STY_ZP = 0x84,
    INS_STY_ZPX = 0x94,
    INS_STY_ABS = 0x8C,
    //JMP aka JuMP to an instruction
    INS_JMP_ABS = 0x4C,
    INS_JMP_IND = 0x6C,
    //JSR aka Jump Sub-Routine
    INS_JSR = 0x20,
    //RTS aka Return To Subroutine
    INS_RTS = 0x60;

    //Status
    void LoadRegistersSetStatus(UI8 Register);

    [[nodiscard]] SI32 Execute(SI32 cycles, MEMORY &memory);

    UI16 AddrZeroPage(SI32 &cycles, const MEMORY &memory);
    UI16 AddrZeroPageX(SI32 &cycles, const MEMORY &memory);
    UI16 AddrZeroPageY(SI32 &cycles, const MEMORY &memory);
    UI16 AddrAbsolute(SI32 &cycles, const MEMORY &memory);
    UI16 AddrAbsoluteX(SI32 &cycles, const MEMORY &memory);
    UI16 AddrAbsoluteY(SI32 &cycles, const MEMORY &memory);
    UI16 AddrAbsoluteX_5(SI32 &cycles, const MEMORY &memory);
    UI16 AddrAbsoluteY_6(SI32 &cycles, const MEMORY &memory);
    UI16 AddrIndirectX(SI32 &cycles, const MEMORY &memory);
    UI16 AddrIndirectY(SI32 &cycles, const MEMORY &memory);
    UI16 AddrIndirectY_6(SI32 &cycles, const MEMORY &memory);
};
