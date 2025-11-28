#include "M6502Lib/cpu.h"
#include <gtest/gtest.h>

class M6502StoreRegisterTests : public ::testing::Test {
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

        void TestStoreRegisterZeroPage(m6502::UI8 OpcodeToTest,  m6502::UI8 m6502::CPU::*RegisterToTest);
        void TestStoreRegisterZeroPageXorY(m6502::UI8 OpcodeToTest,  m6502::UI8 m6502::CPU::*RegisterToTest, m6502::UI8 m6502::CPU::*RegisterToLoadFrom);
        void TestStoreRegisterAbsolute(m6502::UI8 OpcodeToTest,  m6502::UI8 m6502::CPU::*RegisterToTest);
        void TestStoreRegisterAbsoluteXorY(m6502::UI8 OpcodeToTest,  m6502::UI8 m6502::CPU::*RegisterToTest, m6502::UI8 m6502::CPU::*RegisterToLoadFrom);
};

static void VerifyUnmodifiedFlagsFromStoreRegister(const m6502::CPU& cpu, const m6502::CPU& cpuCopy) {
    EXPECT_EQ(cpu.PSF.B, cpuCopy.PSF.B);
    EXPECT_EQ(cpu.PSF.C, cpuCopy.PSF.C);
    EXPECT_EQ(cpu.PSF.D, cpuCopy.PSF.D);
    EXPECT_EQ(cpu.PSF.I, cpuCopy.PSF.I);
    EXPECT_EQ(cpu.PSF.V, cpuCopy.PSF.V);
    EXPECT_EQ(cpu.PSF.Z, cpuCopy.PSF.Z);
    EXPECT_EQ(cpu.PSF.N, cpuCopy.PSF.N);
}

void M6502StoreRegisterTests::TestStoreRegisterZeroPage(const m6502::UI8 OpcodeToTest,  m6502::UI8 m6502::CPU::*RegisterToTest) {
    //Given:
    using namespace m6502;
    ExpectedCycles = 3;
    cpu.*RegisterToTest = 0x2F;
    mem.Data[0xFFFC] = OpcodeToTest;
    mem.Data[0xFFFD] = 0x80;
    mem.Data[0x0080] = 0x00;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(mem.Data[0x0080], cpu.*RegisterToTest);
    VerifyUnmodifiedFlagsFromStoreRegister(cpu, cpuCopy);
}

void M6502StoreRegisterTests::TestStoreRegisterZeroPageXorY(const m6502::UI8 OpcodeToTest,  m6502::UI8 m6502::CPU::*RegisterToTest, m6502::UI8 m6502::CPU::*RegisterToLoadFrom) {
    //Given:
    using namespace m6502;
    ExpectedCycles = 4;
    cpu.*RegisterToLoadFrom = 0x0F;
    cpu.*RegisterToTest = 0x42;
    mem.Data[0xFFFC] = OpcodeToTest;
    mem.Data[0xFFFD] = 0x80;
    mem.Data[0x008F] = 0x00;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(mem.Data[0x008F], cpu.*RegisterToTest);
    VerifyUnmodifiedFlagsFromStoreRegister(cpu, cpuCopy);
}

void M6502StoreRegisterTests::TestStoreRegisterAbsolute(const m6502::UI8 OpcodeToTest,  m6502::UI8 m6502::CPU::*RegisterToTest) {
    //Given:
    using namespace m6502;
    ExpectedCycles = 4;
    cpu.*RegisterToTest = 0x2F;
    mem.Data[0xFFFC] = OpcodeToTest;
    mem.Data[0xFFFD] = 0x00;
    mem.Data[0xFFFE] = 0x80;
    mem.Data[0x8000] = 0x00;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(mem.Data[0x8000], cpu.*RegisterToTest);
    VerifyUnmodifiedFlagsFromStoreRegister(cpu, cpuCopy);
}

