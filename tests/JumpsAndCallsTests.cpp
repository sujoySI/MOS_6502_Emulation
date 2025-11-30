#include "M6502Lib/cpu.h"
#include <gtest/gtest.h>

class M6502JumpsAndCallsTests: public ::testing::Test {
public:

    m6502::MEMORY mem{};
    m6502::CPU cpu{};
    m6502::CPU cpuCopy{};
    m6502::SI32 cycles;
    m6502::SI32 ExpectedCycles;
    virtual void SetUp()
    {
        cpu.Reset(0xFFFC, mem);
    }

    virtual void TearDown()
    {
    }
};

TEST_F(M6502JumpsAndCallsTests, INS_JSR_And_INS_RTS)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 6 + 6 + 2; //JSR and RTS use 6 cycles each and LDA_IM uses 2 cycles
    cpu.Reset(0xFF00, mem);
    mem.Data[0xFF00] = CPU::INS_JSR;
    mem.Data[0xFF01] = 0x00;
    mem.Data[0xFF02] = 0x80;
    mem.Data[0x8000] = CPU::INS_RTS;
    mem.Data[0xFF03] = CPU::INS_LDA_IM;
    mem.Data[0xFF04] = 0x42;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.A, 0x42);
    EXPECT_EQ(cpu.SP, cpuCopy.SP);
}

TEST_F(M6502JumpsAndCallsTests, INS_JSR_DoesnotAffectStatusFlags)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 6;
    cpu.Reset(0xFF00, mem);
    mem.Data[0xFF00] = CPU::INS_JSR;
    mem.Data[0xFF01] = 0x00;
    mem.Data[0xFF02] = 0x80;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.PS.All, cpuCopy.PS.All);
    EXPECT_NE(cpu.SP, cpuCopy.SP);
    EXPECT_EQ(cpu.PC, 0x8000);
}

TEST_F(M6502JumpsAndCallsTests, INS_RTS_DoesnotAffectStatusFlags)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 12;
    cpu.Reset(0xFF00, mem);
    mem.Data[0xFF00] = CPU::INS_JSR;
    mem.Data[0xFF01] = 0x00;
    mem.Data[0xFF02] = 0x80;
    mem.Data[0x8000] = CPU::INS_RTS;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.PS.All, cpuCopy.PS.All);
    EXPECT_EQ(cpu.PC, 0xFF03);
}

TEST_F(M6502JumpsAndCallsTests, INS_JMP_ABS)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 3;
    cpu.Reset(0xFF00, mem);
    mem.Data[0xFF00] = CPU::INS_JMP_ABS;
    mem.Data[0xFF01] = 0x00;
    mem.Data[0xFF02] = 0x80;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.PS.All, cpuCopy.PS.All);
    EXPECT_EQ(cpu.SP, cpuCopy.SP);
    EXPECT_EQ(cpu.PC, 0x8000);
}

TEST_F(M6502JumpsAndCallsTests, INS_JMP_IND)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 5;
    cpu.Reset(0xFF00, mem);
    mem.Data[0xFF00] = CPU::INS_JMP_IND;
    mem.Data[0xFF01] = 0x00;
    mem.Data[0xFF02] = 0x80;//0x8000
    mem.Data[0x8000] = 0x00;
    mem.Data[0x8001] = 0x90;//0x9000

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.PS.All, cpuCopy.PS.All);
    EXPECT_EQ(cpu.SP, cpuCopy.SP);
    EXPECT_EQ(cpu.PC, 0x9000);
}