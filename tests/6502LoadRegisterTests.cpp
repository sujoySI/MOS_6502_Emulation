#include "M6502Lib/cpu.h"
#include <gtest/gtest.h>

class M6502LoadRegisterTests : public ::testing::Test {
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

        void TestLoadRegisterImmediate(m6502::UI8 OpcodeToTest, m6502::UI8 m6502::CPU::*RegisterToTest);
        void TestLoadRegisterImmediateAffectsZeroFlag(m6502::UI8 OpcodeToTest,  m6502::UI8 m6502::CPU::*RegisterToTest);
        void TestLoadRegisterZeroPage(m6502::UI8 OpcodeToTest, m6502::UI8 m6502::CPU::*RegisterToTest);
        void TestLoadRegisterZeroPageXorY(m6502::UI8 OpcodeToTest, m6502::UI8 m6502::CPU::*RegisterToTest, m6502::UI8 m6502::CPU::*RegisterToLoadFrom);
        void TestLoadRegisterZeroPageXorYWhenItWraps(m6502::UI8 OpcodeToTest, m6502::UI8 m6502::CPU::*RegisterToTest, m6502::UI8 m6502::CPU::*RegisterToLoadFrom);
        void TestLoadRegisterAbsolute(m6502::UI8 OpcodeToTest, m6502::UI8 m6502::CPU::*RegisterToTest, m6502::UI8 m6502::CPU::*RegisterToLoadFrom);
        void TestLoadRegisterAbsoluteXorY(m6502::UI8 OpcodeToTest, m6502::UI8 m6502::CPU::*RegisterToTest, m6502::UI8 m6502::CPU::*RegisterToLoadFrom);
        void TestLoadRegisterAbsoluteXorYWhenCrossingPageBoundary(m6502::UI8 OpcodeToTest, m6502::UI8 m6502::CPU::*RegisterToTest, m6502::UI8 m6502::CPU::*RegisterToLoadFrom);
};

static void VerifyUnmodifiedFlagsFromLoadRegister(const m6502::CPU& cpu, const m6502::CPU& cpuCopy) {
    EXPECT_EQ(cpu.PSF.B, cpuCopy.PSF.B);
    EXPECT_EQ(cpu.PSF.C, cpuCopy.PSF.C);
    EXPECT_EQ(cpu.PSF.D, cpuCopy.PSF.D);
    EXPECT_EQ(cpu.PSF.I, cpuCopy.PSF.I);
    EXPECT_EQ(cpu.PSF.V, cpuCopy.PSF.V);
}

void M6502LoadRegisterTests::TestLoadRegisterImmediate(const m6502::UI8 OpcodeToTest,  m6502::UI8 m6502::CPU::*RegisterToTest){
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    mem.Data[0xFFFC] = OpcodeToTest;
    mem.Data[0xFFFD] = 0x84;

    //When:
    //here in clion debug, A is shown in 132 decimal (grey colour) and '\204' octal (green colour)
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cpu.*RegisterToTest, 0x84);
    EXPECT_FALSE(cpu.PSF.Z);
    EXPECT_TRUE(cpu.PSF.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}

void M6502LoadRegisterTests::TestLoadRegisterImmediateAffectsZeroFlag(const m6502::UI8 OpcodeToTest,  m6502::UI8 m6502::CPU::*RegisterToTest)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.*RegisterToTest = 0x44;
    mem.Data[0xFFFC] = OpcodeToTest;
    mem.Data[0xFFFD] = 0x0;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_TRUE(cpu.PSF.Z);
    EXPECT_FALSE(cpu.PSF.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
}

void M6502LoadRegisterTests::TestLoadRegisterZeroPage(const m6502::UI8 OpcodeToTest, m6502::UI8 m6502::CPU::*RegisterToTest){
    //Given:
    using namespace m6502;
    ExpectedCycles = 3;
    mem.Data[0xFFFC] = OpcodeToTest;
    mem.Data[0xFFFD] = 0x42;
    mem.Data[0x0042] = 0x37;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cpu.*RegisterToTest, 0x37);
    EXPECT_FALSE(cpu.PSF.Z);
    EXPECT_FALSE(cpu.PSF.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}

