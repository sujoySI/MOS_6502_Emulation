#include "M6502Lib/cpu.h"
#include <gtest/gtest.h>

SI32 cycles;
SI32 ExpectedCycles;

class M6502Tests1 : public ::testing::Test {
    public:
        MEMORY mem{};
        CPU cpu{};
        CPU cpuCopy{};
        virtual void SetUp()
        {
            cpu.Reset(mem);
        }

        virtual void TearDown()
        {
        }
};

static void VerifyStatusFlagIDuringLDA(const CPU& cpu, const CPU& cpuCopy) {
    EXPECT_EQ(cpu.SF.B, cpuCopy.SF.B);
    EXPECT_EQ(cpu.SF.C, cpuCopy.SF.C);
    EXPECT_EQ(cpu.SF.D, cpuCopy.SF.D);
    EXPECT_EQ(cpu.SF.I, cpuCopy.SF.I);
    EXPECT_EQ(cpu.SF.V, cpuCopy.SF.V);
}

TEST_F(M6502Tests1, CPU_When_0_Cycles_isGiven) {
    //Given:
    ExpectedCycles = 0;

    //When:
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then::
    EXPECT_EQ(cycles, ExpectedCycles);
}

TEST_F(M6502Tests1, CPU_When_LessThanRequired_CyclesIsGiven) {
    //Given:
    ExpectedCycles = 2;
    mem.Data[0xFFFC] = CPU::INS_LDA_IM;
    mem.Data[0xFFFD] = 0x84;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(1, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
}

TEST_F(M6502Tests1, LDA_IM) {
    //Given:
    ExpectedCycles = 2;
    mem.Data[0xFFFC] = CPU::INS_LDA_IM;
    mem.Data[0xFFFD] = 0x84;

    //When:
    //here in clion debug, A is shown in 132 decimal (grey colour) and '\204' octal (green colour)
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cpu.A, 0x84);
    EXPECT_FALSE(cpu.SF.Z);
    EXPECT_TRUE(cpu.SF.N);
    VerifyStatusFlagIDuringLDA(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}

TEST_F(M6502Tests1, LDA_IM_Affect_ZeroFlag) {
    //Given:
    ExpectedCycles = 2;
    cpu.A = 0x44;
    mem.Data[0xFFFC] = CPU::INS_LDA_IM;
    mem.Data[0xFFFD] = 0x0;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    // EXPECT_EQ(cpu.A, 0x0);
    EXPECT_TRUE(cpu.SF.Z);
    EXPECT_FALSE(cpu.SF.N);
    VerifyStatusFlagIDuringLDA(cpu, cpuCopy);
    // EXPECT_EQ(cycles, ExpectedCycles);
}

TEST_F(M6502Tests1, LDA_ZP) {
    //Given:
    ExpectedCycles = 3;
    mem.Data[0xFFFC] = CPU::INS_LDA_ZP;
    mem.Data[0xFFFD] = 0x42;
    mem.Data[0x0042] = 0x37;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_FALSE(cpu.SF.Z);
    EXPECT_FALSE(cpu.SF.N);
    VerifyStatusFlagIDuringLDA(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}

TEST_F(M6502Tests1, LDA_ZPX) {
    //Given:
    ExpectedCycles = 4;
    cpu.X = 5;
    mem.Data[0xFFFC] = CPU::INS_LDA_ZPX;
    mem.Data[0xFFFD] = 0x42;
    mem.Data[0x0047] = 0x37;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_FALSE(cpu.SF.Z);
    EXPECT_FALSE(cpu.SF.N);
    VerifyStatusFlagIDuringLDA(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}

TEST_F(M6502Tests1, LDA_ZPX_whenitWraps) {
    //Given:
    ExpectedCycles = 4;
    cpu.X = 0xFF;
    mem.Data[0xFFFC] = CPU::INS_LDA_ZPX;
    mem.Data[0xFFFD] = 0x80;
    mem.Data[0x007F] = 0x37;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_FALSE(cpu.SF.Z);
    EXPECT_FALSE(cpu.SF.N);
    VerifyStatusFlagIDuringLDA(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}

TEST_F(M6502Tests1, LDA_ABS) {
    //Given:
    ExpectedCycles = 4;
    cpu.X = 5;
    mem.Data[0xFFFC] = CPU::INS_LDA_ABS;
    mem.Data[0xFFFD] = 0x80;
    mem.Data[0xFFFE] = 0x44;
    mem.Data[0x4480] = 0x37;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_FALSE(cpu.SF.Z);
    EXPECT_FALSE(cpu.SF.N);
    VerifyStatusFlagIDuringLDA(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}

TEST_F(M6502Tests1, LDA_ABSX) {
    //Given:
    ExpectedCycles = 4;
    cpu.X = 1;
    mem.Data[0xFFFC] = CPU::INS_LDA_ABSX;
    mem.Data[0xFFFD] = 0x80;
    mem.Data[0xFFFE] = 0x44;    //0x4402
    mem.Data[0x4481] = 0x37;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_FALSE(cpu.SF.Z);
    EXPECT_FALSE(cpu.SF.N);
    VerifyStatusFlagIDuringLDA(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}

TEST_F(M6502Tests1, LDA_ABSX_WhenItCrossesAPageBoundary) {
    //Given:
    ExpectedCycles = 5;
    cpu.X = 0xFF;
    mem.Data[0xFFFC] = CPU::INS_LDA_ABSX;
    mem.Data[0xFFFD] = 0x02;
    mem.Data[0xFFFE] = 0x44;    //0x4402
    mem.Data[0x4501] = 0x37;    //0x4402+0xFF when it crosses page boundary

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_FALSE(cpu.SF.Z);
    EXPECT_FALSE(cpu.SF.N);
    VerifyStatusFlagIDuringLDA(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}

TEST_F(M6502Tests1, LDA_ABSY) {
    //Given:
    ExpectedCycles = 4;
    cpu.Y = 1;
    mem.Data[0xFFFC] = CPU::INS_LDA_ABSY;
    mem.Data[0xFFFD] = 0x80;
    mem.Data[0xFFFE] = 0x44;    //0x4402
    mem.Data[0x4481] = 0x37;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_FALSE(cpu.SF.Z);
    EXPECT_FALSE(cpu.SF.N);
    VerifyStatusFlagIDuringLDA(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}

TEST_F(M6502Tests1, LDA_ABSY_WhenItCrossesAPageBoundary) {
    //Given:
    ExpectedCycles = 5;
    cpu.Y = 0xFF;
    mem.Data[0xFFFC] = CPU::INS_LDA_ABSY;
    mem.Data[0xFFFD] = 0x02;
    mem.Data[0xFFFE] = 0x44;    //0x4402
    mem.Data[0x4501] = 0x37;    //0x4402+0xFF when it crosses page boundary

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_FALSE(cpu.SF.Z);
    EXPECT_FALSE(cpu.SF.N);
    VerifyStatusFlagIDuringLDA(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}

TEST_F(M6502Tests1, LDA_INDX) {
    //Given:
    ExpectedCycles = 6;
    cpu.X = 0x04;
    mem.Data[0xFFFC] = CPU::INS_LDA_INDX;
    mem.Data[0xFFFD] = 0x02;
    mem.Data[0x0006] = 0x00;    //0x0002+0x0004
    mem.Data[0x0007] = 0x80;    //new address 0x8000
    mem.Data[0x8000] = 0x37;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_FALSE(cpu.SF.Z);
    EXPECT_FALSE(cpu.SF.N);
    VerifyStatusFlagIDuringLDA(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}

TEST_F(M6502Tests1, LDA_INDY) {
    //Given:
    ExpectedCycles = 5;
    cpu.Y = 0x04;
    mem.Data[0xFFFC] = CPU::INS_LDA_INDY;
    mem.Data[0xFFFD] = 0x02;
    mem.Data[0x0002] = 0x00;
    mem.Data[0x0003] = 0x80;
    mem.Data[0x8004] = 0x37;    //0x8000+0x04

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_FALSE(cpu.SF.Z);
    EXPECT_FALSE(cpu.SF.N);
    VerifyStatusFlagIDuringLDA(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}

TEST_F(M6502Tests1, LDA_INDY_WhenItCrossesAPageBoundary) {
    //Given:
    ExpectedCycles = 6;
    cpu.Y = 0xFF;
    mem.Data[0xFFFC] = CPU::INS_LDA_INDY;
    mem.Data[0xFFFD] = 0x02;
    mem.Data[0x0002] = 0x02;
    mem.Data[0x0003] = 0x80;
    mem.Data[0x8101] = 0x37;    //0x8002+0xFF when it crosses page boundary

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_FALSE(cpu.SF.Z);
    EXPECT_FALSE(cpu.SF.N);
    VerifyStatusFlagIDuringLDA(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}