void M6502StoreRegisterTests::TestStoreRegisterAbsoluteXorY(const m6502::UI8 OpcodeToTest,  m6502::UI8 m6502::CPU::*RegisterToTest, m6502::UI8 m6502::CPU::*RegisterToLoadFrom) {
    //Given:
    using namespace m6502;
    ExpectedCycles = 5;
    cpu.*RegisterToTest = 0x42;
    cpu.*RegisterToLoadFrom = 0x0F;
    mem.Data[0xFFFC] = OpcodeToTest;
    mem.Data[0xFFFD] = 0x00;
    mem.Data[0xFFFE] = 0x80;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(mem.Data[0x800F], cpu.*RegisterToTest);
    VerifyUnmodifiedFlagsFromStoreRegister(cpu, cpuCopy);
}

//STA
TEST_F(M6502StoreRegisterTests, INS_STA_ZP) {
    using namespace m6502;
    TestStoreRegisterZeroPage(CPU::INS_STA_ZP, &CPU::A);
}

TEST_F(M6502StoreRegisterTests, INS_STA_ZPX) {
    using namespace m6502;
    TestStoreRegisterZeroPageXorY(CPU::INS_STA_ZPX, &CPU::A, &CPU::X);
}


TEST_F(M6502StoreRegisterTests, INS_STA_ABS) {
    using namespace m6502;
    TestStoreRegisterAbsolute(CPU::INS_STA_ABS, &CPU::A);
}

TEST_F(M6502StoreRegisterTests, INS_STA_ABSX) {
    using namespace m6502;
    TestStoreRegisterAbsoluteXorY(CPU::INS_STA_ABSX, &CPU::A, &CPU::X);
}

TEST_F(M6502StoreRegisterTests, INS_STA_ABSY) {
    using namespace m6502;
    TestStoreRegisterAbsoluteXorY(CPU::INS_STA_ABSY, &CPU::A, &CPU::Y);
}

TEST_F(M6502StoreRegisterTests, INS_STA_INDX) {
    //Given:
    using namespace m6502;
    ExpectedCycles = 6;
    cpu.A = 0x42;
    cpu.X = 0x0F;
    mem.Data[0xFFFC] = CPU::INS_STA_INDX;
    mem.Data[0xFFFD] = 0x20;
    mem.Data[0x002F] = 0x00;
    mem.Data[0x0030] = 0x80;
    mem.Data[0x8000 + 0x0F] = 0x00;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(mem.Data[0x8000], 0x42);
    VerifyUnmodifiedFlagsFromStoreRegister(cpu, cpuCopy);
}

TEST_F(M6502StoreRegisterTests, INS_STA_INDY) {
    //Given:
    using namespace m6502;
    ExpectedCycles = 6;
    cpu.A = 0x42;
    cpu.Y = 0x0F;
    mem.Data[0xFFFC] = CPU::INS_STA_INDY;
    mem.Data[0xFFFD] = 0x20;
    mem.Data[0x0020] = 0x00;
    mem.Data[0x0021] = 0x80;
    mem.Data[0x8000 + 0x0F] = 0x00;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    EXPECT_EQ(mem.Data[0x8000 + 0x0F], 0x42);
    VerifyUnmodifiedFlagsFromStoreRegister(cpu, cpuCopy);
}

//STX
TEST_F(M6502StoreRegisterTests, INS_STX_ZP) {
    using namespace m6502;
    TestStoreRegisterZeroPage(CPU::INS_STX_ZP, &CPU::X);
}

TEST_F(M6502StoreRegisterTests, INS_STX_ZPY) {
    using namespace m6502;
    TestStoreRegisterZeroPageXorY(CPU::INS_STX_ZPY, &CPU::X, &CPU::Y);
}

TEST_F(M6502StoreRegisterTests, INS_STX_ABS) {
    using namespace m6502;
    TestStoreRegisterAbsolute(CPU::INS_STX_ABS, &CPU::X);
}

//STY
TEST_F(M6502StoreRegisterTests, INS_STY_ZP) {
    using namespace m6502;
    TestStoreRegisterZeroPage(CPU::INS_STY_ZP, &CPU::Y);
}

TEST_F(M6502StoreRegisterTests, INS_STY_ZPX) {
    using namespace m6502;
    TestStoreRegisterZeroPageXorY(CPU::INS_STY_ZPX, &CPU::Y, &CPU::X);
}

TEST_F(M6502StoreRegisterTests, INS_STY_ABS) {
    using namespace m6502;
    TestStoreRegisterAbsolute(CPU::INS_STY_ABS, &CPU::Y);
}