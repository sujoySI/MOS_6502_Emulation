#include "M6502Lib/cpu.h"
#include <gtest/gtest.h>

class M6502BranchTests: public ::testing::Test {
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

TEST_F(M6502BranchTests, INS_BEQ_BranchForwardWhenZeroFlagIsTrue)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 3;//2, 3 or 5?
    cpu.Reset(0xFF00, mem);
    cpu.PS.Z = true;
    mem.Data[0xFF00] = CPU::INS_BEQ;
    mem.Data[0xFF01] = 0x01;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.PC, 0xFF03);
    EXPECT_EQ( cpu.PS.All, cpuCopy.PS.All);
}
TEST_F(M6502BranchTests, INS_BEQ_DoesNotBranchForwardWhenZeroFlagIsFalse)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;//2, 3 or 5?
    cpu.Reset(0xFF00, mem);
    cpu.PS.Z = false;
    mem.Data[0xFF00] = CPU::INS_BEQ;
    mem.Data[0xFF01] = 0x01;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.PC, 0xFF02);
    EXPECT_EQ( cpu.PS.All, cpuCopy.PS.All);
}
TEST_F(M6502BranchTests, INS_BEQ_BranchForwardWhenZeroFlagIsTrueAndNewPage)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 5;//2, 3 or 5?
    cpu.Reset(0xFEFD, mem);
    cpu.PS.Z = true;
    mem.Data[0xFEFD] = CPU::INS_BEQ;
    mem.Data[0xFEFE] = 0x01;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.PC, 0xFF00);
    EXPECT_EQ( cpu.PS.All, cpuCopy.PS.All);
}
TEST_F(M6502BranchTests, INS_BEQ_BranchBackwardWhenZeroFlagIsTrue)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 3;//2, 3 or 5?
    cpu.Reset(0xFFCC, mem);
    cpu.PS.Z = true;
    mem.Data[0xFFCC] = CPU::INS_BEQ;
    mem.Data[0xFFCD] = static_cast<SI8> ( -0x2 );

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.PC, 0xFFCC);
    EXPECT_EQ( cpu.PS.All, cpuCopy.PS.All);
}
TEST_F(M6502BranchTests, INS_BEQ_BranchBackwardWhenZeroFlagIsTrueFromAssemblyCode)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2 + 3;//lda #0 + beq
    cpu.Reset(0xFFCC, mem);
    cpu.PS.Z = true;
    /*
        loop
        lda #0
        beq loop
    */
    mem.Data[0xFFCC] = 0xA9;
    mem.Data[0xFFCC + 1] = 0x00;
    mem.Data[0xFFCC + 2] = 0xF0;
    mem.Data[0xFFCC + 3] = 0xFC;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.PC, 0xFFCC);
    EXPECT_EQ( cpu.PS.All, cpuCopy.PS.All);
}
TEST_F(M6502BranchTests, INS_BNE_BranchForwardWhenZeroFlagIsFalse)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 3;//2, 3 or 5?
    cpu.Reset(0xFF00, mem);
    cpu.PS.Z = false;
    mem.Data[0xFF00] = CPU::INS_BNE;
    mem.Data[0xFF01] = 0x01;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.PC, 0xFF03);
    EXPECT_EQ( cpu.PS.All, cpuCopy.PS.All);
}
TEST_F(M6502BranchTests, INS_BNE_NotBranchForwardWhenZeroFlagIsTrue)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 3;//2, 3 or 5?
    cpu.Reset(0xFF00, mem);
    cpu.PS.Z = false;
    mem.Data[0xFF00] = CPU::INS_BNE;
    mem.Data[0xFF01] = 0x01;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.PC, 0xFF03);
    EXPECT_EQ( cpu.PS.All, cpuCopy.PS.All);
}
TEST_F(M6502BranchTests, INS_BCS_BranchForwardWhenCarryFlagIsTrue)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 3;//2, 3 or 5?
    cpu.Reset(0xFF00, mem);
    cpu.PS.C = true;
    mem.Data[0xFF00] = CPU::INS_BCS;
    mem.Data[0xFF01] = 0x01;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.PC, 0xFF03);
    EXPECT_EQ( cpu.PS.All, cpuCopy.PS.All);
}
TEST_F(M6502BranchTests, INS_BCC_BranchForwardWhenCarryFlagIsFalse)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 3;//2, 3 or 5?
    cpu.Reset(0xFF00, mem);
    cpu.PS.C = false;
    mem.Data[0xFF00] = CPU::INS_BCC;
    mem.Data[0xFF01] = 0x01;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.PC, 0xFF03);
    EXPECT_EQ( cpu.PS.All, cpuCopy.PS.All);
}
TEST_F(M6502BranchTests, INS_BMI_BranchForwardWhenNegativeFlagIsTrue)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 3;//2, 3 or 5?
    cpu.Reset(0xFF00, mem);
    cpu.PS.N = true;
    mem.Data[0xFF00] = CPU::INS_BMI;
    mem.Data[0xFF01] = 0x01;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.PC, 0xFF03);
    EXPECT_EQ( cpu.PS.All, cpuCopy.PS.All);
}
TEST_F(M6502BranchTests, INS_BPL_BranchForwardWhenNegativeFlagIsFalse)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 3;//2, 3 or 5?
    cpu.Reset(0xFF00, mem);
    cpu.PS.N = false;
    mem.Data[0xFF00] = CPU::INS_BPL;
    mem.Data[0xFF01] = 0x01;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.PC, 0xFF03);
    EXPECT_EQ( cpu.PS.All, cpuCopy.PS.All);
}
TEST_F(M6502BranchTests, INS_BVS_BranchForwardWhenOverflowFlagIsTrue)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 3;//2, 3 or 5?
    cpu.Reset(0xFF00, mem);
    cpu.PS.V = true;
    mem.Data[0xFF00] = CPU::INS_BVS;
    mem.Data[0xFF01] = 0x01;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.PC, 0xFF03);
    EXPECT_EQ( cpu.PS.All, cpuCopy.PS.All);
}
TEST_F(M6502BranchTests, INS_BVC_BranchForwardWhenOverflowFlagIsFalse)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 3;//2, 3 or 5?
    cpu.Reset(0xFF00, mem);
    cpu.PS.V = false;
    mem.Data[0xFF00] = CPU::INS_BVC;
    mem.Data[0xFF01] = 0x01;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.PC, 0xFF03);
    EXPECT_EQ( cpu.PS.All, cpuCopy.PS.All);
}