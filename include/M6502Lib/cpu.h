#pragma once
#include <cstdint>
#include <iostream>
#include <iomanip>

namespace m6502 {
    using UI8  = uint8_t;
    using UI16 = uint16_t;
    using UI32 = uint32_t;
    using SI8  = int8_t;
    using SI16 = int16_t;
    using SI32 = int32_t;

    union FLAG;

    struct MEMORY;
    struct CPU;
}

union m6502::FLAG {
    UI8 All;

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
    FLAG PS; //Processor Status Flag Register
    void Reset(UI16 ResetVector, MEMORY &memory);

    static UI8 ReadUI8(SI32 &cycles, UI16 address, const MEMORY &memory);
    static UI16 ReadUI16(SI32 &cycles, UI16 address, const MEMORY &memory);
    UI8 FetchUI8(SI32 &cycles, const MEMORY &memory);
    SI8 FetchSI8(SI32 &cycles, const MEMORY &memory);
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

    void PushUI8toStack(SI32 &cycles, UI8 value, MEMORY &memory) {
        const UI16 SPword = SPtoAddress();
        memory.Data[SPword] = value;
        --cycles;
        --SP;
        --cycles;
    }

    UI16 PopAddressFromStack(SI32 &cycles, const MEMORY &memory) {
        const UI16 ValueFromStack = ReadUI16(cycles, SPtoAddress() + 1, memory);
        SP +=2;
        --cycles;
        return ValueFromStack;
    }

    UI8 PopUI8fromStack(SI32 &cycles, MEMORY &memory) {
        ++SP;
        const UI16 SPword = SPtoAddress();
        const UI8 value = memory.Data[SPword];
        cycles -= 3;
        return value;
    }

    //Process Status bits
    static constexpr UI8
        NegativeFLagBit = 0b10000000,
        OverFlowFLagBit = 0b01000000;

    //OPCODES
    static constexpr UI8
    //Load one register from Memory
    INS_LDA_IM = 0xA9,
    INS_LDA_ZP = 0xA5,
    INS_LDA_ZPX = 0xB5,
    INS_LDA_ABS = 0xAD,
    INS_LDA_ABSX = 0xBD,
    INS_LDA_ABSY = 0xB9,
    INS_LDA_INDX = 0xA1,
    INS_LDA_INDY = 0xB1,
    INS_LDX_IM = 0xA2,
    INS_LDX_ZP = 0xA6,
    INS_LDX_ZPY = 0xB6,
    INS_LDX_ABS = 0xAE,
    INS_LDX_ABSY = 0xBE,
    INS_LDY_IM = 0xA0,
    INS_LDY_ZP = 0xA4,
    INS_LDY_ZPX = 0xB4,
    INS_LDY_ABS = 0xAC,
    INS_LDY_ABSX = 0xBC,

    //Store one Register to Memory
    INS_STA_ZP = 0x85,
    INS_STA_ZPX = 0x95,
    INS_STA_ABS = 0x8D,
    INS_STA_ABSX = 0x9D,
    INS_STA_ABSY = 0x99,
    INS_STA_INDX = 0x81,
    INS_STA_INDY = 0x91,
    INS_STX_ZP = 0x86,
    INS_STX_ZPY = 0x96,
    INS_STX_ABS = 0x8E,
    INS_STY_ZP = 0x84,
    INS_STY_ZPX = 0x94,
    INS_STY_ABS = 0x8C,

    //Increment and Decrement
    INS_INC_ZP = 0xE6,
    INS_INC_ZPX = 0xF6,
    INS_INC_ABS = 0xEE,
    INS_INC_ABSX = 0xFE,
    INS_INX = 0xE8,
    INS_INY = 0xC8,
    INS_DEC_ZP = 0xC6,
    INS_DEC_ZPX = 0xD6,
    INS_DEC_ABS = 0xCE,
    INS_DEC_ABSX = 0xDE,
    INS_DEX = 0xCA,
    INS_DEY = 0x88,

