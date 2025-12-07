#include "M6502Lib/cpu.h"
#include <gtest/gtest.h>

#define BYTE(A) ( (u_int8_t)(A) )

class M6502AddWithCarryTest: public ::testing::Test {
public:

    m6502::MEMORY mem{};
    m6502::CPU cpu{};
    m6502::CPU cpuCopy{};
    m6502::SI32 cycles{};
    m6502::SI32 ExpectedCycles{};

    struct ADCTestData {
        bool CarryBefore;

        m6502::UI8 A;
        m6502::UI8 Operand;
        m6502::UI8 Answer;

        bool ExpectC;
        bool ExpectZ;
        bool ExpectN;
        bool ExpectV;
    };

    virtual void SetUp()
    {
        cpu.Reset(0xFFFC, mem);
    }

    virtual void TearDown()
    {
    }

    void TestADC_IM(ADCTestData test);
    void TestADC_ZP(ADCTestData test);
    void TestADC_ZPX(ADCTestData test);
    void TestADC_ABS(ADCTestData test);
    void TestADC_ABSX(ADCTestData test);
    void TestADC_ABSY(ADCTestData test);
    void TestADC_INDX(ADCTestData test);
    void TestADC_INDY(ADCTestData test);
};

static void VerifyUnmodifiedFlagsFromArithmeticOperation(const m6502::CPU& cpu, const m6502::CPU& cpuCopy) {
    EXPECT_EQ(cpu.PS.B, cpuCopy.PS.B);
    EXPECT_EQ(cpu.PS.D, cpuCopy.PS.D);
    EXPECT_EQ(cpu.PS.I, cpuCopy.PS.I);
}