void M6502LoadRegisterTests::TestLoadRegisterZeroPageXorY(const m6502::UI8 OpcodeToTest, m6502::UI8 m6502::CPU::*RegisterToTest, m6502::UI8 m6502::CPU::*RegisterToLoadFrom){
    //Given:
    using namespace m6502;
    ExpectedCycles = 4;
    cpu.*RegisterToLoadFrom = 5;
    mem.Data[0xFFFC] = OpcodeToTest;
    mem.Data[0xFFFD] = 0x42;
    mem.Data[0x0047] = 0x37;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cpu.*RegisterToTest, 0x37);
    EXPECT_FALSE(cpu.PSF.Z);
    EXPECT_FALSE(cpu.PSF.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}

void M6502LoadRegisterTests::TestLoadRegisterZeroPageXorYWhenItWraps(const m6502::UI8 OpcodeToTest, m6502::UI8 m6502::CPU::*RegisterToTest, m6502::UI8 m6502::CPU::*RegisterToLoadFrom){
    using namespace m6502;
    ExpectedCycles = 4;
    cpu.*RegisterToLoadFrom = 0xFF;
    mem.Data[0xFFFC] = OpcodeToTest;
    mem.Data[0xFFFD] = 0x80;
    mem.Data[0x007F] = 0x37;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cpu.*RegisterToTest, 0x37);
    EXPECT_FALSE(cpu.PSF.Z);
    EXPECT_FALSE(cpu.PSF.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}

void M6502LoadRegisterTests::TestLoadRegisterAbsolute(const m6502::UI8 OpcodeToTest, m6502::UI8 m6502::CPU::*RegisterToTest, m6502::UI8 m6502::CPU::*RegisterToLoadFrom)
{
    //Given:
    using namespace m6502;
    cpu.PSF.Z = cpu.PSF.N = true;
    ExpectedCycles = 4;
    cpu.*RegisterToLoadFrom = 5;
    mem.Data[0xFFFC] = OpcodeToTest;
    mem.Data[0xFFFD] = 0x80;
    mem.Data[0xFFFE] = 0x44;
    mem.Data[0x4480] = 0x37;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cpu.*RegisterToTest, 0x37);
    EXPECT_FALSE(cpu.PSF.Z);
    EXPECT_FALSE(cpu.PSF.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}

void M6502LoadRegisterTests::TestLoadRegisterAbsoluteXorY(const m6502::UI8 OpcodeToTest, m6502::UI8 m6502::CPU::*RegisterToTest, m6502::UI8 m6502::CPU::*RegisterToLoadFrom)
{
    //Given:
    using namespace m6502;
    cpu.PSF.Z = cpu.PSF.N = true;
    ExpectedCycles = 4;
    cpu.*RegisterToLoadFrom = 1;
    mem.Data[0xFFFC] = OpcodeToTest;
    mem.Data[0xFFFD] = 0x80;
    mem.Data[0xFFFE] = 0x44;    //0x4402
    mem.Data[0x4481] = 0x37;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cpu.*RegisterToTest, 0x37);
    EXPECT_FALSE(cpu.PSF.Z);
    EXPECT_FALSE(cpu.PSF.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}

void M6502LoadRegisterTests::TestLoadRegisterAbsoluteXorYWhenCrossingPageBoundary(const m6502::UI8 OpcodeToTest, m6502::UI8 m6502::CPU::*RegisterToTest, m6502::UI8 m6502::CPU::*RegisterToLoadFrom) {
    //Given:
    using namespace m6502;
    cpu.PSF.Z = cpu.PSF.N = true;
    ExpectedCycles = 5;
    cpu.*RegisterToLoadFrom = 0xFF;
    mem.Data[0xFFFC] = OpcodeToTest;
    mem.Data[0xFFFD] = 0x02;
    mem.Data[0xFFFE] = 0x44;    //0x4402
    mem.Data[0x4501] = 0x37;    //0x4402+0xFF when it crosses page boundary

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cpu.*RegisterToTest, 0x37);
    EXPECT_FALSE(cpu.PSF.Z);
    EXPECT_FALSE(cpu.PSF.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}

