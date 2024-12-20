#include "gtest/gtest.h"

#include "core/arm1176jzf_s/core.hpp"
#include "register_state_checker.hpp"

TEST(teq_instruction, test_01)
{
    using namespace zero_mate::arm1176jzf_s;
    using namespace zero_mate::test;

    CCPU_Core cpu{};

    CRegister_State_Checker<CCPU_Context> register_state_checker{};
    register_state_checker.Record_State(cpu.Get_CPU_Context());

    cpu.Execute({
    { 0xe1300000 } // teq r0, r0
    });

    // clang-format off
    EXPECT_EQ(register_state_checker.Is_Any_Other_Register_Modified(cpu.Get_CPU_Context(), {{}}), false);
    // clang-format on

    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::N), false);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::Z), true);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::C), false);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::V), false);
}

TEST(teq_instruction, test_02)
{
    using namespace zero_mate::arm1176jzf_s;
    using namespace zero_mate::test;

    CCPU_Core cpu{};

    CRegister_State_Checker<CCPU_Context> register_state_checker{};
    register_state_checker.Record_State(cpu.Get_CPU_Context());

    cpu.Execute({
    { 0xe3a00000 }, // mov r0, #0
    { 0xe3e01000 }, // mvn r1, #0
    { 0xe1300001 }  // teq r0, r1
    });

    // clang-format off
    EXPECT_EQ(register_state_checker.Is_Any_Other_Register_Modified(
              cpu.Get_CPU_Context(), { { .idx = 0, .expected_value = 0x00000000 },
                            { .idx = 1, .expected_value = 0xFFFFFFFF } }), false);
    // clang-format on

    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::N), true);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::Z), false);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::C), false);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::V), false);
}

TEST(teq_instruction, test_03)
{
    using namespace zero_mate::arm1176jzf_s;
    using namespace zero_mate::test;

    CCPU_Core cpu{};

    CRegister_State_Checker<CCPU_Context> register_state_checker{};
    register_state_checker.Record_State(cpu.Get_CPU_Context());

    cpu.Execute({
    { 0xe3a0000f }, // mov r0, #15
    { 0xe3a02001 }, // mov r2, #1
    { 0xe1b010e2 }  // movs r1, r2, ROR #1
    });

    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::N), true);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::Z), false);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::C), true);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::V), false);

    cpu.Execute({
    { 0xe1300001 } // teq r0, r1
    });

    // clang-format off
    EXPECT_EQ(register_state_checker.Is_Any_Other_Register_Modified(
              cpu.Get_CPU_Context(), { { .idx = 0, .expected_value = 15 },
                            { .idx = 2, .expected_value = 1  },
                            { .idx = 1, .expected_value = 0x80000000 } }), false);
    // clang-format on

    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::N), true);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::Z), false);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::C), true);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::V), false);
}

TEST(teq_instruction, test_04)
{
    using namespace zero_mate::arm1176jzf_s;
    using namespace zero_mate::test;

    CCPU_Core cpu{};

    CRegister_State_Checker<CCPU_Context> register_state_checker{};
    register_state_checker.Record_State(cpu.Get_CPU_Context());

    cpu.Execute({
    { 0xe3a00000 }, // mov r0, #0
    { 0xe3a01000 }, // mov r1, #0
    { 0xe1300001 }  // teq r0, r1
    });

    // clang-format off
    EXPECT_EQ(register_state_checker.Is_Any_Other_Register_Modified(
              cpu.Get_CPU_Context(), { { .idx = 0, .expected_value = 0 },
                            { .idx = 1, .expected_value = 0 } }), false);
    // clang-format on

    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::N), false);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::Z), true);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::C), false);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::V), false);
}

TEST(teq_instruction, test_05)
{
    using namespace zero_mate::arm1176jzf_s;
    using namespace zero_mate::test;

    CCPU_Core cpu{};

    CRegister_State_Checker<CCPU_Context> register_state_checker{};
    register_state_checker.Record_State(cpu.Get_CPU_Context());

    cpu.Execute({
    { 0xe3a00002 }, // mov r0, #0b10
    { 0xe3a01001 }, // mov r1, #0b01
    { 0xe1300001 }  // teq r0, r1
    });

    // clang-format off
    EXPECT_EQ(register_state_checker.Is_Any_Other_Register_Modified(
              cpu.Get_CPU_Context(), { { .idx = 0, .expected_value = 0b10 },
                            { .idx = 1, .expected_value = 0b01 } }), false);
    // clang-format on

    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::N), false);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::Z), false);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::C), false);
    EXPECT_EQ(cpu.Get_CPU_Context().Is_Flag_Set(CCPU_Context::NFlag::V), false);
}