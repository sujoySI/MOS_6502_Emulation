#include "M6502Lib/cpu.h"
#include <gtest/gtest.h>

class M6502TransferRegisterTest: public ::testing::Test {
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

static void VerifyUnmodifiedFlagsFromBranch(const m6502::CPU& cpu, const m6502::CPU& cpuCopy) {
    EXPECT_EQ(cpu.PS.B, cpuCopy.PS.B);
    EXPECT_EQ(cpu.PS.C, cpuCopy.PS.C);
    EXPECT_EQ(cpu.PS.D, cpuCopy.PS.D);
    EXPECT_EQ(cpu.PS.I, cpuCopy.PS.I);
    EXPECT_EQ(cpu.PS.V, cpuCopy.PS.V);
}

//TAX
TEST_F(M6502TransferRegisterTest, INS_TAX_NotNegativeAndNotZero)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.A = 0x42;
    cpu.X = 0x32;
    cpu.PS.Z = true;
    cpu.PS.N = true;
    mem.Data[0xFF00] = CPU::INS_TAX;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.A, 0x42);
    EXPECT_EQ(cpu.X, 0x42);
    EXPECT_FALSE(cpu.PS.Z);
    EXPECT_FALSE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromBranch(cpu, cpuCopy);
}
TEST_F(M6502TransferRegisterTest, INS_TAX_NotNegativeAndZero)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.A = 0x00;
    cpu.X = 0x32;
    cpu.PS.Z = false;
    cpu.PS.N = true;
    mem.Data[0xFF00] = CPU::INS_TAX;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_EQ(cpu.X, 0x00);
    EXPECT_TRUE(cpu.PS.Z);
    EXPECT_FALSE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromBranch(cpu, cpuCopy);
}
TEST_F(M6502TransferRegisterTest, INS_TAX_Negative)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.A = 0b10001011;
    cpu.X = 0x32;
    cpu.PS.Z = true;
    cpu.PS.N = false;
    mem.Data[0xFF00] = CPU::INS_TAX;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.A, 0b10001011);
    EXPECT_EQ(cpu.X, 0b10001011);
    EXPECT_FALSE(cpu.PS.Z);
    EXPECT_TRUE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromBranch(cpu, cpuCopy);
}
//TAY
TEST_F(M6502TransferRegisterTest, INS_TAY_NotNegativeAndNotZero)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.A = 0x42;
    cpu.Y = 0x32;
    cpu.PS.Z = true;
    cpu.PS.N = true;
    mem.Data[0xFF00] = CPU::INS_TAY;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.A, 0x42);
    EXPECT_EQ(cpu.Y, 0x42);
    EXPECT_FALSE(cpu.PS.Z);
    EXPECT_FALSE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromBranch(cpu, cpuCopy);
}
TEST_F(M6502TransferRegisterTest, INS_TAY_NotNegativeAndZero)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.A = 0x00;
    cpu.Y = 0x32;
    cpu.PS.Z = false;
    cpu.PS.N = true;
    mem.Data[0xFF00] = CPU::INS_TAY;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_EQ(cpu.Y, 0x00);
    EXPECT_TRUE(cpu.PS.Z);
    EXPECT_FALSE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromBranch(cpu, cpuCopy);
}
TEST_F(M6502TransferRegisterTest, INS_TAY_Negative)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.A = 0b10001011;
    cpu.Y = 0x32;
    cpu.PS.Z = true;
    cpu.PS.N = false;
    mem.Data[0xFF00] = CPU::INS_TAY;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.A, 0b10001011);
    EXPECT_EQ(cpu.Y, 0b10001011);
    EXPECT_FALSE(cpu.PS.Z);
    EXPECT_TRUE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromBranch(cpu, cpuCopy);
}
//TXA
TEST_F(M6502TransferRegisterTest, INS_TXA_NotNegativeAndNotZero)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.X = 0x42;
    cpu.A = 0x32;
    cpu.PS.Z = true;
    cpu.PS.N = true;
    mem.Data[0xFF00] = CPU::INS_TXA;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.A, 0x42);
    EXPECT_EQ(cpu.X, 0x42);
    EXPECT_FALSE(cpu.PS.Z);
    EXPECT_FALSE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromBranch(cpu, cpuCopy);
}
TEST_F(M6502TransferRegisterTest, INS_TXA_NotNegativeAndZero)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.X = 0x00;
    cpu.A = 0x32;
    cpu.PS.Z = false;
    cpu.PS.N = true;
    mem.Data[0xFF00] = CPU::INS_TXA;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_EQ(cpu.X, 0x00);
    EXPECT_TRUE(cpu.PS.Z);
    EXPECT_FALSE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromBranch(cpu, cpuCopy);
}
TEST_F(M6502TransferRegisterTest, INS_TXA_Negative)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.X = 0b10001011;
    cpu.A = 0x32;
    cpu.PS.Z = false;
    cpu.PS.N = false;
    mem.Data[0xFF00] = CPU::INS_TXA;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.A, 0b10001011);
    EXPECT_EQ(cpu.X, 0b10001011);
    EXPECT_FALSE(cpu.PS.Z);
    EXPECT_TRUE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromBranch(cpu, cpuCopy);
}
//TAY
TEST_F(M6502TransferRegisterTest, INS_TYA_NotNegativeAndNotZero)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.Y = 0x42;
    cpu.A = 0x32;
    cpu.PS.Z = true;
    cpu.PS.N = true;
    mem.Data[0xFF00] = CPU::INS_TYA;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.Y, 0x42);
    EXPECT_EQ(cpu.A, 0x42);
    EXPECT_FALSE(cpu.PS.Z);
    EXPECT_FALSE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromBranch(cpu, cpuCopy);
}
TEST_F(M6502TransferRegisterTest, INS_TYA_NotNegativeAndZero)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.Y = 0x00;
    cpu.A = 0x32;
    cpu.PS.Z = false;
    cpu.PS.N = true;
    mem.Data[0xFF00] = CPU::INS_TYA;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_EQ(cpu.Y, 0x00);
    EXPECT_TRUE(cpu.PS.Z);
    EXPECT_FALSE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromBranch(cpu, cpuCopy);
}
TEST_F(M6502TransferRegisterTest, INS_TYA_Negative)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.Y = 0b10001011;
    cpu.A = 0x32;
    cpu.PS.Z = false;
    cpu.PS.N = true;
    mem.Data[0xFF00] = CPU::INS_TYA;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.A, 0b10001011);
    EXPECT_EQ(cpu.Y, 0b10001011);
    EXPECT_FALSE(cpu.PS.Z);
    EXPECT_TRUE(cpu.PS.N);
    VerifyUnmodifiedFlagsFromBranch(cpu, cpuCopy);
}