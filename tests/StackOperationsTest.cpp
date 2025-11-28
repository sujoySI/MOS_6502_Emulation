#include "M6502Lib/cpu.h"
#include <gtest/gtest.h>

class M6502StackOperationsTests: public ::testing::Test {
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

TEST_F(M6502StackOperationsTests, INS_TSX)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.PSF.Z = cpu.PSF.N = true;
    cpu.X = 0x00;
    cpu.SP = 0x01;
    mem.Data[0xFF00] = CPU::INS_TSX;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.X, 0x01);
    EXPECT_FALSE(cpu.PSF.Z);
    EXPECT_FALSE(cpu.PSF.N);
}
TEST_F(M6502StackOperationsTests, INS_TSX_ZeroStackPointer)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.PSF.Z = cpu.PSF.N = true;
    cpu.X = 0x00;
    cpu.SP = 0x00;
    mem.Data[0xFF00] = CPU::INS_TSX;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.X, 0x00);
    EXPECT_TRUE(cpu.PSF.Z);
    EXPECT_FALSE(cpu.PSF.N);
}
TEST_F(M6502StackOperationsTests, INS_TSX_NegativeStackPointer)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.PSF.Z = cpu.PSF.N = false;
    cpu.X = 0x00;
    cpu.SP = 0b10000000;
    mem.Data[0xFF00] = CPU::INS_TSX;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.X, 0b10000000);
    EXPECT_FALSE(cpu.PSF.Z);
    EXPECT_TRUE(cpu.PSF.N);
}
TEST_F(M6502StackOperationsTests, INS_TXS)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.X = 0xFF;
    cpu.SP = 0;
    mem.Data[0xFF00] = CPU::INS_TXS;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.SP, 0xFF);
    EXPECT_EQ(cpu.PSF.All, cpu.PSF.All);
}
TEST_F(M6502StackOperationsTests, INS_PHA)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 3;
    cpu.Reset(0xFF00, mem);
    cpu.A = 0x42;
    mem.Data[0xFF00] = CPU::INS_PHA;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(mem.Data[cpu.SPtoAddress() + 1], cpu.A);
    EXPECT_EQ(cpu.PSF.All, cpu.PSF.All);
    EXPECT_EQ(cpu.SP, 0xFE);
}
TEST_F(M6502StackOperationsTests, INS_PHP)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 3;
    cpu.Reset(0xFF00, mem);
    cpu.PSF.All = 0xCC;
    mem.Data[0xFF00] = CPU::INS_PHP;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(mem.Data[cpu.SPtoAddress() + 1], 0xCC);
    EXPECT_EQ(cpu.PSF.All, cpu.PSF.All);
    EXPECT_EQ(cpu.SP, 0xFE);
}
TEST_F(M6502StackOperationsTests, INS_PLA)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 4;
    cpu.Reset(0xFF00, mem);
    cpu.A = 0x00;
    cpu.SP = 0xFE;
    mem.Data[0x01FF] = 0x42;
    mem.Data[0xFF00] = CPU::INS_PLA;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.A, 0x42);
    EXPECT_EQ(cpu.SP, 0xFF);
}
TEST_F(M6502StackOperationsTests, INS_PLA_ZeroValueFromStack)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 4;
    cpu.Reset(0xFF00, mem);
    cpu.PSF.Z = false;
    cpu.PSF.N = true;
    cpu.A = 0x42;
    cpu.SP = 0xFE;
    mem.Data[0x01FF] = 0x00;
    mem.Data[0xFF00] = CPU::INS_PLA;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_TRUE(cpu.PSF.Z);
    EXPECT_FALSE(cpu.PSF.N);
    EXPECT_EQ(cpu.SP, 0xFF);
}
TEST_F(M6502StackOperationsTests, INS_PLA_NegativeValueFromStack)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 4;
    cpu.Reset(0xFF00, mem);
    cpu.PSF.N = false;
    cpu.PSF.Z = true;
    cpu.A = 0x42;
    cpu.SP = 0xFE;
    mem.Data[0x01FF] = 0b10000001;
    mem.Data[0xFF00] = CPU::INS_PLA;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.A, 0b10000001);
    EXPECT_TRUE(cpu.PSF.N);
    EXPECT_FALSE(cpu.PSF.Z);
    EXPECT_EQ(cpu.SP, 0xFF);
}
TEST_F(M6502StackOperationsTests, INS_PLP)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 4;
    cpu.Reset(0xFF00, mem);
    cpu.PSF.All = 0x00;
    cpu.SP = 0xFE;
    mem.Data[0x01FF] = 0x42;
    mem.Data[0xFF00] = CPU::INS_PLP;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.PSF.All, 0x42);
}