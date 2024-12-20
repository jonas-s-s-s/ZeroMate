#include "gtest/gtest.h"

#include "core/peripherals/ram.hpp"
#include "core/arm1176jzf_s/core.hpp"

using namespace zero_mate;

static constexpr std::uint32_t RAM_Size = 1024;

TEST(rfeda_instruction, test_01)
{
    auto ram = std::make_shared<peripheral::CRAM>(RAM_Size);
    auto bus = std::make_shared<CBus>();

    EXPECT_EQ(bus->Attach_Peripheral(0x0, ram), CBus::NStatus::OK);

    arm1176jzf_s::CCPU_Core cpu{ 0, bus };

    cpu.Execute({
    { 0xe3a0e004 }, // mov lr, #4
    { 0xe10f0000 }, // mrs r0, cpsr
    { 0xe3800102 }, // orr r0, r0, #0x80000000
    { 0xe169f000 }, // msr spsr, r0
    { 0xe3a0d0c8 }, // mov sp, #200
    { 0xf84d0513 }, // srsda #19
    { 0xe3a000c8 }, // mov r0, #200
    { 0xf8300a00 }  // rfeda r0!
    });

    EXPECT_EQ(cpu.Get_CPU_Context()[arm1176jzf_s::CCPU_Context::PC_Reg_Idx], 4);
    EXPECT_EQ(cpu.Get_CPU_Context().Get_CPSR(), 0b1000'0000'0000'0000'0000'0000'1101'0011U);
    EXPECT_EQ(cpu.Get_CPU_Context()[0], 192);
}

TEST(rfeda_instruction, test_02)
{
    auto ram = std::make_shared<peripheral::CRAM>(RAM_Size);
    auto bus = std::make_shared<CBus>();

    EXPECT_EQ(bus->Attach_Peripheral(0x0, ram), CBus::NStatus::OK);

    arm1176jzf_s::CCPU_Core cpu{ 0, bus };

    cpu.Execute({
    { 0xe3a0e004 }, // mov lr, #4
    { 0xe10f0000 }, // mrs r0, cpsr
    { 0xe3800102 }, // orr r0, r0, #0x80000000
    { 0xe169f000 }, // msr spsr, r0
    { 0xe3a0d0c8 }, // mov sp, #200
    { 0xf84d0513 }, // srsda #19
    { 0xe3a000c8 }, // mov r0, #200
    { 0xf8100a00 }  // rfeda r0
    });

    EXPECT_EQ(cpu.Get_CPU_Context()[arm1176jzf_s::CCPU_Context::PC_Reg_Idx], 4);
    EXPECT_EQ(cpu.Get_CPU_Context().Get_CPSR(), 0b1000'0000'0000'0000'0000'0000'1101'0011U);
    EXPECT_EQ(cpu.Get_CPU_Context()[0], 200);
}