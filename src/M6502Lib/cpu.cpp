#include "M6502Lib/cpu.h"
#include <cstring>

void m6502::MEMORY::Initialise() {
    memset(Data, 0x00, MAX_MEM * sizeof(Data[0]));
}

void m6502::CPU::Reset(const UI16 ResetVector, MEMORY &memory) {
    PC = ResetVector;
    SP = 0xFF;
    SF.Flags = 0X0000;
    A = X = Y = 0x00;
    memory.Initialise();
}

m6502::UI8 m6502::CPU::ReadUI8(SI32 &cycles, const UI16 address, const MEMORY &memory) {
    const UI8 Datas = memory.Data[address];
    --cycles;
    return Datas;
}

m6502::UI16 m6502::CPU::ReadUI16(SI32 &cycles, const UI16 address, const MEMORY &memory) {
    const UI8 LoByte = ReadUI8(cycles, address, memory);
    const UI8 HiByte = ReadUI8(cycles, address + 1, memory);
    UI16 value = LoByte | (HiByte << 8);
    return value;
}

void m6502::CPU::WriteUI8(const UI8 value ,SI32 &cycles, const UI16 address, MEMORY &memory) {
    memory.Data[address] = value;
    --cycles;
}

void m6502::CPU::WriteUI16( const UI16 value, SI32 &cycles, const UI16 address, MEMORY &memory) {
    memory.Data[address] = value & 0xFF;
    memory.Data[address + 1] = (value >> 8);
    cycles -= 2;
}

m6502::UI8 m6502::CPU::FetchUI8(SI32 &cycles, const MEMORY &memory) {
    const UI8 Datas = memory.Data[PC];
    ++PC;
    --cycles;
    return Datas;
}

m6502::UI16 m6502::CPU::FetchUI16(SI32 &cycles, const MEMORY &memory) {
    //6502 is Little Endian
    UI16 Datas = memory.Data[PC];
    ++PC;
    Datas |= (memory.Data[PC] << 8);
    ++PC;
    cycles -= 2;
    return Datas;
}



m6502::UI16 m6502::CPU::AddrZeroPage(SI32 &cycles, const MEMORY &memory) {
    const UI8 ZeroPageAddr = FetchUI8(cycles, memory);
    return ZeroPageAddr;
}
m6502::UI16 m6502::CPU::AddrZeroPageX(SI32 &cycles, const MEMORY &memory) {
    UI8 ZeroPageAddr = FetchUI8(cycles, memory);
    ZeroPageAddr += X;
    --cycles;
    return ZeroPageAddr;
}

m6502::UI16 m6502::CPU::AddrZeroPageY(SI32 &cycles, const MEMORY &memory) {
    UI8 ZeroPageAddr = FetchUI8(cycles, memory);
    ZeroPageAddr += Y;
    --cycles;
    return ZeroPageAddr;
}

m6502::UI16 m6502::CPU::AddrAbsolute(SI32 &cycles, const MEMORY &memory) {
    const UI16 AbsAddress = FetchUI16(cycles, memory);
    return AbsAddress;
}

m6502::UI16 m6502::CPU::AddrAbsoluteX(SI32 &cycles, const MEMORY &memory) {
    const UI16 AbsAddress = FetchUI16(cycles, memory);
    const UI16 AbsAddressX = AbsAddress + X;
    if (AbsAddressX - AbsAddress >= 0xFF) {
        --cycles;
    }
    return AbsAddressX;
}

m6502::UI16 m6502::CPU::AddrAbsoluteY(SI32 &cycles, const MEMORY &memory) {
    const UI16 AbsAddress = FetchUI16(cycles, memory);
    const UI16 AbsAddressY = AbsAddress + Y;
    if (AbsAddressY - AbsAddress >= 0xFF) {
        --cycles;
    }
    return AbsAddressY;
}

//Takes 5 cycles always
m6502::UI16 m6502::CPU::AddrAbsoluteX_5(SI32 &cycles, const MEMORY &memory) {
    const UI16 AbsAddress = FetchUI16(cycles, memory);
    const UI16 AbsAddressX = AbsAddress + X;
    --cycles;
    return AbsAddressX;
}

//Takes 6 cycles always
m6502::UI16 m6502::CPU::AddrAbsoluteY_6(SI32 &cycles, const MEMORY &memory) {
    const UI16 AbsAddress = FetchUI16(cycles, memory);
    const UI16 AbsAddressY = AbsAddress + Y;
    --cycles;
    return AbsAddressY;
}

m6502::UI16 m6502::CPU::AddrIndirectX(SI32 &cycles, const MEMORY &memory) {
    UI8 ZpAddress = FetchUI8(cycles, memory);
    ZpAddress += X;
    --cycles;
    const UI16 EffectiveAddress = ReadUI16(cycles, ZpAddress,memory);
    return EffectiveAddress;
}

