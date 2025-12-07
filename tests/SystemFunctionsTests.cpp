#include "M6502Lib/cpu.h"
#include <gtest/gtest.h>

class M6502SystemFunctionsTest: public ::testing::Test {
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

TEST_F(M6502SystemFunctionsTest, INS_NOP) {
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    mem.Data[0xFF00] = CPU::INS_NOP;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(cpu.PS.All, cpuCopy.PS.All);
    EXPECT_EQ(cpu.A, cpuCopy.A);
    EXPECT_EQ(cpu.X, cpuCopy.X);
    EXPECT_EQ(cpu.Y, cpuCopy.Y);
    EXPECT_EQ(cpu.SP, cpuCopy.SP);
    EXPECT_EQ(cpu.PC, 0xFF01);
}