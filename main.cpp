#include <cstdint>
#include <cstring>
#include <iostream>

using namespace std;

using UI8 = uint8_t;
using UI16 = uint16_t;
using UI32 = uint32_t;

union FLAG
{
    UI8 Flags;
    struct
    {
        UI8 C : 1;                              //Carryover Flag    //LSB
        UI8 Z : 1;                              //Zero Flag
        UI8 I : 1;                              //Interrupt Flag
        UI8 D : 1;                              //Decimal Flag
        UI8 B : 1;                              //Break Flag (Only for Stack Values)
        UI8   : 1;                              //Reserved
        UI8 V : 1;                              //Overflow Flag
        UI8 N : 1;                              //Negative Flag     //MSB
    };
};

struct MEMORY
{
    static constexpr UI32 MAX_MEM = 1024 * 64;
    UI8 Data[MAX_MEM];
    void Initialise()
    {
        memset(Data, 0x00, MAX_MEM * sizeof(Data[0]));
    }

    //Write 2 Bytes
    void WriteUI16(UI32& cycles, UI16 value, UI32 address)
    {
        Data[address] = value & 0xFF;
        Data[address + 1] = (value >> 8);
        cycles -= 2;
    }
};

struct CPU
{
    UI16 PC;                                    //Program Counter
    UI8  SP;                                    //Stack Pointer
    UI8   A;                                    //Accumulator or Main register
    UI8   X;                                    //Index register X
    UI8   Y;                                    //Index register Y
    FLAG SF;                                    //Status Flag Register
    void Reset(MEMORY& memory )
    {
        PC = 0xFFFC;
        SP = 0x00;
        SF.Flags = 0X0000;
        A = X = Y = 0x00;
        memory.Initialise();
    }

    UI8 FetchUI8( UI32& cycles, MEMORY& memory )
    {
        UI8 Datas = memory.Data[PC];
        ++PC;
        --cycles;
        return Datas;
    }

    UI16 FetchUI16( UI32& cycles, MEMORY& memory )
    {
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

    UI8 ReadUI8( UI32& cycles, UI8 address, MEMORY& memory )
    {
        UI8 Datas = memory.Data[address];
        --cycles;
        return Datas;
    }

    //OPCODES
    static constexpr UI8 INS_LDA_IM = 0xA9;
    static constexpr UI8 INS_LDA_ZP = 0xA5;
    static constexpr UI8 INS_LDA_ZPX = 0xB5;
    static constexpr UI8 INS_JSR = 0x20;

    //Status
    void LDASetStatus()
    {
        SF.Z = (A == 0);
        SF.N = (A & 0b10000000) > 0;
    }

    void Execute(UI32 cycles, MEMORY& memory)
    {
        while (cycles > 0)
        {
            UI8 Instruction = FetchUI8( cycles, memory );
            switch (Instruction)
            {
                case INS_LDA_IM:                                                //Load Accumulator ImmediateMode
                {
                    UI8 Value = FetchUI8( cycles, memory );
                    A = Value;
                    LDASetStatus();
                } break;
                case INS_LDA_ZP:                                                //Load Accumulator ZeroPageMode
                {
                    UI8 ZeroPageAddress = FetchUI8( cycles, memory );
                    A = ReadUI8(cycles, ZeroPageAddress, memory);
                    LDASetStatus();
                } break;
                case INS_LDA_ZPX:                                               //Load Accumulator ZeroPageXMode
                {
                    UI8 ZeroPageAddress = FetchUI8( cycles, memory );
                    ZeroPageAddress += X;
                    --cycles;
                    A = ReadUI8(cycles, ZeroPageAddress, memory);
                    LDASetStatus();
                } break;
                case INS_JSR:
                {
                    UI16 SubAddress = FetchUI16(cycles, memory);
                    memory.WriteUI16(cycles,PC - 1, SP);
                    ++SP;
                    PC = SubAddress;
                    --cycles;
                } break;
                default:
                {
                    cout<<"Instruction not handled: "<<Instruction<<"\n";
                } break;
            }
        }
    }
};

int main()
{
    MEMORY mem;
    CPU cpu;
    cpu.Reset( mem );
    //StartInline
    mem.Data[0xFFFC] = CPU::INS_JSR;
    mem.Data[0xFFFD] = 0x42;
    mem.Data[0xFFFE] = 0x42;
    mem.Data[0x4242] = CPU::INS_LDA_IM;
    mem.Data[0x4243] = 0x84;
    //EndInline
    cpu.Execute( 9,mem );
    return 0;
}