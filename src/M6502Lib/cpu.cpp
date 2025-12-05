#include "M6502Lib/cpu.h"
#include <cstring>

void m6502::MEMORY::Initialise() {
    memset(Data, 0x00, MAX_MEM * sizeof(Data[0]));
}

void m6502::CPU::Reset(const UI16 ResetVector, MEMORY &memory) {
    PC = ResetVector;
    SP = 0xFF;
    PS.All = 0X0000;
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
    const UI16 value = LoByte | (HiByte << 8);
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

m6502::SI8 m6502::CPU::FetchSI8(SI32 &cycles, const MEMORY &memory) {
    return  static_cast<SI8> (FetchUI8(cycles, memory));
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

void m6502::CPU::SetZeroAndNegativeFlag(const UI8 Register) {
    PS.Z = (Register == 0);
    PS.N = (Register & 0b10000000) > 0;
}

m6502::UI16 m6502::CPU::LoadPrg(const UI8 *Program, const SI32 Numbytes, MEMORY &memory) {
    UI16 LoadAddress = 0;
    if (Program && Numbytes > 2) {
        SI32 At = 0;
        const UI16 Lo = Program[At++];
        const UI16 Hi = (Program[At++] << 8);
        LoadAddress = Lo | Hi;
        const UI16 Compare = LoadAddress+Numbytes-2;
        for (SI32 i = LoadAddress; i < Compare; ++i) {
            memory.Data[i] = Program[At++];
        }
    }
    return LoadAddress;
}

void m6502::CPU::PrintStatus() const {
    std::cout<<std::dec<<"A:"<<static_cast<int>(A)<<" X:"<<static_cast<int>(X)<<" Y:"<<static_cast<int>(Y)<<"\n";
    std::cout<<std::dec<<"PC:"<<static_cast<int>(PC)<<" SP:"<<static_cast<int>(SP)<<"\n";
    std::cout<<std::dec<<"PS:"<<static_cast<int>(PS.All)<<"\n\n";
}

void m6502::CPU::PrintStatusHex() const {
    std::cout<<std::hex<<"A:0x"<<static_cast<int>(A)<<" X:0x"<<static_cast<int>(X)<<" Y:0x"<<static_cast<int>(Y)<<" ";
    std::cout<<std::hex<<"PC:0x"<<static_cast<int>(PC)<<" SP:0x"<<static_cast<int>(SP)<<" ";
    std::cout<<std::hex<<"PS:0x"<<static_cast<int>(PS.All)<<"\n";
}

m6502::SI32 m6502::CPU::Execute(SI32 cycles, MEMORY &memory) {
    auto LoadRegister = [&cycles, &memory, this] (const UI16 Address, UI8& Register){
        Register = ReadUI8(cycles, Address, memory);
        SetZeroAndNegativeFlag(Register);
    };

    auto AndOp = [&cycles, &memory, this] (const UI16 Address){
        A &= ReadUI8(cycles, Address, memory);
        SetZeroAndNegativeFlag(A);
    };
    auto EorOp = [&cycles, &memory, this] (const UI16 Address){
        A ^= ReadUI8(cycles, Address, memory);
        SetZeroAndNegativeFlag(A);
    };
    auto OraOp = [&cycles, &memory, this] (const UI16 Address){
        A |= ReadUI8(cycles, Address, memory);
        SetZeroAndNegativeFlag(A);
    };

    auto BranchIf = [&cycles, &memory, this] (const bool Test, const bool Expected) {
        const SI8 Offset = FetchSI8(cycles, memory);
        if (Test == Expected ) {
            const UI16 OldPC = PC;
            PC += Offset;
            --cycles;

            // ReSharper disable once CppTooWideScope
            const bool PageChanged = (PC >> 8) != (OldPC >> 8);
            if (PageChanged) {
                cycles -= 2;
            }
        }
    };

    const SI32 CyclesRequested = cycles;
    while (cycles > 0) {
        switch (const UI8 Instruction = FetchUI8(cycles, memory)) {
            //Load Register
            //LDA
            case INS_LDA_IM: //Load Accumulator ImmediateMode
            {
                A = FetchUI8(cycles, memory);
                SetZeroAndNegativeFlag(A);
            }break;
            case INS_LDA_ZP: //Load Accumulator ZeroPageMode
            {
                const UI16 Address = AddrZeroPage(cycles, memory);
                LoadRegister(Address, A);
            }break;
            case INS_LDA_ZPX: //Load Accumulator ZeroPageXMode
            {
                const UI16 Address = AddrZeroPageX(cycles, memory);
                LoadRegister(Address, A);
            }break;
            case INS_LDA_ABS: //Load Accumulator AbsoluteMode
            {
                const UI16 Address = AddrAbsolute(cycles, memory);
                LoadRegister(Address, A);
            }break;
            case INS_LDA_ABSX: //Load Accumulator AbsoluteXMode
            {
                const UI16 Address = AddrAbsoluteX(cycles, memory);
                LoadRegister(Address, A);
            }break;
            case INS_LDA_ABSY: //Load Accumulator AbsoluteYMode
            {
                const UI16 Address = AddrAbsoluteY(cycles, memory);
                LoadRegister(Address, A);
            }break;
            case INS_LDA_INDX: //Load Accumulator IndexedIndirectXMode
            {
                const UI16 Address = AddrIndirectX(cycles, memory);
                LoadRegister(Address, A);
            }break;
            case INS_LDA_INDY: //Load Accumulator IndirectIndexedYMode
            {
                const UI16 Address = AddrIndirectY(cycles, memory);
                LoadRegister(Address, A);
            }break;
            //LDX
            case INS_LDX_IM: //Load X Register ImmediateMode
            {
                X= FetchUI8(cycles, memory);
                SetZeroAndNegativeFlag(X);
            }break;
            case INS_LDX_ZP: //Load X Register ZeroPageMode
            {
                const UI16 Address = AddrZeroPage(cycles, memory);
                LoadRegister(Address, X);
            }break;
            case INS_LDX_ZPY: //Load X Register ZeroPageYMode
            {
                const UI16 Address = AddrZeroPageY(cycles, memory);
                LoadRegister(Address, X);
            }break;
            case INS_LDX_ABS: //Load X Register AbsoluteMode
            {
                const UI16 Address = AddrAbsolute(cycles, memory);
                LoadRegister(Address, X);
            }break;
            case INS_LDX_ABSY: //Load X Register AbsoluteYMode
            {
                const UI16 Address = AddrAbsoluteY(cycles, memory);
                LoadRegister(Address, X);
            }break;
            //LDY
            case INS_LDY_IM: //Load Y Register ImmediateMode
            {
                Y = FetchUI8(cycles, memory);
                SetZeroAndNegativeFlag(Y);
            }break;
            case INS_LDY_ZP: //Load Y Register ZeroPageMode
            {
                const UI16 Address = AddrZeroPage(cycles, memory);
                LoadRegister(Address, Y);
            }break;
            case INS_LDY_ZPX: //Load Y Register ZeroPageXMode
            {
                const UI16 Address = AddrZeroPageX(cycles, memory);
                LoadRegister(Address, Y);
            }break;
            case INS_LDY_ABS: //Load Y Register AbsoluteMode
            {
                const UI16 Address = AddrAbsolute(cycles, memory);
                LoadRegister(Address, Y);
            }break;
            case INS_LDY_ABSX: //Load Y Register AbsoluteXMode
            {
                const UI16 Address = AddrAbsoluteX(cycles, memory);
                LoadRegister(Address, Y);
            }break;

            //Store Registers
            // STA
            case INS_STA_ZP: //Store Accumulator ZeroPageMode
            {
                const UI16 Address = AddrZeroPage(cycles, memory);
                WriteUI8(A, cycles, Address, memory);
            }break;
            case INS_STA_ZPX: //Store Accumulator ZeroPageXMode
            {
                const UI16 Address = AddrZeroPageX(cycles, memory);
                WriteUI8(A, cycles, Address, memory);
            }break;
            case INS_STA_ABS: //Store Accumulator AbsoluteMode
            {
                const UI16 Address = AddrAbsolute(cycles, memory);
                WriteUI8(A, cycles, Address, memory);
            }break;
            case INS_STA_ABSX: //Store Accumulator AbsoluteXMode
            {
                const UI16 Address = AddrAbsoluteX_5(cycles, memory);
                WriteUI8(A, cycles, Address, memory);
            }break;
            case INS_STA_ABSY: //Store Accumulator AbsoluteYMode
            {
                const UI16 Address = AddrAbsoluteY_6(cycles, memory);
                WriteUI8(A, cycles, Address, memory);
            }break;
            case INS_STA_INDX: //Store Accumulator IndexedIndirectXMode
            {
                const UI16 Address = AddrIndirectX(cycles, memory);
                WriteUI8(A, cycles, Address, memory);
            }break;
            case INS_STA_INDY: //Store Accumulator IndirectIndexedYMode
            {
                const UI16 Address = AddrIndirectY_6(cycles, memory);
                WriteUI8(A, cycles, Address, memory);
            }break;
            // STX
            case INS_STX_ZP: //Store X Register ZeroPageMode
            {
                const UI16 Address = AddrZeroPage(cycles, memory);
                WriteUI8(X, cycles, Address, memory);
            }break;
            case INS_STX_ZPY: //Store X Register ZeroPageYMode
            {
                const UI16 Address = AddrZeroPageY(cycles, memory);
                WriteUI8(X, cycles, Address, memory);
            }break;
            case INS_STX_ABS: //Store X Register AbsoluteMode
            {
                const UI16 Address = AddrAbsolute(cycles, memory);
                WriteUI8(X, cycles, Address, memory);
            }break;
            // STY
            case INS_STY_ZP: //Store Y Register ZeroPageMode
            {
                const UI16 Address = AddrZeroPage(cycles, memory);
                WriteUI8(Y, cycles, Address, memory);
            }break;
            case INS_STY_ZPX: //Store Y Register ZeroPageXMode
            {
                const UI16 Address = AddrZeroPageX(cycles, memory);
                WriteUI8(Y, cycles, Address, memory);
            }break;
            case INS_STY_ABS: //Store Y Register AbsoluteMode
            {
                const UI16 Address = AddrAbsolute(cycles, memory);
                WriteUI8(Y, cycles, Address, memory);
            }break;

            // Transfer Registers
            case INS_TAX: //Transfer A to X
            {
                X = A;
                --cycles;
                SetZeroAndNegativeFlag(X);
            }break;
            case INS_TAY: //Transfer A to Y
            {
                Y = A;
                --cycles;
                SetZeroAndNegativeFlag(Y);
            }break;
            case INS_TXA: //Transfer X to A
            {
                A = X;
                --cycles;
                SetZeroAndNegativeFlag(A);
            }break;
            case INS_TYA: //Transfer Y to A
            {
                A = Y;
                --cycles;
                SetZeroAndNegativeFlag(A);
            }break;

            //Stack Operations
            case INS_TSX: //Transfer SP to X register
            {
                X = SP;
                --cycles;
                SetZeroAndNegativeFlag(X);
            }break;
            case INS_TXS: //Transfer X to SP register
            {
                SP = X;
                --cycles;
            }break;
            case INS_PHA: //Push A to stack
            {
                PushUI8toStack( cycles, A, memory);
            }break;
            case INS_PHP: //Push PSF to stack
            {
                PushUI8toStack( cycles, PS.All, memory);
            }break;
            case INS_PLA: //Pull from stack to A
            {
                A = PopUI8fromStack( cycles, memory);
                SetZeroAndNegativeFlag(A);
            }break;
            case INS_PLP: //Pull from stack to PSF
            {
                PS.All = PopUI8fromStack( cycles, memory);
            }break;

            // Increment and Decrement
            // Increment
            case INS_INC_ZP: // Increment data in Memory ZeroPageMode
            {
                const UI16 Address = AddrZeroPage(cycles, memory);
                UI8 Value = ReadUI8(cycles, Address, memory);
                ++Value;
                --cycles;
                WriteUI8(Value, cycles, Address, memory);
                SetZeroAndNegativeFlag(Value);
            }break;
            case INS_INC_ZPX: // Increment data in Memory ZeroPageXMode
            {
                const UI16 Address = AddrZeroPageX(cycles, memory);
                UI8 Value = ReadUI8(cycles, Address, memory);
                ++Value;
                --cycles;
                WriteUI8(Value, cycles, Address, memory);
                SetZeroAndNegativeFlag(Value);
            }break;
            case INS_INC_ABS: // Increment data in Memory AbsoluteMode
            {
                const UI16 Address = AddrAbsolute(cycles, memory);
                UI8 Value = ReadUI8(cycles, Address, memory);
                ++Value;
                --cycles;
                WriteUI8(Value, cycles, Address, memory);
                SetZeroAndNegativeFlag(Value);
            }break;
            case INS_INC_ABSX: // Increment data in Memory AbsoluteXMode
            {
                const UI16 Address = AddrAbsoluteX_5(cycles, memory);
                UI8 Value = ReadUI8(cycles, Address, memory);
                ++Value;
                --cycles;
                WriteUI8(Value, cycles, Address, memory);
                SetZeroAndNegativeFlag(Value);
            }break;
            case INS_INX: // Increment data in X Register
            {
                ++X;
                --cycles;
                SetZeroAndNegativeFlag(X);
            }break;
            case INS_INY: // Increment data in Y Register
            {
                ++Y;
                --cycles;
                SetZeroAndNegativeFlag(Y);
            }break;
            //Decrement
            case INS_DEC_ZP: // Decrement data in Memory ZeroPageMode
            {
                const UI16 Address = AddrZeroPage(cycles, memory);
                UI8 Value = ReadUI8(cycles, Address, memory);
                --Value;
                --cycles;
                WriteUI8(Value, cycles, Address, memory);
                SetZeroAndNegativeFlag(Value);
            }break;
            case INS_DEC_ZPX: // Decrement data in Memory ZeroPageXMode
            {
                const UI16 Address = AddrZeroPageX(cycles, memory);
                UI8 Value = ReadUI8(cycles, Address, memory);
                --Value;
                --cycles;
                WriteUI8(Value, cycles, Address, memory);
                SetZeroAndNegativeFlag(Value);
            }break;
            case INS_DEC_ABS: // Decrement data in Memory AbsoluteMode
            {
                const UI16 Address = AddrAbsolute(cycles, memory);
                UI8 Value = ReadUI8(cycles, Address, memory);
                --Value;
                --cycles;
                WriteUI8(Value, cycles, Address, memory);
                SetZeroAndNegativeFlag(Value);
            }break;
            case INS_DEC_ABSX: // Decrement data in Memory AbsoluteXMode
            {
                const UI16 Address = AddrAbsoluteX_5(cycles, memory);
                UI8 Value = ReadUI8(cycles, Address, memory);
                --Value;
                --cycles;
                WriteUI8(Value, cycles, Address, memory);
                SetZeroAndNegativeFlag(Value);
            }break;
            case INS_DEX: // Decrement data in X Register
            {
                if (X == 0x00) {
                    X = 0xFF;
                    --cycles;
                    SetZeroAndNegativeFlag(X);
                    break;
                }
                else {
                    --X;
                    --cycles;
                    SetZeroAndNegativeFlag(X);
                }
            }break;
            case INS_DEY: // Decrement data in Y Register
            {
                if (Y == 0x00) {
                    Y = 0xFF;
                    --cycles;
                    SetZeroAndNegativeFlag(Y);
                    break;
                }
                else {
                    --Y;
                    --cycles;
                    SetZeroAndNegativeFlag(Y);
                }
            }break;

            // Logical
            //AND
            case INS_AND_IM: //AND to A ImmediateMode
            {
                A &= FetchUI8(cycles, memory);
                SetZeroAndNegativeFlag(A);
            }break;
            case INS_AND_ZP: //AND to A ZeroPageMode
            {
                const UI16 Address = AddrZeroPage(cycles, memory);
                AndOp(Address);
            }break;
            case INS_AND_ZPX: //AND to A ZeroPAgeXMode
            {
                const UI16 Address = AddrZeroPageX(cycles, memory);
                AndOp(Address);
            }break;
            case INS_AND_ABS: //And to A AbsoluteMode
            {
                const UI16 Address = AddrAbsolute(cycles, memory);
                AndOp(Address);
            }break;
            case INS_AND_ABSX: //And to A AbsoluteXMode
            {
                const UI16 Address = AddrAbsoluteX(cycles, memory);
                AndOp(Address);
            }break;
            case INS_AND_ABSY: //And to A AbsoluteYMode
            {
                const UI16 Address = AddrAbsoluteY(cycles, memory);
                AndOp(Address);
            }break;
            case INS_AND_INDX: //And to A IndexedIndirectXMode
            {
                const UI16 Address = AddrIndirectX(cycles, memory);
                AndOp(Address);
            }break;
            case INS_AND_INDY: //And to A IndexedIndirectYMode
            {
                const UI16 Address = AddrIndirectY(cycles, memory);
                AndOp(Address);
            }break;
            //EOR
            case INS_EOR_IM: //EOR to A ImmediateMode
            {
                A ^= FetchUI8(cycles, memory);
                SetZeroAndNegativeFlag(A);
            }break;
            case INS_EOR_ZP: //EOR to A ZeroPageMode
            {
                const UI16 Address = AddrZeroPage(cycles, memory);
                EorOp(Address);
            }break;
            case INS_EOR_ZPX: //EOR to A ZeroPageXMode
            {
                const UI16 Address = AddrZeroPageX(cycles, memory);
                EorOp(Address);
            }break;
            case INS_EOR_ABS: //EOR to A AbsoluteMode
            {
                const UI16 Address = AddrAbsolute(cycles, memory);
                EorOp(Address);
            }break;
            case INS_EOR_ABSX: //EOR to A AbsoluteXMode
            {
                const UI16 Address = AddrAbsoluteX(cycles, memory);
                EorOp(Address);
            }break;
            case INS_EOR_ABSY: //EOR to A AbsoluteYMode
            {
                const UI16 Address = AddrAbsoluteY(cycles, memory);
                EorOp(Address);
            }break;
            case INS_EOR_INDX: //EOR to A IndexedIndirectXMode
            {
                const UI16 Address = AddrIndirectX(cycles, memory);
                EorOp(Address);
            }break;
            case INS_EOR_INDY: //EOR to A IndexedIndirectYMode
            {
                const UI16 Address = AddrIndirectY(cycles, memory);
                EorOp(Address);
            }break;
            //ORA
            case INS_ORA_IM: //OR to A ImmediateMode
            {
                A |= FetchUI8(cycles, memory);
                SetZeroAndNegativeFlag(A);
            }break;
            case INS_ORA_ZP: //OR to A ZeroPageMode
            {
                const UI16 Address = AddrZeroPage(cycles, memory);
                OraOp(Address);
            }break;
            case INS_ORA_ZPX: //OR to A ZeroPageXMode
            {
                const UI16 Address = AddrZeroPageX(cycles, memory);
                OraOp(Address);
            }break;
            case INS_ORA_ABS: //OR to A AbsoluteMode
            {
                const UI16 Address = AddrAbsolute(cycles, memory);
                OraOp(Address);
            }break;
            case INS_ORA_ABSX: //OR to A AbsoluteXMode
            {
                const UI16 Address = AddrAbsoluteX(cycles, memory);
                OraOp(Address);
            }break;
            case INS_ORA_ABSY: //OR to A AbsoluteYMode
            {
                const UI16 Address = AddrAbsoluteY(cycles, memory);
                OraOp(Address);
            }break;
            case INS_ORA_INDX: //OR to A IndexedIndirectXMode
            {
                const UI16 Address = AddrIndirectX(cycles, memory);
                OraOp(Address);
            }break;
            case INS_ORA_INDY: //OR to A IndexedIndirectYMode
            {
                const UI16 Address = AddrIndirectY(cycles, memory);
                OraOp(Address);
            }break;
            //BIT
            case INS_BIT_ZP: //BIT to A ZeroPageMode
            {
                const UI16 Address = AddrZeroPage(cycles, memory);
                const UI8 Value = ReadUI8(cycles, Address, memory);
                PS.Z = !(A & Value);
                PS.N = (Value & NegativeFLagBit) !=0;
                PS.V = (Value & OverFlowFLagBit) !=0;
            }break;
            case INS_BIT_ABS: //BIT to A AbsoluteMode
            {
                const UI16 Address = AddrAbsolute(cycles, memory);
                const UI8 Value = ReadUI8(cycles, Address, memory);
                PS.Z = !(A & Value);
                PS.N = (Value & NegativeFLagBit) !=0;
                PS.V = (Value & OverFlowFLagBit) !=0;
            }break;

            //Branches
            case INS_BCC: //Branch if Carry Flag Not Set
            {
                BranchIf( PS.C, false );
            }break;
            case INS_BCS: //Branch if Carry Flag is Set
            {
                BranchIf( PS.C, true );
            }break;
            case INS_BEQ: //Branch on Equal if Zero Flag is Set
            {
                BranchIf( PS.Z, true );
            }break;
            case INS_BNE: //Branch on Not Equal if Zero Flag is Set
            {
                BranchIf( PS.Z, false );
            }break;
            case INS_BMI: //Branch on Minus if Negative Flag is Set
            {
                BranchIf( PS.N, true );
            }break;
            case INS_BPL: //Branch on Positive if Negative Flag not Set
            {
                BranchIf( PS.N, false );
            }break;
            case INS_BVS: //Branch if Overflow Flag is Set
            {
                BranchIf( PS.V, true );
            }break;
            case INS_BVC: //Branch if Overflow Flag not Set
            {
                BranchIf( PS.V, false );
            }break;

            /*  Note:
                An original 6502 has does not correctly fetch the target address if the indirect vector falls on a page boundary
                (e.g. $xxFF where xx is any value from $00 to $FF). In this case fetches the LSB from $xxFF as expected but takes
                the MSB from $xx00. This is fixed in some later chips like the 65SC02 so for compatibility always ensure the indirect
                vector is not at the end of the page.
            */

            //Jump And Calls
            case INS_JMP_ABS: //Jump to instruction Absolute mode
            {
                const UI16 Address = AddrAbsolute(cycles, memory);
                PC = Address;
            }break;
            case INS_JMP_IND: //Jump to instruction Indirect mode
            {
                UI16 Address = AddrAbsolute(cycles, memory);
                Address = ReadUI16(cycles, Address,memory);
                PC = Address;
            }break;
            case INS_JSR: //Jump SubRoutine Only one mode
            {
                const UI16 SubAddress = FetchUI16(cycles, memory);
                PushPCtoStack(cycles, memory);
                PC = SubAddress;
                --cycles;
            }break;
            case INS_RTS: //Return To SubRoutine Only one mode
            {
                const UI16 ReturnAddress = PopAddressFromStack(cycles, memory);
                PC = ReturnAddress + 1;
                cycles -= 2;
            }break;
            default: {
                //If other errors occur then I blame ReadUI16's --cycles.
                std::cout << "Instruction not handled: " << Instruction << "\n";
                throw -1;
            }
        }
    }
    return CyclesRequested - cycles;
}