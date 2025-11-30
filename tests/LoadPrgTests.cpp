#include "M6502Lib/cpu.h"
#include <gtest/gtest.h>
/*
TestPrgOrignal Input

* = $1000

lda #$FF

start
sta $90
sta $8000
eor #$CC
jmp start

*/
static m6502::UI8 TestPrg[] = {
    0x00, 0x10, 0xA9, 0xFF, 0x85, 0x90, 0x8D,
    0x00, 0x80, 0x49, 0xCC, 0x4C, 0x02, 0x10 };
static constexpr m6502::SI32 NumBytesInPrg = 14;

class M6502LoadPrgTests : public ::testing::Test {
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

TEST_F(M6502LoadPrgTests, TestLoadProgramIntoCorrectAreaOfMemory) {
    //Given:
    using namespace m6502;

    //When:
    m6502::CPU::LoadPrg(TestPrg, NumBytesInPrg, mem);

    //Then:
    EXPECT_EQ(mem.Data[0x0FFF] ,0x00);
    EXPECT_EQ(mem.Data[0x1000] ,0xA9);
    EXPECT_EQ(mem.Data[0x1001] ,0xFF);
    EXPECT_EQ(mem.Data[0x1002] ,0x85);
    /* ****************** */
    EXPECT_EQ(mem.Data[0x1009] ,0x4C);
    EXPECT_EQ(mem.Data[0x100A] ,0x02);
    EXPECT_EQ(mem.Data[0x100B] ,0x10);
    EXPECT_EQ(mem.Data[0x100C] ,0x00);
}
TEST_F(M6502LoadPrgTests, TestLoadProgramAndExecuteIt) {
    //Given:
    using namespace m6502;

    //When:
    const UI16 StartAddress = cpu.LoadPrg(TestPrg, NumBytesInPrg, mem);
    cpu.PC = StartAddress;

    //Then:
    // std::cout<<"\n";
    for (SI32 clock = 1000; clock > 0;) {
        clock -= cpu.Execute(1, mem);
        cpu.PrintStatusHex();
    }
}