#include "M6502Lib/cpu.h"
#include <gtest/gtest.h>

class M6502StatusFlagChangeTests: public ::testing::Test {
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

TEST_F(M6502StatusFlagChangeTests, INS_CLC)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.PS.C = true;
    mem.Data[0xFF00] = CPU::INS_CLC;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_FALSE( cpu.PS.C );
    EXPECT_EQ( cpu.PS.Z, cpuCopy.PS.Z );
    EXPECT_EQ( cpu.PS.N, cpuCopy.PS.N );
    EXPECT_EQ( cpu.PS.I, cpuCopy.PS.I );
    EXPECT_EQ( cpu.PS.V, cpuCopy.PS.V );
    EXPECT_EQ( cpu.PS.D, cpuCopy.PS.D );
    EXPECT_EQ( cpu.PS.B, cpuCopy.PS.B );
}
TEST_F(M6502StatusFlagChangeTests, INS_CLD)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.PS.D = true;
    mem.Data[0xFF00] = CPU::INS_CLD;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_FALSE( cpu.PS.D );
    EXPECT_EQ( cpu.PS.Z, cpuCopy.PS.Z );
    EXPECT_EQ( cpu.PS.N, cpuCopy.PS.N );
    EXPECT_EQ( cpu.PS.I, cpuCopy.PS.I );
    EXPECT_EQ( cpu.PS.C, cpuCopy.PS.C );
    EXPECT_EQ( cpu.PS.V, cpuCopy.PS.V );
    EXPECT_EQ( cpu.PS.B, cpuCopy.PS.B );
}
TEST_F(M6502StatusFlagChangeTests, INS_CLI)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.PS.I = true;
    mem.Data[0xFF00] = CPU::INS_CLI;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_FALSE( cpu.PS.I );
    EXPECT_EQ( cpu.PS.Z, cpuCopy.PS.Z );
    EXPECT_EQ( cpu.PS.N, cpuCopy.PS.N );
    EXPECT_EQ( cpu.PS.C, cpuCopy.PS.C );
    EXPECT_EQ( cpu.PS.V, cpuCopy.PS.V );
    EXPECT_EQ( cpu.PS.D, cpuCopy.PS.D );
    EXPECT_EQ( cpu.PS.B, cpuCopy.PS.B );
}
TEST_F(M6502StatusFlagChangeTests, INS_CLV)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.PS.V = true;
    mem.Data[0xFF00] = CPU::INS_CLV;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_FALSE( cpu.PS.V );
    EXPECT_EQ( cpu.PS.Z, cpuCopy.PS.Z );
    EXPECT_EQ( cpu.PS.N, cpuCopy.PS.N );
    EXPECT_EQ( cpu.PS.I, cpuCopy.PS.I );
    EXPECT_EQ( cpu.PS.C, cpuCopy.PS.C );
    EXPECT_EQ( cpu.PS.D, cpuCopy.PS.D );
    EXPECT_EQ( cpu.PS.B, cpuCopy.PS.B );
}
TEST_F(M6502StatusFlagChangeTests, INS_SEC)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.PS.C = false;
    mem.Data[0xFF00] = CPU::INS_SEC;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_TRUE( cpu.PS.C );
    EXPECT_EQ( cpu.PS.Z, cpuCopy.PS.Z );
    EXPECT_EQ( cpu.PS.N, cpuCopy.PS.N );
    EXPECT_EQ( cpu.PS.I, cpuCopy.PS.I );
    EXPECT_EQ( cpu.PS.V, cpuCopy.PS.V );
    EXPECT_EQ( cpu.PS.D, cpuCopy.PS.D );
    EXPECT_EQ( cpu.PS.B, cpuCopy.PS.B );
}
TEST_F(M6502StatusFlagChangeTests, INS_SED)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.PS.D = false;
    mem.Data[0xFF00] = CPU::INS_SED;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_TRUE( cpu.PS.D );
    EXPECT_EQ( cpu.PS.Z, cpuCopy.PS.Z );
    EXPECT_EQ( cpu.PS.N, cpuCopy.PS.N );
    EXPECT_EQ( cpu.PS.I, cpuCopy.PS.I );
    EXPECT_EQ( cpu.PS.V, cpuCopy.PS.V );
    EXPECT_EQ( cpu.PS.C, cpuCopy.PS.C );
    EXPECT_EQ( cpu.PS.B, cpuCopy.PS.B );
}
TEST_F(M6502StatusFlagChangeTests, INS_SEI)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.PS.I = false;
    mem.Data[0xFF00] = CPU::INS_SEI;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_TRUE( cpu.PS.I );
    EXPECT_EQ( cpu.PS.Z, cpuCopy.PS.Z );
    EXPECT_EQ( cpu.PS.N, cpuCopy.PS.N );
    EXPECT_EQ( cpu.PS.C, cpuCopy.PS.C );
    EXPECT_EQ( cpu.PS.V, cpuCopy.PS.V );
    EXPECT_EQ( cpu.PS.D, cpuCopy.PS.D );
    EXPECT_EQ( cpu.PS.B, cpuCopy.PS.B );
}