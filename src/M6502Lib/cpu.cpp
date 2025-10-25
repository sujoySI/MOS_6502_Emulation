#include "M6502Lib/cpu.h"

static UI8 ReadUI8(UI32 &cycles, const UI8 address, const MEMORY &memory) {
    const UI8 Datas = memory.Data[address];
    --cycles;
    return Datas;
}

void MEMORY::Initialise() {
    memset(Data, 0x00, MAX_MEM * sizeof(Data[0]));
}

void MEMORY::WriteUI16(UI32 &cycles, const UI16 value, const UI32 address) {
    Data[address] = value & 0xFF;
    Data[address + 1] = (value >> 8);
    cycles -= 2;
}

void CPU::Reset(MEMORY &memory) {
    PC = 0xFFFC;
    SP = 0x00;
    SF.Flags = 0X0000;
    A = X = Y = 0x00;
    memory.Initialise();
}

UI8 CPU::FetchUI8(UI32 &cycles, const MEMORY &memory) {
    const UI8 Datas = memory.Data[PC];
    ++PC;
    --cycles;
    return Datas;
}


UI16 CPU::FetchUI16(UI32 &cycles, const MEMORY &memory) {
    //6502 is Little Endian
    UI16 Datas = memory.Data[PC];
    ++PC;
    Datas |= (memory.Data[PC] << 8);
    ++PC;
    cycles -= 2;
    // If I wanted to handle endianness I would have to swap bytes here
    //if(PLATFORM_BIG_ENDIAN)
    //SwapBytesInWord(Data);
    return Datas;
}

void CPU::LDASetStatus() {
    SF.Z = (A == 0);
    SF.N = (A & 0b10000000) > 0;
}

UI32 CPU::Execute(UI32 cycles, MEMORY &memory) {
    while (cycles > 0) {
        switch (const UI8 Instruction = FetchUI8(cycles, memory)) {
            case INS_LDA_IM: //Load Accumulator ImmediateMode
            {
                const UI8 Value = FetchUI8(cycles, memory);
                A = Value;
                LDASetStatus();
            }
            break;
            case INS_LDA_ZP: //Load Accumulator ZeroPageMode
            {
                const UI8 ZeroPageAddress = FetchUI8(cycles, memory);
                A = ReadUI8(cycles, ZeroPageAddress, memory);
                LDASetStatus();
            }
            break;
            case INS_LDA_ZPX: //Load Accumulator ZeroPageXMode
            {
                UI8 ZeroPageAddress = FetchUI8(cycles, memory);
                ZeroPageAddress += X;
                --cycles;
                A = ReadUI8(cycles, ZeroPageAddress, memory);
                LDASetStatus();
            }
            break;
            case INS_JSR: //Jump SubRoutine Only one mode
            {
                const UI16 SubAddress = FetchUI16(cycles, memory);
                memory.WriteUI16(cycles, PC - 1, SP);
                ++SP;
                PC = SubAddress;
                --cycles;
            }
            break;
            default: {
                std::cout << "Instruction not handled: " << Instruction << "\n";
            }
            break;
        }
    }
    return cycles;
}
