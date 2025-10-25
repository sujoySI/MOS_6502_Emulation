#include "M6502Lib/cpu.h"
#include <gtest/gtest.h>

TEST(M6502, JSR_LoadTest) {
    MEMORY mem{};
    CPU cpu{};
    cpu.Reset(mem);
    //StartInline
    mem.Data[0xFFFC] = CPU::INS_JSR;
    mem.Data[0xFFFD] = 0x42;
    mem.Data[0xFFFE] = 0x42;
    mem.Data[0x4242] = CPU::INS_LDA_IM;
    mem.Data[0x4243] = 0x84;
    //EndInline
    auto cycles = cpu.Execute(8, mem);
    // std::cout<<"A: "<<cpu.A<<"\n";
    //here in clion debug, A is shown in 132 decimal (grey colour) and '\204' octal (green colour)
    EXPECT_EQ(cpu.A, 0x84);
    EXPECT_EQ(cycles, 0);
}

TEST(M6502, LDA_Load) {
    MEMORY mem{};
    CPU cpu{};
    cpu.Reset(mem);
    //StartInline
    mem.Data[0xFFFC] = CPU::INS_JSR;
    mem.Data[0xFFFD] = 0x42;
    mem.Data[0xFFFE] = 0x42;
    mem.Data[0x4242] = CPU::INS_LDA_IM;
    mem.Data[0x4243] = 0x84;
    //EndInline
    auto cycles = cpu.Execute(8, mem);
    // std::cout<<"A: "<<cpu.A<<"\n";
    //here in clion debug, A is shown in 132 decimal (grey colour) and '\204' octal (green colour)
    EXPECT_EQ(cpu.A, 0x84);
    EXPECT_EQ(cycles, 0);
}