m6502::UI16 m6502::CPU::AddrIndirectY(SI32 &cycles, const MEMORY &memory) {
    const UI8 ZpAddress = FetchUI8(cycles, memory);
    const UI16 EffectiveAddress = ReadUI16(cycles, ZpAddress,memory);
    const UI16 EffectiveAddressY = EffectiveAddress + Y;
    if (EffectiveAddressY - EffectiveAddress >= 0xFF) {
        --cycles;
    }
    return EffectiveAddressY;
}

//Takes 6 cycles always
m6502::UI16 m6502::CPU::AddrIndirectY_6(SI32 &cycles, const MEMORY &memory) {
    const UI8 ZpAddress = FetchUI8(cycles, memory);
    const UI16 EffectiveAddress = ReadUI16(cycles, ZpAddress,memory);
    const UI16 EffectiveAddressY = EffectiveAddress + Y;
    --cycles;
    return EffectiveAddressY;
}

void m6502::CPU::LoadRegistersSetStatus(const UI8 Register) {
    SF.Z = (Register == 0);
    SF.N = (Register & 0b10000000) > 0;
}

m6502::SI32 m6502::CPU::Execute(SI32 cycles, MEMORY &memory) {
    auto LoadRegister = [&cycles, &memory, this] (const UI16 Address, UI8& Register)
    {
        Register = ReadUI8(cycles, Address, memory);
        LoadRegistersSetStatus(Register);
    };

    const SI32 CyclesRequested = cycles;
    while (cycles > 0) {
        switch (const UI8 Instruction = FetchUI8(cycles, memory)) {
            case INS_LDA_IM: //Load Accumulator ImmediateMode
            {
                A = FetchUI8(cycles, memory);
                LoadRegistersSetStatus(A);
            }
            break;
            case INS_LDA_ZP: //Load Accumulator ZeroPageMode
            {
                const UI16 Address = AddrZeroPage(cycles, memory);
                LoadRegister(Address, A);
            }
            break;
            case INS_LDA_ZPX: //Load Accumulator ZeroPageXMode
            {
                const UI16 Address = AddrZeroPageX(cycles, memory);
                LoadRegister(Address, A);
            }
            break;
            case INS_LDA_ABS: //Load Accumulator AbsoluteMode
            {
                const UI16 Address = AddrAbsolute(cycles, memory);
                LoadRegister(Address, A);
            }
                break;
            case INS_LDA_ABSX: //Load Accumulator AbsoluteXMode
            {
                const UI16 Address = AddrAbsoluteX(cycles, memory);
                LoadRegister(Address, A);
            }
                break;
            case INS_LDA_ABSY: //Load Accumulator AbsoluteYMode
            {
                const UI16 Address = AddrAbsoluteY(cycles, memory);
                LoadRegister(Address, A);
            }
                break;
            case INS_LDA_INDX: //Load Accumulator IndexedIndirectXMode
            {
                const UI16 Address = AddrIndirectX(cycles, memory);
                LoadRegister(Address, A);
            }
                break;
            case INS_LDA_INDY: //Load Accumulator IndirectIndexedYMode
            {
                const UI16 Address = AddrIndirectY(cycles, memory);
                LoadRegister(Address, A);
            }
                break;
            //
            //LDX
            case INS_LDX_IM: //Load X Register ImmediateMode
            {
                X= FetchUI8(cycles, memory);
                LoadRegistersSetStatus(X);
            }
                break;
            case INS_LDX_ZP: //Load X Register ZeroPageMode
            {
                const UI16 Address = AddrZeroPage(cycles, memory);
                LoadRegister(Address, X);
            }
                break;
            case INS_LDX_ZPY: //Load X Register ZeroPageYMode
            {
                const UI16 Address = AddrZeroPageY(cycles, memory);
                LoadRegister(Address, X);
            }
                break;
            case INS_LDX_ABS: //Load X Register AbsoluteMode
            {
                const UI16 Address = AddrAbsolute(cycles, memory);
                LoadRegister(Address, X);
            }
                break;
            case INS_LDX_ABSY: //Load X Register AbsoluteYMode
            {
                const UI16 Address = AddrAbsoluteY(cycles, memory);
                LoadRegister(Address, X);
            }
                break;
            //
            //LDY
            case INS_LDY_IM: //Load Y Register ImmediateMode
            {
                Y = FetchUI8(cycles, memory);
                LoadRegistersSetStatus(Y);
            }
                break;
            case INS_LDY_ZP: //Load Y Register ZeroPageMode
            {
                const UI16 Address = AddrZeroPage(cycles, memory);
                LoadRegister(Address, Y);
            }
                break;
            case INS_LDY_ZPX: //Load Y Register ZeroPageXMode
            {
                const UI16 Address = AddrZeroPageX(cycles, memory);
                LoadRegister(Address, Y);
            }
                break;
            case INS_LDY_ABS: //Load Y Register AbsoluteMode
            {
                const UI16 Address = AddrAbsolute(cycles, memory);
                LoadRegister(Address, Y);
            }
                break;
            case INS_LDY_ABSX: //Load Y Register AbsoluteXMode
            {
                const UI16 Address = AddrAbsoluteX(cycles, memory);
                LoadRegister(Address, Y);
            }
                break;
            //
            // STA
            case INS_STA_ZP: //Store Accumulator ZeroPageMode
            {
                const UI16 Address = AddrZeroPage(cycles, memory);
                WriteUI8(A, cycles, Address, memory);
            }
                break;
            case INS_STA_ZPX: //Store Accumulator ZeroPageXMode
            {
                const UI16 Address = AddrZeroPageX(cycles, memory);
                WriteUI8(A, cycles, Address, memory);
            }
                break;
            case INS_STA_ABS: //Store Accumulator AbsoluteMode
            {
                const UI16 Address = AddrAbsolute(cycles, memory);
                WriteUI8(A, cycles, Address, memory);
            }
                break;
            case INS_STA_ABSX: //Store Accumulator AbsoluteXMode
            {
                const UI16 Address = AddrAbsoluteX_5(cycles, memory);
                WriteUI8(A, cycles, Address, memory);
            }
                break;
            case INS_STA_ABSY: //Store Accumulator AbsoluteYMode
            {
                const UI16 Address = AddrAbsoluteY_6(cycles, memory);
                WriteUI8(A, cycles, Address, memory);
            }
                break;
            case INS_STA_INDX: //Store Accumulator IndexedIndirectXMode
            {
                const UI16 Address = AddrIndirectX(cycles, memory);
                WriteUI8(A, cycles, Address, memory);
            }
                break;
            case INS_STA_INDY: //Store Accumulator IndirectIndexedYMode
            {
                const UI16 Address = AddrIndirectY_6(cycles, memory);
                WriteUI8(A, cycles, Address, memory);
            }
                break;
            //
            // STX
            case INS_STX_ZP: //Store X Register ZeroPageMode
            {
                const UI16 Address = AddrZeroPage(cycles, memory);
                WriteUI8(X, cycles, Address, memory);
            }
                break;
            case INS_STX_ZPY: //Store X Register ZeroPageYMode
            {
                const UI16 Address = AddrZeroPageY(cycles, memory);
                WriteUI8(X, cycles, Address, memory);
            }
                break;
            case INS_STX_ABS: //Store X Register AbsoluteMode
            {
                const UI16 Address = AddrAbsolute(cycles, memory);
                WriteUI8(X, cycles, Address, memory);
            }
                break;
            //
            // STY
            case INS_STY_ZP: //Store Y Register ZeroPageMode
            {
                const UI16 Address = AddrZeroPage(cycles, memory);
                WriteUI8(Y, cycles, Address, memory);
            }
                break;
            case INS_STY_ZPX: //Store Y Register ZeroPageXMode
            {
                const UI16 Address = AddrZeroPageX(cycles, memory);
                WriteUI8(Y, cycles, Address, memory);
            }
                break;
            case INS_STY_ABS: //Store Y Register AbsoluteMode
            {
                const UI16 Address = AddrAbsolute(cycles, memory);
                WriteUI8(Y, cycles, Address, memory);
            }
                break;
            //
            //Jump and Calls
            case INS_JMP_ABS: //Jump to instruction Absolute mode
            {
                const UI16 Address = AddrAbsolute(cycles, memory);
                PC = Address;
            }
                break;
            case INS_JMP_IND: //Jump to instruction Indirect mode
            {
                UI16 Address = AddrAbsolute(cycles, memory);
                Address = ReadUI16(cycles, Address,memory);
                PC = Address;
            }
                break;
            //Note:
            // An original 6502 has does not correctly fetch the target address if the indirect vector falls on a page boundary
            // (e.g. $xxFF where xx is any value from $00 to $FF). In this case fetches the LSB from $xxFF as expected but takes
            // the MSB from $xx00. This is fixed in some later chips like the 65SC02 so for compatibility always ensure the indirect
            // vector is not at the end of the page.
            // **
            case INS_JSR: //Jump SubRoutine Only one mode
            {
                const UI16 SubAddress = FetchUI16(cycles, memory);
                PushPCtoStack(cycles, memory);
                PC = SubAddress;
                --cycles;
            }
            break;
            case INS_RTS: //Return To SubRoutine Only one mode
            {
                const UI16 ReturnAddress = PopAddressFromStack(cycles, memory);
                PC = ReturnAddress + 1;
                cycles -= 2;
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