void M6502AddWithCarryTest::TestADC_IM(const ADCTestData test) {
    //Given:
    using namespace m6502;
    ExpectedCycles = 2;
    cpu.Reset(0xFF00, mem);
    cpu.A = test.A;
    cpu.PS.C = test.CarryBefore;
    cpu.PS.Z = !test.ExpectZ;
    cpu.PS.N = !test.ExpectN;
    cpu.PS.V = !test.ExpectV;
    mem.Data[0xFF00] = CPU::INS_ADC_IM;
    mem.Data[0xFF01] = test.Operand;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    // EXPECT_EQ(cpu.PS.All, cpuCopy.PS.All);
    EXPECT_EQ(cpu.A, test.Answer);
    EXPECT_EQ(cpu.PS.C, test.ExpectC);
    EXPECT_EQ(cpu.PS.Z, test.ExpectZ);
    EXPECT_EQ(cpu.PS.N, test.ExpectN);
    EXPECT_EQ(cpu.PS.V, test.ExpectV);
    VerifyUnmodifiedFlagsFromArithmeticOperation(cpu, cpuCopy);
}
void M6502AddWithCarryTest::TestADC_ZP(const ADCTestData test) {
    //Given:
    using namespace m6502;
    ExpectedCycles = 3;
    cpu.Reset(0xFF00, mem);
    cpu.A = test.A;
    cpu.PS.C = test.CarryBefore;
    cpu.PS.Z = !test.ExpectZ;
    cpu.PS.N = !test.ExpectN;
    cpu.PS.V = !test.ExpectV;
    mem.Data[0xFF00] = CPU::INS_ADC_ZP;
    mem.Data[0xFF01] = 0x42;
    mem.Data[0x0042] = test.Operand;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    // EXPECT_EQ(cpu.PS.All, cpuCopy.PS.All);
    EXPECT_EQ(cpu.A, test.Answer);
    EXPECT_EQ(cpu.PS.C, test.ExpectC);
    EXPECT_EQ(cpu.PS.Z, test.ExpectZ);
    EXPECT_EQ(cpu.PS.N, test.ExpectN);
    EXPECT_EQ(cpu.PS.V, test.ExpectV);
    VerifyUnmodifiedFlagsFromArithmeticOperation(cpu, cpuCopy);
}
void M6502AddWithCarryTest::TestADC_ZPX(const ADCTestData test) {
    //Given:
    using namespace m6502;
    ExpectedCycles = 4;
    cpu.Reset(0xFF00, mem);
    cpu.X = 0x10;
    cpu.A = test.A;
    cpu.PS.C = test.CarryBefore;
    cpu.PS.Z = !test.ExpectZ;
    cpu.PS.N = !test.ExpectN;
    cpu.PS.V = !test.ExpectV;
    mem.Data[0xFF00] = CPU::INS_ADC_ZPX;
    mem.Data[0xFF01] = 0x42;
    mem.Data[0x0052] = test.Operand;//0x0042+0x10

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    // EXPECT_EQ(cpu.PS.All, cpuCopy.PS.All);
    EXPECT_EQ(cpu.A, test.Answer);
    EXPECT_EQ(cpu.PS.C, test.ExpectC);
    EXPECT_EQ(cpu.PS.Z, test.ExpectZ);
    EXPECT_EQ(cpu.PS.N, test.ExpectN);
    EXPECT_EQ(cpu.PS.V, test.ExpectV);
    VerifyUnmodifiedFlagsFromArithmeticOperation(cpu, cpuCopy);
}
void M6502AddWithCarryTest::TestADC_ABS(const ADCTestData test) {
    //Given:
    using namespace m6502;
    ExpectedCycles = 4;
    cpu.Reset(0xFF00, mem);
    cpu.A = test.A;
    cpu.PS.C = test.CarryBefore;
    cpu.PS.Z = !test.ExpectZ;
    cpu.PS.N = !test.ExpectN;
    cpu.PS.V = !test.ExpectV;
    mem.Data[0xFF00] = CPU::INS_ADC_ABS;
    mem.Data[0xFF01] = 0x00;
    mem.Data[0xFF02] = 0x80;
    mem.Data[0x8000] = test.Operand;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    // EXPECT_EQ(cpu.PS.All, cpuCopy.PS.All);
    EXPECT_EQ(cpu.A, test.Answer);
    EXPECT_EQ(cpu.PS.C, test.ExpectC);
    EXPECT_EQ(cpu.PS.Z, test.ExpectZ);
    EXPECT_EQ(cpu.PS.N, test.ExpectN);
    EXPECT_EQ(cpu.PS.V, test.ExpectV);
    VerifyUnmodifiedFlagsFromArithmeticOperation(cpu, cpuCopy);
}
void M6502AddWithCarryTest::TestADC_ABSX(const ADCTestData test) {
    //Given:
    using namespace m6502;
    ExpectedCycles = 4;
    cpu.Reset(0xFF00, mem);
    cpu.X = 0x10;
    cpu.A = test.A;
    cpu.PS.C = test.CarryBefore;
    cpu.PS.Z = !test.ExpectZ;
    cpu.PS.N = !test.ExpectN;
    cpu.PS.V = !test.ExpectV;
    mem.Data[0xFF00] = CPU::INS_ADC_ABSX;
    mem.Data[0xFF01] = 0x00;
    mem.Data[0xFF02] = 0x80;
    mem.Data[0x8010] = test.Operand;//0x8000+0x10

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    // EXPECT_EQ(cpu.PS.All, cpuCopy.PS.All);
    EXPECT_EQ(cpu.A, test.Answer);
    EXPECT_EQ(cpu.PS.C, test.ExpectC);
    EXPECT_EQ(cpu.PS.Z, test.ExpectZ);
    EXPECT_EQ(cpu.PS.N, test.ExpectN);
    EXPECT_EQ(cpu.PS.V, test.ExpectV);
    VerifyUnmodifiedFlagsFromArithmeticOperation(cpu, cpuCopy);
}
void M6502AddWithCarryTest::TestADC_ABSY(const ADCTestData test) {
    //Given:
    using namespace m6502;
    ExpectedCycles = 4;
    cpu.Reset(0xFF00, mem);
    cpu.Y = 0x10;
    cpu.A = test.A;
    cpu.PS.C = test.CarryBefore;
    cpu.PS.Z = !test.ExpectZ;
    cpu.PS.N = !test.ExpectN;
    cpu.PS.V = !test.ExpectV;
    mem.Data[0xFF00] = CPU::INS_ADC_ABSY;
    mem.Data[0xFF01] = 0x00;
    mem.Data[0xFF02] = 0x80;
    mem.Data[0x8010] = test.Operand;//0x8000+0x10

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    // EXPECT_EQ(cpu.PS.All, cpuCopy.PS.All);
    EXPECT_EQ(cpu.A, test.Answer);
    EXPECT_EQ(cpu.PS.C, test.ExpectC);
    EXPECT_EQ(cpu.PS.Z, test.ExpectZ);
    EXPECT_EQ(cpu.PS.N, test.ExpectN);
    EXPECT_EQ(cpu.PS.V, test.ExpectV);
    VerifyUnmodifiedFlagsFromArithmeticOperation(cpu, cpuCopy);
}
void M6502AddWithCarryTest::TestADC_INDX(const ADCTestData test) {
    //Given:
    using namespace m6502;
    ExpectedCycles = 6;
    cpu.Reset(0xFF00, mem);
    cpu.X = 0x04;
    cpu.A = test.A;
    cpu.PS.C = test.CarryBefore;
    cpu.PS.Z = !test.ExpectZ;
    cpu.PS.N = !test.ExpectN;
    cpu.PS.V = !test.ExpectV;
    mem.Data[0xFF00] = CPU::INS_ADC_INDX;
    mem.Data[0xFF01] = 0x02;
    mem.Data[0x0006] = 0x00;//0x04+0x02
    mem.Data[0x0007] = 0x80;
    mem.Data[0x8000] = test.Operand;

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    // EXPECT_EQ(cpu.PS.All, cpuCopy.PS.All);
    EXPECT_EQ(cpu.A, test.Answer);
    EXPECT_EQ(cpu.PS.C, test.ExpectC);
    EXPECT_EQ(cpu.PS.Z, test.ExpectZ);
    EXPECT_EQ(cpu.PS.N, test.ExpectN);
    EXPECT_EQ(cpu.PS.V, test.ExpectV);
    VerifyUnmodifiedFlagsFromArithmeticOperation(cpu, cpuCopy);
}
void M6502AddWithCarryTest::TestADC_INDY(const ADCTestData test) {
    //Given:
    using namespace m6502;
    ExpectedCycles = 5;
    cpu.Reset(0xFF00, mem);
    cpu.Y = 0x04;
    cpu.A = test.A;
    cpu.PS.C = test.CarryBefore;
    cpu.PS.Z = !test.ExpectZ;
    cpu.PS.N = !test.ExpectN;
    cpu.PS.V = !test.ExpectV;
    mem.Data[0xFF00] = CPU::INS_ADC_INDY;
    mem.Data[0xFF01] = 0x02;
    mem.Data[0x0002] = 0x00;
    mem.Data[0x0003] = 0x80;
    mem.Data[0x8004] = test.Operand;//0x8000+0x04

    //When:
    cpuCopy = cpu;
    cycles = cpu.Execute(ExpectedCycles, mem);

    //Then:
    EXPECT_EQ(cycles, ExpectedCycles);
    // EXPECT_EQ(cpu.PS.All, cpuCopy.PS.All);
    EXPECT_EQ(cpu.A, test.Answer);
    EXPECT_EQ(cpu.PS.C, test.ExpectC);
    EXPECT_EQ(cpu.PS.Z, test.ExpectZ);
    EXPECT_EQ(cpu.PS.N, test.ExpectN);
    EXPECT_EQ(cpu.PS.V, test.ExpectV);
    VerifyUnmodifiedFlagsFromArithmeticOperation(cpu, cpuCopy);
}

