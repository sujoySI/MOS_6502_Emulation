#include "M6502Lib/cpu.h"
#include <gtest/gtest.h>

class M6502LogicalAndEorOraBitTests : public ::testing::Test {
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

        enum class ELogicalOp {
            And, Eor, Or
        };

        static m6502::UI8 DoLogicalOp(const m6502::UI8 Op1, const m6502::UI8 Op2, const ELogicalOp LogicalOp) {
            switch (LogicalOp) {
                case ELogicalOp::And: {
                    return Op1 & Op2 ;
                }break;
                case ELogicalOp::Or: {
                    return Op1 | Op2 ;
                }break;
                case ELogicalOp::Eor: {
                    return Op1 ^ Op2 ;
                }break;
            }
            throw -1;
        }

        void TestLogicalOpOnARegisterImmediate(ELogicalOp LogicalOp);
        void TestLogicalOpOnARegisterImmediateAffectsZeroFlag(ELogicalOp LogicalOp);
        void TestLogicalOpOnARegisterZeroPage(ELogicalOp LogicalOp);
        void TestLogicalOpOnARegisterZeroPageX(ELogicalOp LogicalOp);
        void TestLogicalOpOnARegisterZeroPageXWhenItWraps(ELogicalOp LogicalOp);
        void TestLogicalOpOnARegisterAbsolute(ELogicalOp LogicalOp);
        void TestLogicalOpOnARegisterAbsoluteX(ELogicalOp LogicalOp);
        void TestLogicalOpOnARegisterAbsoluteY(ELogicalOp LogicalOp);
        void TestLogicalOpOnARegisterAbsoluteXWhenCrossingPageBoundary(ELogicalOp LogicalOp);
        void TestLogicalOpOnARegisterAbsoluteYWhenCrossingPageBoundary(ELogicalOp LogicalOp);
        void TestLogicalOpOnARegisterIndrirectX(ELogicalOp LogicalOp);
        void TestLogicalOpOnARegisterIndrirectY(ELogicalOp LogicalOp);
        void TestLogicalOpOnARegisterIndrirectYWhenItCrossesAPageBoundary(ELogicalOp LogicalOp);
};

static void VerifyUnmodifiedFlagsFromLogicalOpOnARegister(const m6502::CPU& cpu, const m6502::CPU& cpuCopy) {
    EXPECT_EQ(cpu.PSF.B, cpuCopy.PSF.B);
    EXPECT_EQ(cpu.PSF.C, cpuCopy.PSF.C);
    EXPECT_EQ(cpu.PSF.D, cpuCopy.PSF.D);
    EXPECT_EQ(cpu.PSF.I, cpuCopy.PSF.I);
    EXPECT_EQ(cpu.PSF.V, cpuCopy.PSF.V);
}

