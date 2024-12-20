#include "gtest/gtest.h"

#include "core/arm1176jzf_s/core.hpp"

TEST(umull_instruction, test_01)
{
    using namespace zero_mate::arm1176jzf_s;

    CCPU_Core cpu{};

    cpu.Execute({
    { 0xe3e00009 }, // mov r0, #0xFFFFFFF6
    { 0xe3e01009 }, // mov r1, #0xFFFFFFF6
    { 0xe0932190 }  // umulls r2, r3, r0, r1
    });

    EXPECT_EQ(cpu.Get_CPU_Context()[2], 0x00000064);
    EXPECT_EQ(cpu.Get_CPU_Context()[3], 0xFFFFFFec);

    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::N), true);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::Z), false);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::C), false);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::V), false);
}

TEST(umull_instruction, test_02)
{
    using namespace zero_mate::arm1176jzf_s;

    CCPU_Core cpu{};

    cpu.Execute({
    { 0xe3e00000 }, // mvn r0, #0
    { 0xe3a01001 }, // mov r1, #1
    { 0xe0932190 }  // umulls r2, r3, r0, r1
    });

    EXPECT_EQ(cpu.Get_CPU_Context()[2], 0xFFFFFFFF);
    EXPECT_EQ(cpu.Get_CPU_Context()[3], 0x0);

    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::N), false);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::Z), false);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::C), false);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::V), false);
}

TEST(umull_instruction, test_03)
{
    using namespace zero_mate::arm1176jzf_s;

    CCPU_Core cpu{};

    cpu.Execute({
    { 0xe3a00c02 }, // mov r0, #512
    { 0xe3a01b01 }, // mov r1, #1024
    { 0xe0932190 }  // umulls r2, r3, r0, r1
    });

    EXPECT_EQ(cpu.Get_CPU_Context()[2], 512 * 1024);
    EXPECT_EQ(cpu.Get_CPU_Context()[3], 0);

    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::N), false);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::Z), false);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::C), false);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::V), false);
}

TEST(umull_instruction, test_04)
{
    using namespace zero_mate::arm1176jzf_s;

    CCPU_Core cpu{};

    cpu.Execute({
    { 0xe3a00000 }, // mov r0, #0
    { 0xe3a01b01 }, // mov r1, #1024
    { 0xe0932190 }  // umulls r2, r3, r0, r1
    });

    EXPECT_EQ(cpu.Get_CPU_Context()[2], 0);
    EXPECT_EQ(cpu.Get_CPU_Context()[3], 0);

    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::N), false);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::Z), true);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::C), false);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::V), false);
}