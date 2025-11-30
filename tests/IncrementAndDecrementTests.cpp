#include "M6502Lib/cpu.h"
#include <gtest/gtest.h>

class M6502IncrementAndDecrementTests : public ::testing::Test {
    public:
        m6502::MEMORY mem{};
        m6502::CPU cpu{};
        m6502::CPU cpuCopy{};
        m6502::SI32 cycles{};
        m6502::SI32 ExpectedCycles{};
        virtual void SetUp()
        {
            cpu.Reset(0xFFFC, mem);
        }

        virtual void TearDown()
        {

        }
};

static void VerifyUnmodifiedFlagsFromIncrementAndDecrement(const m6502::CPU& cpu, const m6502::CPU& cpuCopy) {
    EXPECT_EQ(cpu.PS.B, cpuCopy.PS.B);
    EXPECT_EQ(cpu.PS.C, cpuCopy.PS.C);
    EXPECT_EQ(cpu.PS.D, cpuCopy.PS.D);
    EXPECT_EQ(cpu.PS.I, cpuCopy.PS.I);
    EXPECT_EQ(cpu.PS.V, cpuCopy.PS.V);
}

//Increment
TEST_F(M6502IncrementAndDecrementTests, INS_INC_ZP)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 5;
    cpu.Reset(0xFF00, mem);
    cpu.PS.Z = true;
    cpu.PS.N = true;
    mem.Data[0xFF00] = CPU::INS_INC_ZP;
    mem.Data[0xFF01] = 0x42;
    mem.Data[0x0042] = 0x57;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(mem.Data[0x0042], 0x58);
    EXPECT_FALSE(cpu.PS.Z);
    EXPECT_FALSE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromIncrementAndDecrement(cpu, cpuCopy);
}
TEST_F(M6502IncrementAndDecrementTests, INS_INC_ZPX)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 6;
    cpu.Reset(0xFF00, mem);
    cpu.PS.Z = true;
    cpu.PS.N = true;
    cpu.X = 0x10;
    mem.Data[0xFF00] = CPU::INS_INC_ZPX;
    mem.Data[0xFF01] = 0x42;
    mem.Data[0x0052] = 0x57; //0x42 + 0x10

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(mem.Data[0x0052], 0x58);
    EXPECT_FALSE(cpu.PS.Z);
    EXPECT_FALSE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromIncrementAndDecrement(cpu, cpuCopy);
}
TEST_F(M6502IncrementAndDecrementTests, INS_INC_ABS)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 6;
    cpu.Reset(0xFF00, mem);
    cpu.PS.Z = true;
    cpu.PS.N = true;
    // cpu.X = 0x10;
    mem.Data[0xFF00] = CPU::INS_INC_ABS;
    mem.Data[0xFF01] = 0x00;
    mem.Data[0xFF02] = 0x80;
    mem.Data[0x8000] = 0x57;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(mem.Data[0x8000], 0x58);
    EXPECT_FALSE(cpu.PS.Z);
    EXPECT_FALSE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromIncrementAndDecrement(cpu, cpuCopy);
}
TEST_F(M6502IncrementAndDecrementTests, INS_INC_ABSX)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 7;
    cpu.Reset(0xFF00, mem);
    cpu.PS.Z = true;
    cpu.PS.N = true;
    cpu.X = 0x10;
    mem.Data[0xFF00] = CPU::INS_INC_ABSX;
    mem.Data[0xFF01] = 0x00;
    mem.Data[0xFF02] = 0x80;
    mem.Data[0x8010] = 0x57; //0x8000 + //0x10

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(mem.Data[0x8010], 0x58);
    EXPECT_FALSE(cpu.PS.Z);
    EXPECT_FALSE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromIncrementAndDecrement(cpu, cpuCopy);
}
TEST_F(M6502IncrementAndDecrementTests, INS_INX_ZeroValue)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.X = 0x00;
    cpu.PS.Z = true;
    cpu.PS.N = true;
    mem.Data[0xFF00] = CPU::INS_INX;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.X, 0x01);
    EXPECT_FALSE(cpu.PS.Z);
    EXPECT_FALSE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromIncrementAndDecrement(cpu, cpuCopy);
}
TEST_F(M6502IncrementAndDecrementTests, INS_INX_255Value)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.X = 0xFF;
    cpu.PS.Z = false;
    cpu.PS.N = true;
    mem.Data[0xFF00] = CPU::INS_INX;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.X, 0x00);
    EXPECT_TRUE(cpu.PS.Z);
    EXPECT_FALSE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromIncrementAndDecrement(cpu, cpuCopy);
}
TEST_F(M6502IncrementAndDecrementTests, INS_INX_NegativeValue)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.X = 0b10001000;
    cpu.PS.Z = true;
    cpu.PS.N = false;
    mem.Data[0xFF00] = CPU::INS_INX;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.X, 0b10001001);
    EXPECT_FALSE(cpu.PS.Z);
    EXPECT_TRUE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromIncrementAndDecrement(cpu, cpuCopy);
}
TEST_F(M6502IncrementAndDecrementTests, INS_INY_ZeroValue)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.Y = 0x00;
    cpu.PS.Z = true;
    cpu.PS.N = true;
    mem.Data[0xFF00] = CPU::INS_INY;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.Y, 0x01);
    EXPECT_FALSE(cpu.PS.Z);
    EXPECT_FALSE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromIncrementAndDecrement(cpu, cpuCopy);
}
TEST_F(M6502IncrementAndDecrementTests, INS_INY_255Value)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.Y = 0xFF;
    cpu.PS.Z = false;
    cpu.PS.N = true;
    mem.Data[0xFF00] = CPU::INS_INY;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.Y, 0x00);
    EXPECT_TRUE(cpu.PS.Z);
    EXPECT_FALSE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromIncrementAndDecrement(cpu, cpuCopy);
}
TEST_F(M6502IncrementAndDecrementTests, INS_INY_NegativeValue)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.Y = 0b10001000;
    cpu.PS.Z = true;
    cpu.PS.N = false;
    mem.Data[0xFF00] = CPU::INS_INY;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.Y, 0b10001001);
    EXPECT_FALSE(cpu.PS.Z);
    EXPECT_TRUE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromIncrementAndDecrement(cpu, cpuCopy);
}
//Decrement
TEST_F(M6502IncrementAndDecrementTests, INS_DEC_ZP)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 5;
    cpu.Reset(0xFF00, mem);
    cpu.PS.Z = true;
    cpu.PS.N = true;
    mem.Data[0xFF00] = CPU::INS_DEC_ZP;
    mem.Data[0xFF01] = 0x42;
    mem.Data[0x0042] = 0x57;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(mem.Data[0x0042], 0x56);
    EXPECT_FALSE(cpu.PS.Z);
    EXPECT_FALSE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromIncrementAndDecrement(cpu, cpuCopy);
}
TEST_F(M6502IncrementAndDecrementTests, INS_DEC_ZPX)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 6;
    cpu.Reset(0xFF00, mem);
    cpu.PS.Z = true;
    cpu.PS.N = true;
    cpu.X = 0x10;
    mem.Data[0xFF00] = CPU::INS_DEC_ZPX;
    mem.Data[0xFF01] = 0x42;
    mem.Data[0x0052] = 0x57; //0x42 + 0x10

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(mem.Data[0x0052], 0x56);
    EXPECT_FALSE(cpu.PS.Z);
    EXPECT_FALSE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromIncrementAndDecrement(cpu, cpuCopy);
}
TEST_F(M6502IncrementAndDecrementTests, INS_DEC_ABS)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 6;
    cpu.Reset(0xFF00, mem);
    cpu.PS.Z = true;
    cpu.PS.N = true;
    // cpu.X = 0x10;
    mem.Data[0xFF00] = CPU::INS_DEC_ABS;
    mem.Data[0xFF01] = 0x00;
    mem.Data[0xFF02] = 0x80;
    mem.Data[0x8000] = 0x57;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(mem.Data[0x8000], 0x56);
    EXPECT_FALSE(cpu.PS.Z);
    EXPECT_FALSE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromIncrementAndDecrement(cpu, cpuCopy);
}
TEST_F(M6502IncrementAndDecrementTests, INS_DEC_ABSX)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 7;
    cpu.Reset(0xFF00, mem);
    cpu.PS.Z = true;
    cpu.PS.N = true;
    cpu.X = 0x10;
    mem.Data[0xFF00] = CPU::INS_DEC_ABSX;
    mem.Data[0xFF01] = 0x00;
    mem.Data[0xFF02] = 0x80;
    mem.Data[0x8010] = 0x57; //0x8000 + //0x10

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(mem.Data[0x8010], 0x56);
    EXPECT_FALSE(cpu.PS.Z);
    EXPECT_FALSE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromIncrementAndDecrement(cpu, cpuCopy);
}
TEST_F(M6502IncrementAndDecrementTests, INS_DEX_ZeroValue)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.X = 0x00;
    cpu.PS.Z = true;
    cpu.PS.N = false;
    mem.Data[0xFF00] = CPU::INS_DEX;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.X, 0xFF);
    EXPECT_FALSE(cpu.PS.Z);
    EXPECT_TRUE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromIncrementAndDecrement(cpu, cpuCopy);
}
TEST_F(M6502IncrementAndDecrementTests, INS_DEX_255Value)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.X = 0xFF;
    cpu.PS.Z = true;
    cpu.PS.N = false;
    mem.Data[0xFF00] = CPU::INS_DEX;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.X, 0xFE);
    EXPECT_FALSE(cpu.PS.Z);
    EXPECT_TRUE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromIncrementAndDecrement(cpu, cpuCopy);
}
TEST_F(M6502IncrementAndDecrementTests, INS_DEX_NegativeValue)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.X = 0b10001000;
    cpu.PS.Z = true;
    cpu.PS.N = false;
    mem.Data[0xFF00] = CPU::INS_DEX;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.X, 0b10000111);
    EXPECT_FALSE(cpu.PS.Z);
    EXPECT_TRUE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromIncrementAndDecrement(cpu, cpuCopy);
}
TEST_F(M6502IncrementAndDecrementTests, INS_DEY_ZeroValue)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.Y = 0x00;
    cpu.PS.Z = true;
    cpu.PS.N = false;
    mem.Data[0xFF00] = CPU::INS_DEY;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.Y, 0xFF);
    EXPECT_FALSE(cpu.PS.Z);
    EXPECT_TRUE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromIncrementAndDecrement(cpu, cpuCopy);
}
TEST_F(M6502IncrementAndDecrementTests, INS_DEY_255Value)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.Y = 0xFF;
    cpu.PS.Z = true;
    cpu.PS.N = false;
    mem.Data[0xFF00] = CPU::INS_DEY;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.Y, 0xFE);
    EXPECT_FALSE(cpu.PS.Z);
    EXPECT_TRUE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromIncrementAndDecrement(cpu, cpuCopy);
}
TEST_F(M6502IncrementAndDecrementTests, INS_DEY_NegativeValue)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.Y = 0b10001000;
    cpu.PS.Z = true;
    cpu.PS.N = false;
    mem.Data[0xFF00] = CPU::INS_DEY;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.Y, 0b10000111);
    EXPECT_FALSE(cpu.PS.Z);
    EXPECT_TRUE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromIncrementAndDecrement(cpu, cpuCopy);
}

