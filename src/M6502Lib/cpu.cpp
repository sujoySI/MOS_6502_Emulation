#include "M6502Lib/cpu.h"

static UI8 ReadUI8(SI32 &cycles, const UI16 address, const MEMORY &memory) {
    const UI8 Datas = memory.Data[address];
    --cycles;
    return Datas;
}

static UI16 ReadUI16(SI32 &cycles, const UI16 address, const MEMORY &memory) {
    UI8 LoByte = ReadUI8(cycles, address, memory);
    UI8 HiByte = ReadUI8(cycles, address + 1, memory);
    return LoByte | (HiByte << 8);
}

void MEMORY::Initialise() {
    memset(Data, 0x00, MAX_MEM * sizeof(Data[0]));
}

void MEMORY::WriteUI16(SI32 &cycles, const UI16 value, const UI32 address) {
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

UI8 CPU::FetchUI8(SI32 &cycles, const MEMORY &memory) {
    const UI8 Datas = memory.Data[PC];
    ++PC;
    --cycles;
    return Datas;
}


UI16 CPU::FetchUI16(SI32 &cycles, const MEMORY &memory) {
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

SI32 CPU::Execute(SI32 cycles, MEMORY &memory) {
    const SI32 CyclesRequested = cycles;
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
            case INS_LDA_ABS: //Load Accumulator AbsoluteMode
            {
                UI16 AbsAddress = FetchUI16(cycles, memory);
                A = ReadUI8(cycles, AbsAddress, memory);
                LDASetStatus();
            }
                break;
            case INS_LDA_ABSX: //Load Accumulator AbsoluteXMode
            {
                UI16 AbsAddress = FetchUI16(cycles, memory);
                UI16 AbsAddressX = AbsAddress + X;
                A = ReadUI8(cycles, AbsAddressX, memory);
                if (AbsAddressX - AbsAddress >= 0xFF) {
                    --cycles;
                }
                LDASetStatus();
            }
                break;
            case INS_LDA_ABSY: //Load Accumulator AbsoluteYMode
            {
                UI16 AbsAddress = FetchUI16(cycles, memory);
                UI16 AbsAddressY = AbsAddress + Y;
                A = ReadUI8(cycles, AbsAddressY, memory);
                if (AbsAddressY - AbsAddress >= 0xFF) {
                    --cycles;
                }
                LDASetStatus();
            }
                break;
            case INS_LDA_INDX: //Load Accumulator IndexedIndirectXMode
            {
                UI8 ZpAddress = FetchUI8(cycles, memory);
                ZpAddress += X;
                --cycles;
                UI16 EffectiveAddress = ReadUI16(cycles, ZpAddress,memory);
                A = ReadUI8(cycles, EffectiveAddress, memory);
                LDASetStatus();
            }
                break;
            case INS_LDA_INDY: //Load Accumulator IndirectIndexedYMode
            {
                UI8 ZpAddress = FetchUI8(cycles, memory);
                UI16 EffectiveAddress = ReadUI16(cycles, ZpAddress,memory);
                UI16 EffectiveAddressY = EffectiveAddress + Y;
                A = ReadUI8(cycles, EffectiveAddressY, memory);
                if (EffectiveAddressY - EffectiveAddress >= 0xFF) {
                    --cycles;
                }
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
                //If other errors occur then I blame ReadUI16's --cycles.
                std::cout << "Instruction not handled: " << Instruction << "\n";
                throw -1;
            }
            break;
        }
    }
    return CyclesRequested - cycles;
}