    //Logic Instruction
    INS_AND_IM = 0x29,
    INS_AND_ZP = 0x22,
    INS_AND_ZPX = 0x35,
    INS_AND_ABS = 0x2D,
    INS_AND_ABSX = 0x3D,
    INS_AND_ABSY = 0x39,
    INS_AND_INDX = 0x21,
    INS_AND_INDY = 0x31,
    INS_EOR_IM = 0x49,
    INS_EOR_ZP = 0x45,
    INS_EOR_ZPX = 0x55,
    INS_EOR_ABS = 0x4D,
    INS_EOR_ABSX = 0x5D,
    INS_EOR_ABSY = 0x59,
    INS_EOR_INDX = 0x41,
    INS_EOR_INDY = 0x51,
    INS_ORA_IM = 0x09,
    INS_ORA_ZP = 0x05,
    INS_ORA_ZPX = 0x15,
    INS_ORA_ABS = 0x0D,
    INS_ORA_ABSX = 0x1D,
    INS_ORA_ABSY = 0x19,
    INS_ORA_INDX = 0x01,
    INS_ORA_INDY = 0x11,
    INS_BIT_ZP = 0x24,
    INS_BIT_ABS = 0x2C,

    //Arithmetic
    //ADC
    INS_ADC_IM = 0x69,
    INS_ADC_ZP = 0x65,
    INS_ADC_ZPX = 0x75,
    INS_ADC_ABS = 0x6D,
    INS_ADC_ABSX = 0x7D,
    INS_ADC_ABSY = 0x79,
    INS_ADC_INDX = 0x61,
    INS_ADC_INDY = 0x71,
    //SBC
    INS_SBC_IM = 0xE9,
    INS_SBC_ZP = 0xE5,
    INS_SBC_ZPX = 0xF5,
    INS_SBC_ABS = 0xED,
    INS_SBC_ABSX = 0xFD,
    INS_SBC_ABSY = 0xF9,
    INS_SBC_INDX = 0xE1,
    INS_SBC_INDY = 0xF1,
    //CMP
    INS_CMP_IM = 0xC9,
    INS_CMP_ZP = 0xC5,
    INS_CMP_ZPX = 0xD5,
    INS_CMP_ABS = 0xCD,
    INS_CMP_ABSX = 0xDD,
    INS_CMP_ABSY = 0xD9,
    INS_CMP_INDX = 0xC1,
    INS_CMP_INDY = 0xD1,
    //CPX
    INS_CPX_IM = 0xE0,
    INS_CPX_ZP = 0xE4,
    INS_CPX_ABS = 0xEC,
    //CPX
    INS_CPY_IM = 0xC0,
    INS_CPY_ZP = 0xC4,
    INS_CPY_ABS = 0xCC,

    //Branch Instructions
    INS_BCC = 0x90,
    INS_BCS = 0xB0,
    INS_BEQ = 0xF0,
    INS_BMI = 0x30,
    INS_BNE = 0xD0,
    INS_BPL = 0x10,
    INS_BVC = 0x50,
    INS_BVS = 0x70,

    //Trasnfer Register to another
    INS_TAX = 0xAA,
    INS_TAY = 0xA8,
    INS_TXA = 0x8A,
    INS_TYA = 0x98,

    //Stack Operatrions
    INS_TSX = 0xBA,
    INS_TXS = 0x9A,
    INS_PHA = 0x48,
    INS_PLA = 0x68,
    INS_PHP = 0x08,
    INS_PLP = 0x28,

    //Status Flag Change
    INS_CLC = 0x18,
    INS_CLD = 0xD8,
    INS_CLI = 0x58,
    INS_CLV = 0xB8,
    INS_SEC = 0x38,
    INS_SED = 0xF8,
    INS_SEI = 0x78,

    //System Functions
    INS_BRK = 0x00,
    INS_NOP = 0xEA,
    INS_RTI = 0x40,

    //Jump and Calls
    INS_JMP_ABS = 0x4C,
    INS_JMP_IND = 0x6C,
    INS_JSR = 0x20,
    INS_RTS = 0x60;

    //Status
    void SetZeroAndNegativeFlag(UI8 Register);

    static UI16 LoadPrg(const UI8* Program, SI32 Numbytes, MEMORY &memory);

    void PrintStatus() const;
    void PrintStatusHex() const;

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