TEST_F(M6502AddWithCarryTest, INS_ADC_IM_AddTwoPositiveNumbersWithCarry) {
    ADCTestData Test = {};
    Test.CarryBefore = true;
    Test.A = 20;
    Test.Operand = 17;
    Test.Answer = 38;
    Test.ExpectC = false;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
    TestADC_IM(Test);
}
TEST_F(M6502AddWithCarryTest, INS_ADC_IM_AddPositiveAndNegativeNumbersWithCarry) {
    // A:       00010100 +20
    // Operand: 11111111 -17
    // Answer:  00000100 +4
    // C:1, N:0, V:0, Z:0

    ADCTestData Test = {};
    Test.CarryBefore = true;
    Test.A = 20;
    Test.Operand = -17;
    Test.Answer = 4;
    Test.ExpectC = true;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
    TestADC_IM(Test);
}
TEST_F(M6502AddWithCarryTest, INS_ADC_ZP_AddTwoPositiveNumbersWithCarry) {
    ADCTestData Test = {};
    Test.CarryBefore = true;
    Test.A = 20;
    Test.Operand = 17;
    Test.Answer = 38;
    Test.ExpectC = false;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
    TestADC_ZP(Test);
}
TEST_F(M6502AddWithCarryTest, INS_ADC_ZP_AddPositiveAndNegativeNumbersWithCarry) {
    // A:       00010100 +20
    // Operand: 11111111 -17
    // Answer:  00000100 +4
    // C:1, N:0, V:0, Z:0

    ADCTestData Test = {};
    Test.CarryBefore = true;
    Test.A = 20;
    Test.Operand = -17;
    Test.Answer = 4;
    Test.ExpectC = true;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
    TestADC_ZP(Test);
}
TEST_F(M6502AddWithCarryTest, INS_ADC_ZPX_AddTwoPositiveNumbersWithCarry) {
    ADCTestData Test = {};
    Test.CarryBefore = true;
    Test.A = 20;
    Test.Operand = 17;
    Test.Answer = 38;
    Test.ExpectC = false;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
    TestADC_ZPX(Test);
}
TEST_F(M6502AddWithCarryTest, INS_ADC_ZPX_AddPositiveAndNegativeNumbersWithCarry) {
    // A:       00010100 +20
    // Operand: 11111111 -17
    // Answer:  00000100 +4
    // C:1, N:0, V:0, Z:0

    ADCTestData Test = {};
    Test.CarryBefore = true;
    Test.A = 20;
    Test.Operand = -17;
    Test.Answer = 4;
    Test.ExpectC = true;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
    TestADC_ZPX(Test);
}
TEST_F(M6502AddWithCarryTest, INS_ADC_ABS_AddZerosToGetZero) {
    ADCTestData Test = {};
    Test.CarryBefore = false;
    Test.A = 0x00;
    Test.Operand = 0x00;
    Test.Answer = 0x00;
    Test.ExpectC = false;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = true;
    TestADC_ABS(Test);
}
TEST_F(M6502AddWithCarryTest, INS_ADC_ABS_AddCarryAndZerosToGetOne) {
    ADCTestData Test = {};
    Test.CarryBefore = true;
    Test.A = 0x00;
    Test.Operand = 0x00;
    Test.Answer = 0x01;
    Test.ExpectC = false;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
    TestADC_ABS(Test);
}
TEST_F(M6502AddWithCarryTest, INS_ADC_ABS_AddOneToFFAndGetACarry) {
    ADCTestData Test = {};
    Test.CarryBefore = false;
    Test.A = 0xFF;
    Test.Operand = 0x01;
    Test.Answer = 0x00;
    Test.ExpectC = true;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = true;
    TestADC_ABS(Test);
}
TEST_F(M6502AddWithCarryTest, INS_ADC_ABS_SetsNegativeFlagIfResutIsNegative) {
    ADCTestData Test = {};
    Test.CarryBefore = false;
    Test.A = 0;
    Test.Operand = -1;
    Test.Answer = -1;
    Test.ExpectC = false;
    Test.ExpectN = true;
    Test.ExpectV = false;
    Test.ExpectZ = false;
    TestADC_ABS(Test);
}
TEST_F(M6502AddWithCarryTest, INS_ADC_ABS_AddTwoPositiveNumbersWithCarry) {
    ADCTestData Test = {};
    Test.CarryBefore = true;
    Test.A = 20;
    Test.Operand = 17;
    Test.Answer = 38;
    Test.ExpectC = false;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
    TestADC_ABS(Test);
}
TEST_F(M6502AddWithCarryTest, INS_ADC_ABS_AddPositiveAndNegativeNumbersWithCarry) {
    // A:       00010100 +20
    // Operand: 11111111 -17
    // Answer:  00000100 +4
    // C:1, N:0, V:0, Z:0

    ADCTestData Test = {};
    Test.CarryBefore = true;
    Test.A = 20;
    Test.Operand = -17;
    Test.Answer = 4;
    Test.ExpectC = true;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
    TestADC_ABS(Test);
}
TEST_F(M6502AddWithCarryTest, INS_ADC_ABS_SetsOverflowFlagIfSignedNegativeAdditionFails) {
    // A:       10000000 -128
    // Operand: 11111111 -1
    // Answer:  01111111 +127
    // C:1, N:0, V:1, Z:0

    ADCTestData Test = {};
    Test.CarryBefore = false;
    Test.A = -128;
    Test.Operand = -1;
    Test.Answer = 127;
    Test.ExpectC = true;
    Test.ExpectN = false;
    Test.ExpectV = true;
    Test.ExpectZ = false;
    TestADC_ABS(Test);
}
TEST_F(M6502AddWithCarryTest, INS_ADC_ABS_SetsOverflowFlagIfSignedNegativeAdditionFailsWithCarry) {
    // A:       10000000 -128
    // Operand: 11111111 -1
    // Answer:  10000000 -128
    // C:1, N:1, V:0, Z:0

    ADCTestData Test = {};
    Test.CarryBefore = true;
    Test.A = -128;
    Test.Operand = -1;
    Test.Answer = -128;
    Test.ExpectC = true;
    Test.ExpectN = true;
    Test.ExpectV = false;
    Test.ExpectZ = false;
    TestADC_ABS(Test);
}
TEST_F(M6502AddWithCarryTest, INS_ADC_ABS_SetsOverflowFlagIfSignedPositiveAdditionFails) {
    // A:       0111111 +127
    // Operand: 0000001 +1
    // Answer:  1000000 -128
    // C:0, N:1, V:1, Z:0

    ADCTestData Test = {};
    Test.CarryBefore = false;
    Test.A = 127;
    Test.Operand = 1;
    Test.Answer = 128;
    Test.ExpectC = false;
    Test.ExpectN = true;
    Test.ExpectV = true;
    Test.ExpectZ = false;
    TestADC_ABS(Test);
}
TEST_F(M6502AddWithCarryTest, INS_ADC_ABSX_AddTwoPositiveNumbersWithCarry) {
    ADCTestData Test = {};
    Test.CarryBefore = true;
    Test.A = 20;
    Test.Operand = 17;
    Test.Answer = 38;
    Test.ExpectC = false;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
    TestADC_ABSX(Test);
}
TEST_F(M6502AddWithCarryTest, INS_ADC_ABSX_AddPositiveAndNegativeNumbersWithCarry) {
    // A:       00010100 +20
    // Operand: 11111111 -17
    // Answer:  00000100 +4
    // C:1, N:0, V:0, Z:0

    ADCTestData Test = {};
    Test.CarryBefore = true;
    Test.A = 20;
    Test.Operand = -17;
    Test.Answer = 4;
    Test.ExpectC = true;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
    TestADC_ABSX(Test);
}
TEST_F(M6502AddWithCarryTest, INS_ADC_ABSY_AddTwoPositiveNumbersWithCarry) {
    ADCTestData Test = {};
    Test.CarryBefore = true;
    Test.A = 20;
    Test.Operand = 17;
    Test.Answer = 38;
    Test.ExpectC = false;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
    TestADC_ABSY(Test);
}
TEST_F(M6502AddWithCarryTest, INS_ADC_ABSY_AddPositiveAndNegativeNumbersWithCarry) {
    // A:       00010100 +20
    // Operand: 11111111 -17
    // Answer:  00000100 +4
    // C:1, N:0, V:0, Z:0

    ADCTestData Test = {};
    Test.CarryBefore = true;
    Test.A = 20;
    Test.Operand = -17;
    Test.Answer = 4;
    Test.ExpectC = true;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
    TestADC_ABSY(Test);
}
TEST_F(M6502AddWithCarryTest, INS_ADC_INDX_AddTwoPositiveNumbersWithCarry) {
    ADCTestData Test = {};
    Test.CarryBefore = true;
    Test.A = 20;
    Test.Operand = 17;
    Test.Answer = 38;
    Test.ExpectC = false;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
    TestADC_INDX(Test);
}
TEST_F(M6502AddWithCarryTest, INS_ADC_INDX_AddPositiveAndNegativeNumbersWithCarry) {
    // A:       00010100 +20
    // Operand: 11111111 -17
    // Answer:  00000100 +4
    // C:1, N:0, V:0, Z:0

    ADCTestData Test = {};
    Test.CarryBefore = true;
    Test.A = 20;
    Test.Operand = -17;
    Test.Answer = 4;
    Test.ExpectC = true;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
    TestADC_INDX(Test);
}
TEST_F(M6502AddWithCarryTest, INS_ADC_INDY_AddTwoPositiveNumbersWithCarry) {
    ADCTestData Test = {};
    Test.CarryBefore = true;
    Test.A = 20;
    Test.Operand = 17;
    Test.Answer = 38;
    Test.ExpectC = false;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
    TestADC_INDY(Test);
}
TEST_F(M6502AddWithCarryTest, INS_ADC_INDY_AddPositiveAndNegativeNumbersWithCarry) {
    // A:       00010100 +20
    // Operand: 11111111 -17
    // Answer:  00000100 +4
    // C:1, N:0, V:0, Z:0

    ADCTestData Test = {};
    Test.CarryBefore = true;
    Test.A = 20;
    Test.Operand = -17;
    Test.Answer = 4;
    Test.ExpectC = true;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
    TestADC_INDY(Test);
}