//LDA
TEST_F(M6502LoadRegisterTests, CPU_When_0_CyclesIsGiven) {
    //Given:
    using namespace m6502;
    ExpectedCycles = 0;

    //When:
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then::
    EXPECT_EQ(cycles, ExpectedCycles);
}

TEST_F(M6502LoadRegisterTests, CPU_When_LessThanRequired_CyclesIsGiven) {
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    mem.Data[0xFFFC] = CPU::INS_LDA_IM;
    mem.Data[0xFFFD] = 0x84;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(1, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
}

TEST_F(M6502LoadRegisterTests, INS_LDA_IM) {
    using namespace m6502;
    TestLoadRegisterImmediate(CPU::INS_LDA_IM, &CPU::A);
}

TEST_F(M6502LoadRegisterTests, LDA_IM_AffectsZeroFlag) {
    using namespace m6502;
    TestLoadRegisterImmediateAffectsZeroFlag(CPU::INS_LDA_IM, &CPU::A);
}

TEST_F(M6502LoadRegisterTests, INS_LDA_ZP) {
    using namespace m6502;
    TestLoadRegisterZeroPage(CPU::INS_LDA_ZP, &CPU::A);
}

TEST_F(M6502LoadRegisterTests, INS_LDA_ZPX) {
    using namespace m6502;
    TestLoadRegisterZeroPageXorY(CPU::INS_LDA_ZPX, &CPU::A, &CPU::X);
}

TEST_F(M6502LoadRegisterTests, INS_LDA_ZPX_WhenitWraps) {
    using namespace m6502;
    TestLoadRegisterZeroPageXorYWhenItWraps(CPU::INS_LDA_ZPX, &CPU::A, &CPU::X);
}

TEST_F(M6502LoadRegisterTests, INS_LDA_ABS) {
    using namespace m6502;
    TestLoadRegisterAbsolute(CPU::INS_LDA_ABS, &CPU::A, &CPU::X);
}

TEST_F(M6502LoadRegisterTests, INS_LDA_ABSX) {
    using namespace m6502;
    TestLoadRegisterAbsoluteXorY(CPU::INS_LDA_ABSX, &CPU::A, &CPU::X);
}

TEST_F(M6502LoadRegisterTests, INS_LDA_ABSX_WhenItCrossesAPageBoundary) {
    using namespace m6502;
    TestLoadRegisterAbsoluteXorYWhenCrossingPageBoundary(CPU::INS_LDA_ABSX, &CPU::A, &CPU::X);
}

TEST_F(M6502LoadRegisterTests, INS_LDA_ABSY) {
    using namespace m6502;
    TestLoadRegisterAbsoluteXorY(CPU::INS_LDA_ABSY, &CPU::A, &CPU::Y);
}

TEST_F(M6502LoadRegisterTests, INS_LDA_ABSY_WhenItCrossesAPageBoundary) {
    using namespace m6502;
    TestLoadRegisterAbsoluteXorYWhenCrossingPageBoundary(CPU::INS_LDA_ABSY, &CPU::A, &CPU::Y);
}

TEST_F(M6502LoadRegisterTests, INS_LDA_INDX) {
    //Given:
    using namespace m6502;
    cpu.PSF.Z = cpu.PSF.N = true;
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
    EXPECT_FALSE(cpu.PSF.Z);
    EXPECT_FALSE(cpu.PSF.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}

TEST_F(M6502LoadRegisterTests, INS_LDA_INDY) {
    //Given:
    using namespace m6502;
    cpu.PSF.Z = cpu.PSF.N = true;
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
    EXPECT_FALSE(cpu.PSF.Z);
    EXPECT_FALSE(cpu.PSF.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}

TEST_F(M6502LoadRegisterTests, INS_LDA_INDY_WhenItCrossesAPageBoundary) {
    //Given:
    using namespace m6502;
    cpu.PSF.Z = cpu.PSF.N = true;
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
    EXPECT_FALSE(cpu.PSF.Z);
    EXPECT_FALSE(cpu.PSF.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}


//LDX
TEST_F(M6502LoadRegisterTests, INS_LDX_IM) {
    using namespace m6502;
    TestLoadRegisterImmediate(CPU::INS_LDX_IM, &CPU::X);
}

TEST_F(M6502LoadRegisterTests, INS_LDX_IM_AffectsZeroFlag) {
    using namespace m6502;
    TestLoadRegisterImmediateAffectsZeroFlag(CPU::INS_LDX_IM, &CPU::X);
}

TEST_F(M6502LoadRegisterTests, INS_LDX_ZP) {
    using namespace m6502;
    TestLoadRegisterZeroPage(CPU::INS_LDX_ZP, &CPU::X);
}

TEST_F(M6502LoadRegisterTests, INS_LDX_ZPY) {
    using namespace m6502;
    TestLoadRegisterZeroPageXorY(CPU::INS_LDX_ZPY, &CPU::X, &CPU::Y);
}

TEST_F(M6502LoadRegisterTests, INS_LDX_ZPY_WhenitWraps) {
    using namespace m6502;
    TestLoadRegisterZeroPageXorYWhenItWraps(CPU::INS_LDX_ZPY, &CPU::X, &CPU::Y);
}

TEST_F(M6502LoadRegisterTests, INS_LDX_ABS) {
    using namespace m6502;
    TestLoadRegisterAbsolute(CPU::INS_LDX_ABS, &CPU::X, &CPU::Y);
}

TEST_F(M6502LoadRegisterTests, INS_LDX_ABSY) {
    using namespace m6502;
    TestLoadRegisterAbsoluteXorY(CPU::INS_LDX_ABSY, &CPU::X, &CPU::Y);
}

TEST_F(M6502LoadRegisterTests, INS_LDX_ABSY_WhenItCrossesAPageBoundary) {
    using namespace m6502;
    TestLoadRegisterAbsoluteXorYWhenCrossingPageBoundary(CPU::INS_LDX_ABSY, &CPU::X, &CPU::Y);
}


//LDY
TEST_F(M6502LoadRegisterTests, INS_LDY_IM) {
    using namespace m6502;
    TestLoadRegisterImmediate(CPU::INS_LDY_IM, &CPU::Y);
}

TEST_F(M6502LoadRegisterTests, INS_LDY_IM_AffectsZeroFlag) {
    using namespace m6502;
    TestLoadRegisterImmediateAffectsZeroFlag(CPU::INS_LDY_IM, &CPU::Y);
}

TEST_F(M6502LoadRegisterTests, INS_LDY_ZP) {
    using namespace m6502;
    TestLoadRegisterZeroPage(CPU::INS_LDY_ZP, &CPU::Y);
}

TEST_F(M6502LoadRegisterTests, INS_LDY_ZPX) {
    using namespace m6502;
    TestLoadRegisterZeroPageXorY(CPU::INS_LDY_ZPX, &CPU::Y, &CPU::X);
}

TEST_F(M6502LoadRegisterTests, INS_LDY_ZPX_WhenitWraps) {
    using namespace m6502;
    TestLoadRegisterZeroPageXorYWhenItWraps(CPU::INS_LDY_ZPX, &CPU::Y, &CPU::X);
}

TEST_F(M6502LoadRegisterTests, INS_LDY_ABS) {
    using namespace m6502;
    TestLoadRegisterAbsolute(CPU::INS_LDY_ABS, &CPU::Y, &CPU::X);
}

TEST_F(M6502LoadRegisterTests, INS_LDY_ABSX) {
    using namespace m6502;
    TestLoadRegisterAbsoluteXorY(CPU::INS_LDY_ABSX, &CPU::Y, &CPU::X);
}

TEST_F(M6502LoadRegisterTests, INS_LDY_ABSX_WhenItCrossesAPageBoundary) {
    using namespace m6502;
    TestLoadRegisterAbsoluteXorYWhenCrossingPageBoundary(CPU::INS_LDY_ABSX, &CPU::Y, &CPU::X);
}