//Prg
// TEST_F(M6502IncrementAndDecrementTests, TestLoadProgramIntoCorrectAreaOfMemory) {
//     //Given:
//     using namespace m6502;
//
//     //When:
//     m6502::CPU::LoadPrg(TestPrg, NumBytesInPrg, mem);
//
//     //Then:
//     EXPECT_EQ(mem.Data[0x0FFF] ,0x00);
//     EXPECT_EQ(mem.Data[0x1000] ,0xA9);
//     EXPECT_EQ(mem.Data[0x1001] ,0xFF);
//     EXPECT_EQ(mem.Data[0x1002] ,0x85);
//     /* ****************** */
//     EXPECT_EQ(mem.Data[0x1009] ,0x4C);
//     EXPECT_EQ(mem.Data[0x100A] ,0x02);
//     EXPECT_EQ(mem.Data[0x100B] ,0x10);
//     EXPECT_EQ(mem.Data[0x100C] ,0x00);
// }
TEST_F(M6502IncrementAndDecrementTests, TestLoadProgramThatCanIncMemory) {
    //Given:
    /*
        TestPrgOrignal Input

        * = $1000

        lda #$00
        sta $42

        start
        inc $42
        lda $42
        inx
        jmp start

    */
    using namespace m6502;
    static m6502::UI8 TestPrg[] = {
        0x00, 0x10, 0xA9, 0x00, 0x85, 0x42, 0xE6,
        0x42, 0xA6, 0x42, 0xE8, 0x4C, 0x04, 0x10 };
    static constexpr m6502::SI32 NumBytesInPrg = sizeof(TestPrg);

    //When:
    const UI16 StartAddress = cpu.LoadPrg(TestPrg, NumBytesInPrg, mem);
    cpu.PC = StartAddress;

    //Then:
    // std::cout<<"\n";
    for (SI32 clock = 1000; clock > 0;) {
        clock -= cpu.Execute(1, mem);
        // cpu.PrintStatusHex();
    }
}