void M6502LogicalAndEorOraBitTests::TestLogicalOpOnARegisterImmediate(const ELogicalOp LogicalOp){
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.A = 0xCC;
    switch (LogicalOp) {
        case ELogicalOp::And: {
            mem.Data[0xFFFC] = CPU::INS_AND_IM;
        }break;
        case ELogicalOp::Or: {
            mem.Data[0xFFFC] = CPU::INS_ORA_IM;
        }break;
        case ELogicalOp::Eor: {
            mem.Data[0xFFFC] = CPU::INS_EOR_IM;
        }break;
    }
    mem.Data[0xFFFD] = 0x84;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    const UI8 ExpectedResult = DoLogicalOp(0xCC, 0x84, LogicalOp);
    const bool ExpectedNegative = (ExpectedResult & 0b10000000 ) > 0;
    EXPECT_EQ(cpu.A, ExpectedResult);
    EXPECT_FALSE(cpu.PSF.Z);
    EXPECT_EQ(cpu.PSF.N, ExpectedNegative);
    VerifyUnmodifiedFlagsFromLogicalOpOnARegister(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}
void M6502LogicalAndEorOraBitTests::TestLogicalOpOnARegisterImmediateAffectsZeroFlag(const ELogicalOp LogicalOp)
{
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.A = 0xCC;
    switch (LogicalOp) {
        case ELogicalOp::And: {
            mem.Data[0xFFFC] = CPU::INS_AND_IM;
        }break;
        case ELogicalOp::Or: {
            mem.Data[0xFFFC] = CPU::INS_ORA_IM;
        }break;
        case ELogicalOp::Eor: {
            mem.Data[0xFFFC] = CPU::INS_EOR_IM;
        }break;
    }
    mem.Data[0xFFFD] = 0x0;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    const UI8 ExpectedResult = DoLogicalOp(0xCC, 0x0, LogicalOp);
    const bool ExpectedNegative = (ExpectedResult & 0b10000000 ) > 0;
    const bool ExpectedZero = (cpu.A == 0);
    EXPECT_EQ(cpu.PSF.Z, ExpectedZero);
    EXPECT_EQ(cpu.PSF.N, ExpectedNegative);
    VerifyUnmodifiedFlagsFromLogicalOpOnARegister(cpu, cpuCopy);
}
void M6502LogicalAndEorOraBitTests::TestLogicalOpOnARegisterZeroPage(const ELogicalOp LogicalOp){
    //Given:
    using namespace m6502;
    ExpectedCycles = 3;
    cpu.A = 0xCC;
    switch (LogicalOp) {
        case ELogicalOp::And: {
            mem.Data[0xFFFC] = CPU::INS_AND_ZP;
        }break;
        case ELogicalOp::Or: {
            mem.Data[0xFFFC] = CPU::INS_ORA_ZP;
        }break;
        case ELogicalOp::Eor: {
            mem.Data[0xFFFC] = CPU::INS_EOR_ZP;
        }break;
    }
    mem.Data[0xFFFD] = 0x42;
    mem.Data[0x0042] = 0x37;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    const UI8 ExpectedResult = DoLogicalOp(0xCC, 0x37, LogicalOp);
    const bool ExpectedNegative = (ExpectedResult & 0b10000000 ) > 0;
    EXPECT_EQ(cpu.A, ExpectedResult);
    EXPECT_FALSE(cpu.PSF.Z);
    EXPECT_EQ(cpu.PSF.N, ExpectedNegative);
    VerifyUnmodifiedFlagsFromLogicalOpOnARegister(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}
void M6502LogicalAndEorOraBitTests::TestLogicalOpOnARegisterZeroPageX(const ELogicalOp LogicalOp){
    //Given:
    using namespace m6502;
    ExpectedCycles = 4;
    cpu.X = 5;
    cpu.A = 0xCC;
    switch (LogicalOp) {
        case ELogicalOp::And: {
            mem.Data[0xFFFC] = CPU::INS_AND_ZPX;
        }break;
        case ELogicalOp::Or: {
            mem.Data[0xFFFC] = CPU::INS_ORA_ZPX;
        }break;
        case ELogicalOp::Eor: {
            mem.Data[0xFFFC] = CPU::INS_EOR_ZPX;
        }break;
    }
    mem.Data[0xFFFD] = 0x42;
    mem.Data[0x0047] = 0x37;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    const UI8 ExpectedResult = DoLogicalOp(0xCC, 0x37, LogicalOp);
    const bool ExpectedNegative = (ExpectedResult & 0b10000000 ) > 0;
    EXPECT_EQ(cpu.A, ExpectedResult);
    EXPECT_FALSE(cpu.PSF.Z);
    EXPECT_EQ(cpu.PSF.N, ExpectedNegative);
    VerifyUnmodifiedFlagsFromLogicalOpOnARegister(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}
void M6502LogicalAndEorOraBitTests::TestLogicalOpOnARegisterZeroPageXWhenItWraps(const ELogicalOp LogicalOp){
    using namespace m6502;
    ExpectedCycles = 4;
    cpu.A = 0xCC;
    cpu.X = 0xFF;
    switch (LogicalOp) {
        case ELogicalOp::And: {
            mem.Data[0xFFFC] = CPU::INS_AND_ZPX;
        }break;
        case ELogicalOp::Or: {
            mem.Data[0xFFFC] = CPU::INS_ORA_ZPX;
        }break;
        case ELogicalOp::Eor: {
            mem.Data[0xFFFC] = CPU::INS_EOR_ZPX;
        }break;
    }
    mem.Data[0xFFFD] = 0x80;
    mem.Data[0x007F] = 0x37;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    const UI8 ExpectedResult = DoLogicalOp(0xCC, 0x37, LogicalOp);
    const bool ExpectedNegative = (ExpectedResult & 0b10000000 ) > 0;
    EXPECT_EQ(cpu.A, ExpectedResult);
    EXPECT_FALSE(cpu.PSF.Z);
    EXPECT_EQ(cpu.PSF.N, ExpectedNegative);
    VerifyUnmodifiedFlagsFromLogicalOpOnARegister(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}
void M6502LogicalAndEorOraBitTests::TestLogicalOpOnARegisterAbsolute(const ELogicalOp LogicalOp)
{
    //Given:
    using namespace m6502;
    cpu.PSF.Z = cpu.PSF.N = true;
    ExpectedCycles = 4;
    cpu.X = 5;
    cpu.A = 0xCC;
    switch (LogicalOp) {
        case ELogicalOp::And: {
            mem.Data[0xFFFC] = CPU::INS_AND_ABS;
        }break;
        case ELogicalOp::Or: {
            mem.Data[0xFFFC] = CPU::INS_ORA_ABS;
        }break;
        case ELogicalOp::Eor: {
            mem.Data[0xFFFC] = CPU::INS_EOR_ABS;
        }break;
    }
    mem.Data[0xFFFD] = 0x80;
    mem.Data[0xFFFE] = 0x44;
    mem.Data[0x4480] = 0x37;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    const UI8 ExpectedResult = DoLogicalOp(0xCC, 0x37, LogicalOp);
    const bool ExpectedNegative = (ExpectedResult & 0b10000000 ) > 0;
    EXPECT_EQ(cpu.A, ExpectedResult);
    EXPECT_FALSE(cpu.PSF.Z);
    EXPECT_EQ(cpu.PSF.N, ExpectedNegative);
    VerifyUnmodifiedFlagsFromLogicalOpOnARegister(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}
void M6502LogicalAndEorOraBitTests::TestLogicalOpOnARegisterAbsoluteX(const ELogicalOp LogicalOp)
{
    //Given:
    using namespace m6502;
    cpu.PSF.Z = cpu.PSF.N = true;
    ExpectedCycles = 4;
    cpu.X = 1;
    cpu.A = 0xCC;
    switch (LogicalOp) {
        case ELogicalOp::And: {
            mem.Data[0xFFFC] = CPU::INS_AND_ABSX;
        }break;
        case ELogicalOp::Or: {
            mem.Data[0xFFFC] = CPU::INS_ORA_ABSX;
        }break;
        case ELogicalOp::Eor: {
            mem.Data[0xFFFC] = CPU::INS_EOR_ABSX;
        }break;
    }
    mem.Data[0xFFFD] = 0x80;
    mem.Data[0xFFFE] = 0x44;    //0x4402
    mem.Data[0x4481] = 0x37;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    const UI8 ExpectedResult = DoLogicalOp(0xCC, 0x37, LogicalOp);
    const bool ExpectedNegative = (ExpectedResult & 0b10000000 ) > 0;
    EXPECT_EQ(cpu.A, ExpectedResult);
    EXPECT_FALSE(cpu.PSF.Z);
    EXPECT_EQ(cpu.PSF.N, ExpectedNegative);
    VerifyUnmodifiedFlagsFromLogicalOpOnARegister(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}
void M6502LogicalAndEorOraBitTests::TestLogicalOpOnARegisterAbsoluteY(const ELogicalOp LogicalOp)
{
    //Given:
    using namespace m6502;
    cpu.PSF.Z = cpu.PSF.N = true;
    ExpectedCycles = 4;
    cpu.Y = 1;
    cpu.A = 0xCC;
    switch (LogicalOp) {
        case ELogicalOp::And: {
            mem.Data[0xFFFC] = CPU::INS_AND_ABSY;
        }break;
        case ELogicalOp::Or: {
            mem.Data[0xFFFC] = CPU::INS_ORA_ABSY;
        }break;
        case ELogicalOp::Eor: {
            mem.Data[0xFFFC] = CPU::INS_EOR_ABSY;
        }break;
    }
    mem.Data[0xFFFD] = 0x80;
    mem.Data[0xFFFE] = 0x44;    //0x4402
    mem.Data[0x4481] = 0x37;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    const UI8 ExpectedResult = DoLogicalOp(0xCC, 0x37, LogicalOp);
    const bool ExpectedNegative = (ExpectedResult & 0b10000000 ) > 0;
    EXPECT_EQ(cpu.A, ExpectedResult);
    EXPECT_FALSE(cpu.PSF.Z);
    EXPECT_EQ(cpu.PSF.N, ExpectedNegative);
    VerifyUnmodifiedFlagsFromLogicalOpOnARegister(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}
void M6502LogicalAndEorOraBitTests::TestLogicalOpOnARegisterAbsoluteXWhenCrossingPageBoundary(const ELogicalOp LogicalOp) {
    //Given:
    using namespace m6502;
    cpu.PSF.Z = cpu.PSF.N = true;
    ExpectedCycles = 5;
    cpu.A = 0xCC;
    cpu.X = 0xFF;
    switch (LogicalOp) {
        case ELogicalOp::And: {
            mem.Data[0xFFFC] = CPU::INS_AND_ABSX;
        }break;
        case ELogicalOp::Or: {
            mem.Data[0xFFFC] = CPU::INS_ORA_ABSX;
        }break;
        case ELogicalOp::Eor: {
            mem.Data[0xFFFC] = CPU::INS_EOR_ABSX;
        }break;
    }
    mem.Data[0xFFFD] = 0x02;
    mem.Data[0xFFFE] = 0x44;    //0x4402
    mem.Data[0x4501] = 0x37;    //0x4402+0xFF when it crosses page boundary

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    const UI8 ExpectedResult = DoLogicalOp(0xCC, 0x37, LogicalOp);
    const bool ExpectedNegative = (ExpectedResult & 0b10000000 ) > 0;
    EXPECT_EQ(cpu.A, ExpectedResult);
    EXPECT_FALSE(cpu.PSF.Z);
    EXPECT_EQ(cpu.PSF.N, ExpectedNegative);
    VerifyUnmodifiedFlagsFromLogicalOpOnARegister(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}
void M6502LogicalAndEorOraBitTests::TestLogicalOpOnARegisterAbsoluteYWhenCrossingPageBoundary(const ELogicalOp LogicalOp) {
    //Given:
    using namespace m6502;
    cpu.PSF.Z = cpu.PSF.N = true;
    ExpectedCycles = 5;
    cpu.A = 0xCC;
    cpu.Y = 0xFF;
    switch (LogicalOp) {
        case ELogicalOp::And: {
            mem.Data[0xFFFC] = CPU::INS_AND_ABSY;
        }break;
        case ELogicalOp::Or: {
            mem.Data[0xFFFC] = CPU::INS_ORA_ABSY;
        }break;
        case ELogicalOp::Eor: {
            mem.Data[0xFFFC] = CPU::INS_EOR_ABSY;
        }break;
    }
    mem.Data[0xFFFD] = 0x02;
    mem.Data[0xFFFE] = 0x44;    //0x4402
    mem.Data[0x4501] = 0x37;    //0x4402+0xFF when it crosses page boundary

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    const UI8 ExpectedResult = DoLogicalOp(0xCC, 0x37, LogicalOp);
    const bool ExpectedNegative = (ExpectedResult & 0b10000000 ) > 0;
    EXPECT_EQ(cpu.A, ExpectedResult);
    EXPECT_FALSE(cpu.PSF.Z);
    EXPECT_EQ(cpu.PSF.N, ExpectedNegative);
    VerifyUnmodifiedFlagsFromLogicalOpOnARegister(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}
void M6502LogicalAndEorOraBitTests::TestLogicalOpOnARegisterIndrirectX(const ELogicalOp LogicalOp) {
    //Given:
    using namespace m6502;
    cpu.PSF.Z = cpu.PSF.N = true;
    ExpectedCycles = 6;
    cpu.X = 0x04;
    cpu.A = 0xCC;
    switch (LogicalOp) {
        case ELogicalOp::And: {
            mem.Data[0xFFFC] = CPU::INS_AND_INDX;
        }break;
        case ELogicalOp::Or: {
            mem.Data[0xFFFC] = CPU::INS_ORA_INDX;
        }break;
        case ELogicalOp::Eor: {
            mem.Data[0xFFFC] = CPU::INS_EOR_INDX;
        }break;
    }
    mem.Data[0xFFFD] = 0x02;
    mem.Data[0x0006] = 0x00;    //0x0002+0x0004
    mem.Data[0x0007] = 0x80;    //new address 0x8000
    mem.Data[0x8000] = 0x37;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    const UI8 ExpectedResult = DoLogicalOp(0xCC, 0x37, LogicalOp);
    const bool ExpectedNegative = (ExpectedResult & 0b10000000 ) > 0;
    EXPECT_EQ(cpu.A, ExpectedResult);
    EXPECT_FALSE(cpu.PSF.Z);
    EXPECT_EQ(cpu.PSF.N, ExpectedNegative);
    VerifyUnmodifiedFlagsFromLogicalOpOnARegister(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}
void M6502LogicalAndEorOraBitTests::TestLogicalOpOnARegisterIndrirectY(const ELogicalOp LogicalOp) {
    //Given:
    using namespace m6502;
    cpu.PSF.Z = cpu.PSF.N = true;
    ExpectedCycles = 5;
    cpu.Y = 0x04;
    cpu.A = 0xCC;
    switch (LogicalOp) {
        case ELogicalOp::And: {
            mem.Data[0xFFFC] = CPU::INS_AND_INDY;
        }break;
        case ELogicalOp::Or: {
            mem.Data[0xFFFC] = CPU::INS_ORA_INDY;
        }break;
        case ELogicalOp::Eor: {
            mem.Data[0xFFFC] = CPU::INS_EOR_INDY;
        }break;
    }
    mem.Data[0xFFFD] = 0x02;
    mem.Data[0x0002] = 0x00;
    mem.Data[0x0003] = 0x80;
    mem.Data[0x8004] = 0x37;    //0x8000+0x04

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    const UI8 ExpectedResult = DoLogicalOp(0xCC, 0x37, LogicalOp);
    const bool ExpectedNegative = (ExpectedResult & 0b10000000 ) > 0;
    EXPECT_EQ(cpu.A, ExpectedResult);
    EXPECT_FALSE(cpu.PSF.Z);
    EXPECT_EQ(cpu.PSF.N, ExpectedNegative);
    VerifyUnmodifiedFlagsFromLogicalOpOnARegister(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}
void M6502LogicalAndEorOraBitTests::TestLogicalOpOnARegisterIndrirectYWhenItCrossesAPageBoundary(const ELogicalOp LogicalOp) {
    //Given:
    using namespace m6502;
    cpu.PSF.Z = cpu.PSF.N = true;
    ExpectedCycles = 6;
    cpu.A = 0xCC;
    cpu.Y = 0xFF;
    switch (LogicalOp) {
        case ELogicalOp::And: {
            mem.Data[0xFFFC] = CPU::INS_AND_INDY;
        }break;
        case ELogicalOp::Or: {
            mem.Data[0xFFFC] = CPU::INS_ORA_INDY;
        }break;
        case ELogicalOp::Eor: {
            mem.Data[0xFFFC] = CPU::INS_EOR_INDY;
        }break;
    }
    mem.Data[0xFFFD] = 0x02;
    mem.Data[0x0002] = 0x02;
    mem.Data[0x0003] = 0x80;
    mem.Data[0x8101] = 0x37;    //0x8002+0xFF when it crosses page boundary

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    const UI8 ExpectedResult = DoLogicalOp(0xCC, 0x37, LogicalOp);
    const bool ExpectedNegative = (ExpectedResult & 0b10000000 ) > 0;
    EXPECT_EQ(cpu.A, ExpectedResult);
    EXPECT_FALSE(cpu.PSF.Z);
    EXPECT_EQ(cpu.PSF.N, ExpectedNegative);
    VerifyUnmodifiedFlagsFromLogicalOpOnARegister(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}



//AND
TEST_F(M6502LogicalAndEorOraBitTests, INS_AND_IM) {
    using namespace m6502;
    TestLogicalOpOnARegisterImmediate(ELogicalOp::And);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_AND_IM_AffectsZeroFlag) {
    using namespace m6502;
    TestLogicalOpOnARegisterImmediateAffectsZeroFlag(ELogicalOp::And);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_AND_ZP) {
    using namespace m6502;
    TestLogicalOpOnARegisterZeroPage(ELogicalOp::And);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_AND_ZPX) {
    using namespace m6502;
    TestLogicalOpOnARegisterZeroPageX(ELogicalOp::And);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_AND_ZPX_WhenitWraps) {
    using namespace m6502;
    TestLogicalOpOnARegisterZeroPageXWhenItWraps(ELogicalOp::And);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_AND_ABS) {
    using namespace m6502;
    TestLogicalOpOnARegisterAbsolute(ELogicalOp::And);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_AND_ABSX) {
    using namespace m6502;
    TestLogicalOpOnARegisterAbsoluteX(ELogicalOp::And);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_AND_ABSX_WhenItCrossesAPageBoundary) {
    using namespace m6502;
    TestLogicalOpOnARegisterAbsoluteXWhenCrossingPageBoundary(ELogicalOp::And);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_AND_ABSY) {
    using namespace m6502;
    TestLogicalOpOnARegisterAbsoluteY(ELogicalOp::And);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_AND_ABSY_WhenItCrossesAPageBoundary) {
    using namespace m6502;
    TestLogicalOpOnARegisterAbsoluteYWhenCrossingPageBoundary(ELogicalOp::And);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_AND_INDX) {
    using namespace m6502;
    TestLogicalOpOnARegisterIndrirectX(ELogicalOp::And);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_AND_INDY) {
    using namespace m6502;
    TestLogicalOpOnARegisterIndrirectY(ELogicalOp::And);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_AND_INDY_WhenItCrossesAPageBoundary) {
    using namespace m6502;
    TestLogicalOpOnARegisterIndrirectYWhenItCrossesAPageBoundary(ELogicalOp::And);
}

//EOR
TEST_F(M6502LogicalAndEorOraBitTests, INS_EOR_IM) {
    using namespace m6502;
    TestLogicalOpOnARegisterImmediate(ELogicalOp::Eor);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_EOR_IM_AffectsZeroFlag) {
    using namespace m6502;
    TestLogicalOpOnARegisterImmediateAffectsZeroFlag(ELogicalOp::Eor);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_EOR_ZP) {
    using namespace m6502;
    TestLogicalOpOnARegisterZeroPage(ELogicalOp::Eor);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_EOR_ZPX) {
    using namespace m6502;
    TestLogicalOpOnARegisterZeroPageX(ELogicalOp::Eor);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_EOR_ZPX_WhenitWraps) {
    using namespace m6502;
    TestLogicalOpOnARegisterZeroPageXWhenItWraps(ELogicalOp::Eor);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_EOR_ABS) {
    using namespace m6502;
    TestLogicalOpOnARegisterAbsolute(ELogicalOp::Eor);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_EOR_ABSX) {
    using namespace m6502;
    TestLogicalOpOnARegisterAbsoluteX(ELogicalOp::Eor);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_EOR_ABSX_WhenItCrossesAPageBoundary) {
    using namespace m6502;
    TestLogicalOpOnARegisterAbsoluteXWhenCrossingPageBoundary(ELogicalOp::Eor);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_EOR_ABSY) {
    using namespace m6502;
    TestLogicalOpOnARegisterAbsoluteY(ELogicalOp::Eor);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_EOR_ABSY_WhenItCrossesAPageBoundary) {
    using namespace m6502;
    TestLogicalOpOnARegisterAbsoluteYWhenCrossingPageBoundary(ELogicalOp::Eor);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_EOR_INDX) {
    using namespace m6502;
    TestLogicalOpOnARegisterIndrirectX(ELogicalOp::Eor);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_EOR_INDY) {
    using namespace m6502;
    TestLogicalOpOnARegisterIndrirectY(ELogicalOp::Eor);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_EOR_INDY_WhenItCrossesAPageBoundary) {
    using namespace m6502;
    TestLogicalOpOnARegisterIndrirectYWhenItCrossesAPageBoundary(ELogicalOp::Eor);
}

//ORA
TEST_F(M6502LogicalAndEorOraBitTests, INS_ORA_IM) {
    using namespace m6502;
    TestLogicalOpOnARegisterImmediate(ELogicalOp::Or);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_ORA_IM_AffectsZeroFlag) {
    using namespace m6502;
    TestLogicalOpOnARegisterImmediateAffectsZeroFlag(ELogicalOp::Or);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_ORA_ZP) {
    using namespace m6502;
    TestLogicalOpOnARegisterZeroPage(ELogicalOp::Or);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_ORA_ZPX) {
    using namespace m6502;
    TestLogicalOpOnARegisterZeroPageX(ELogicalOp::Or);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_AND_ORA_WhenitWraps) {
    using namespace m6502;
    TestLogicalOpOnARegisterZeroPageXWhenItWraps(ELogicalOp::Or);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_ORA_ABS) {
    using namespace m6502;
    TestLogicalOpOnARegisterAbsolute(ELogicalOp::Or);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_ORA_ABSX) {
    using namespace m6502;
    TestLogicalOpOnARegisterAbsoluteX(ELogicalOp::Or);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_ORA_ABSX_WhenItCrossesAPageBoundary) {
    using namespace m6502;
    TestLogicalOpOnARegisterAbsoluteXWhenCrossingPageBoundary(ELogicalOp::Or);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_ORA_ABSY) {
    using namespace m6502;
    TestLogicalOpOnARegisterAbsoluteY(ELogicalOp::Or);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_ORA_ABSY_WhenItCrossesAPageBoundary) {
    using namespace m6502;
    TestLogicalOpOnARegisterAbsoluteYWhenCrossingPageBoundary(ELogicalOp::Or);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_ORA_INDX) {
    using namespace m6502;
    TestLogicalOpOnARegisterIndrirectX(ELogicalOp::Or);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_ORA_INDY) {
    using namespace m6502;
    TestLogicalOpOnARegisterIndrirectY(ELogicalOp::Or);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_ORA_INDY_WhenItCrossesAPageBoundary) {
    using namespace m6502;
    TestLogicalOpOnARegisterIndrirectYWhenItCrossesAPageBoundary(ELogicalOp::Or);
}

//BIT
TEST_F(M6502LogicalAndEorOraBitTests, INS_BIT_ZP) {
    //Given:
    using namespace m6502;
    ExpectedCycles = 3;
    cpu.A = 0xCC;
    mem.Data[0xFFFC] = CPU::INS_BIT_ZP;
    mem.Data[0xFFFD] = 0x42;
    mem.Data[0x0042] = 0xCC;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    // EXPECT_EQ(cpu.A, 0x37);
    EXPECT_EQ(cpu.A, 0xCC);
    EXPECT_FALSE(cpu.PSF.Z);
    EXPECT_TRUE(cpu.PSF.N);
    EXPECT_TRUE(cpu.PSF.V);
    // VerifyUnmodifiedFlagsFromLogicalOpOnARegister(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_BIT_ZP_ResultZero) {
    //Given:
    using namespace m6502;
    ExpectedCycles = 3;
    cpu.A = 0xCC;
    mem.Data[0xFFFC] = CPU::INS_BIT_ZP;
    mem.Data[0xFFFD] = 0x42;
    mem.Data[0x0042] = 0x33;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    // EXPECT_EQ(cpu.A, 0x37);
    EXPECT_EQ(cpu.A, 0xCC);
    EXPECT_TRUE(cpu.PSF.Z);
    EXPECT_FALSE(cpu.PSF.N);
    EXPECT_FALSE(cpu.PSF.V);
    VerifyUnmodifiedFlagsFromLogicalOpOnARegister(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_BIT_ZP_ResultZero_Bit6and7areZero) {
    //Given:
    using namespace m6502;
    ExpectedCycles = 3;
    cpu.A = 0x33;
    mem.Data[0xFFFC] = CPU::INS_BIT_ZP;
    mem.Data[0xFFFD] = 0x42;
    mem.Data[0x0042] = 0xCC;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    // EXPECT_EQ(cpu.A, 0x37);
    EXPECT_EQ(cpu.A, 0x33);
    EXPECT_TRUE(cpu.PSF.Z);
    EXPECT_TRUE(cpu.PSF.N);
    EXPECT_TRUE(cpu.PSF.V);
    // VerifyUnmodifiedFlagsFromLogicalOpOnARegister(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_BIT_ZP_ResultZero_Bit6and7areMixed) {
    //Given:
    using namespace m6502;
    ExpectedCycles = 3;
    // cpu.A = 0x33;
    mem.Data[0xFFFC] = CPU::INS_BIT_ZP;
    mem.Data[0xFFFD] = 0x42;
    mem.Data[0x0042] = 0b10000000;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    // EXPECT_EQ(cpu.A, 0x37);
    // EXPECT_EQ(cpu.A, 0x33);
    // EXPECT_TRUE(cpu.PSF.Z);
    EXPECT_TRUE(cpu.PSF.N);
    EXPECT_FALSE(cpu.PSF.V);
    // VerifyUnmodifiedFlagsFromLogicalOpOnARegister(cpu, cpuCopy);
    // EXPECT_EQ(cycles, ExpectedCycles);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_BIT_ABS) {
    //Given:
    using namespace m6502;
    ExpectedCycles = 4;
    cpu.A = 0xCC;
    mem.Data[0xFFFC] = CPU::INS_BIT_ABS;
    mem.Data[0xFFFD] = 0x00;
    mem.Data[0xFFFE] = 0x80;
    mem.Data[0x8000] = 0xCC;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    // EXPECT_EQ(cpu.A, 0x37);
    EXPECT_EQ(cpu.A, 0xCC);
    EXPECT_FALSE(cpu.PSF.Z);
    EXPECT_TRUE(cpu.PSF.N);
    EXPECT_TRUE(cpu.PSF.V);
    // VerifyUnmodifiedFlagsFromLogicalOpOnARegister(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_BIT_ABS_ResultZero) {
    //Given:
    using namespace m6502;
    ExpectedCycles = 4;
    cpu.A = 0xCC;
    mem.Data[0xFFFC] = CPU::INS_BIT_ABS;
    mem.Data[0xFFFD] = 0x00;
    mem.Data[0xFFFE] = 0x80;
    mem.Data[0x8000] = 0x33;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    // EXPECT_EQ(cpu.A, 0x37);
    EXPECT_EQ(cpu.A, 0xCC);
    EXPECT_TRUE(cpu.PSF.Z);
    EXPECT_FALSE(cpu.PSF.N);
    EXPECT_FALSE(cpu.PSF.V);
    // VerifyUnmodifiedFlagsFromLogicalOpOnARegister(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_BIT_ABS_ResultZero_Bit6and7areZero) {
    //Given:
    using namespace m6502;
    ExpectedCycles = 4;
    cpu.A = 0x33;
    mem.Data[0xFFFC] = CPU::INS_BIT_ABS;
    mem.Data[0xFFFD] = 0x00;
    mem.Data[0xFFFE] = 0x80;
    mem.Data[0x8000] = 0xCC;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    // EXPECT_EQ(cpu.A, 0x37);
    EXPECT_EQ(cpu.A, 0x33);
    EXPECT_TRUE(cpu.PSF.Z);
    EXPECT_TRUE(cpu.PSF.N);
    EXPECT_TRUE(cpu.PSF.V);
    // VerifyUnmodifiedFlagsFromLogicalOpOnARegister(cpu, cpuCopy);
    EXPECT_EQ(cycles, ExpectedCycles);
}
TEST_F(M6502LogicalAndEorOraBitTests, INS_BIT_ABS_ResultZero_Bit6and7areMixed) {
    //Given:
    using namespace m6502;
    ExpectedCycles = 4;
    // cpu.A = 0x33;
    mem.Data[0xFFFC] = CPU::INS_BIT_ABS;
    mem.Data[0xFFFD] = 0x00;
    mem.Data[0xFFFE] = 0x80;
    mem.Data[0x8000] = 0b10000000;

    //When:
    cpuCopy= cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    // EXPECT_EQ(cpu.A, 0x37);
    // EXPECT_EQ(cpu.A, 0x33);
    // EXPECT_TRUE(cpu.PSF.Z);
    EXPECT_TRUE(cpu.PSF.N);
    EXPECT_FALSE(cpu.PSF.V);
    // VerifyUnmodifiedFlagsFromLogicalOpOnARegister(cpu, cpuCopy);
    // EXPECT_EQ(cycles, ExpectedCycles);
}