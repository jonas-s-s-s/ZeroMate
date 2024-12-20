// ---------------------------------------------------------------------------------------------------------------------
/// \file context.cpp
/// \date 24. 05. 2023
/// \author Jakub Silhavy (jakub.silhavy.cz@gmail.com)
///
/// \brief This file implements the context (registers) of the CPU.
// ---------------------------------------------------------------------------------------------------------------------

// 3rd party library includes

#include <magic_enum.hpp>

// Project file imports

#include "context.hpp"
#include "exceptions.hpp"

#include "zero_mate/utils/singleton.hpp"

namespace zero_mate::arm1176jzf_s
{
    CCPU_Context::CCPU_Context()
    : m_mode{ NCPU_Mode::Supervisor }
    , m_regs{}
    , m_logging_system{ utils::CSingleton<utils::CLogging_System>::Get_Instance() }
    {
        Reset();
    }

    void CCPU_Context::Reset()
    {
        // The CPU starts up in the Supervisor mode.
        m_mode = NCPU_Mode::Supervisor;

        Init_Registers();

        // Disable IRQ and FIQ.
        Enable_IRQ(false);
        Enable_FIQ(false);
    }

    inline void CCPU_Context::Init_Registers()
    {
        // Set all registers r0-r15 to 0.
        m_regs.fill(0);

        // Set all banked registers to 0.
        Init_FIQ_Banked_Regs();
        Init_IRQ_Banked_Regs();
        Init_Supervisor_Banked_Regs();
        Init_Undefined_Banked_Regs();
        Init_Abort_Banked_Regs();

        // Set all CPSR registers to 0
        // (except for their least significant 5 bits - CPU mode itself)
        Init_CPSR();

        // Set all SPSR registers to 0
        Init_SPSR();
    }

    void CCPU_Context::Enable_IRQ(bool enable)
    {
        // The enabled bit is actually inverted.
        Set_Flag(m_cpsr[m_mode], NFlag::I, !enable);
    }

    void CCPU_Context::Enable_FIQ(bool enable)
    {
        // The enabled bit is actually inverted.
        Set_Flag(m_cpsr[m_mode], NFlag::F, !enable);
    }

    void CCPU_Context::Init_FIQ_Banked_Regs()
    {
        for (std::uint32_t idx = 8; idx <= 14; ++idx)
        {
            m_banked_regs[NCPU_Mode::FIQ][idx] = { 0 };
        }
    }

    void CCPU_Context::Init_IRQ_Banked_Regs()
    {
        m_banked_regs[NCPU_Mode::IRQ][13] = { 0 };
        m_banked_regs[NCPU_Mode::IRQ][14] = { 0 };
    }

    void CCPU_Context::Init_Supervisor_Banked_Regs()
    {
        m_banked_regs[NCPU_Mode::Supervisor][13] = { 0 };
        m_banked_regs[NCPU_Mode::Supervisor][14] = { 0 };
    }

    void CCPU_Context::Init_Undefined_Banked_Regs()
    {
        m_banked_regs[NCPU_Mode::Undefined][13] = { 0 };
        m_banked_regs[NCPU_Mode::Undefined][14] = { 0 };
    }

    void CCPU_Context::Init_Abort_Banked_Regs()
    {
        m_banked_regs[NCPU_Mode::Abort][13] = { 0 };
        m_banked_regs[NCPU_Mode::Abort][14] = { 0 };
    }

    inline void CCPU_Context::Init_CPSR()
    {
        m_cpsr[NCPU_Mode::User] = { static_cast<std::uint32_t>(NCPU_Mode::User) };
        m_cpsr[NCPU_Mode::FIQ] = { static_cast<std::uint32_t>(NCPU_Mode::FIQ) };
        m_cpsr[NCPU_Mode::IRQ] = { static_cast<std::uint32_t>(NCPU_Mode::IRQ) };
        m_cpsr[NCPU_Mode::Supervisor] = { static_cast<std::uint32_t>(NCPU_Mode::Supervisor) };
        m_cpsr[NCPU_Mode::Abort] = { static_cast<std::uint32_t>(NCPU_Mode::Abort) };
        m_cpsr[NCPU_Mode::Undefined] = { static_cast<std::uint32_t>(NCPU_Mode::Undefined) };
        m_cpsr[NCPU_Mode::System] = { static_cast<std::uint32_t>(NCPU_Mode::System) };
    }

    inline void CCPU_Context::Init_SPSR()
    {
        m_spsr[NCPU_Mode::FIQ] = { 0 };
        m_spsr[NCPU_Mode::IRQ] = { 0 };
        m_spsr[NCPU_Mode::Supervisor] = { 0 };
        m_spsr[NCPU_Mode::Abort] = { 0 };
        m_spsr[NCPU_Mode::Undefined] = { 0 };
    }

    const std::uint32_t& CCPU_Context::operator[](std::uint32_t idx) const
    {
        return Get_Register(idx, m_mode);
    }

    std::uint32_t& CCPU_Context::operator[](std::uint32_t idx)
    {
        return Get_Register(idx, m_mode);
    }

    const std::uint32_t& CCPU_Context::Get_Register(std::uint32_t idx, NCPU_Mode mode) const
    {
        // Check if a banked register should be returned.
        if (m_banked_regs.contains(mode) && m_banked_regs.at(mode).contains(idx))
        {
            return m_banked_regs.at(mode).at(idx);
        }

        return m_regs.at(idx);
    }

    std::uint32_t& CCPU_Context::Get_Register(std::uint32_t idx, NCPU_Mode mode)
    {
        // Check if a banked register should be returned.
        if (m_banked_regs.contains(mode) && m_banked_regs.at(mode).contains(idx))
        {
            return m_banked_regs[mode][idx];
        }

        return m_regs[idx];
    }

    std::uint32_t& CCPU_Context::Get_CPSR()
    {
        return m_cpsr[m_mode];
    }

    const std::uint32_t& CCPU_Context::Get_CPSR() const
    {
        return m_cpsr.at(m_mode);
    }

    void CCPU_Context::Set_CPSR(std::uint32_t new_cpsr)
    {
        // Make sure the change is allowed.
        if (Invalid_Change_Of_Control_Bits(new_cpsr))
        {
            m_logging_system->Error("Attempt to change the control bits in a non-privileged mode (User mode)");
            return;
        }

        // Switch off to the new mode of the CPU.
        Set_CPU_Mode(Get_CPU_Mode(new_cpsr));

        // TODO these two lines might be redundant?
        new_cpsr &= ~CPU_Mode_Mask;
        new_cpsr |= static_cast<std::uint32_t>(Get_CPU_Mode());

        // Set the new value of the CPSR register in the new mode of the CPU.
        m_cpsr[m_mode] = new_cpsr;
    }

    bool CCPU_Context::Invalid_Change_Of_Control_Bits(std::uint32_t new_cpsr) noexcept
    {
        if (m_mode == NCPU_Mode::User)
        {
            // Check if least significant byte would change.
            return (new_cpsr & CPU_Control_Bits_Mask) != (Get_CPSR() & CPU_Control_Bits_Mask);
        }

        // The CPU is in a privileged mode - manipulation is allowed.
        return false;
    }

    void CCPU_Context::Verify_SPSR_Accessibility(NCPU_Mode mode) const
    {
        if (Is_Mode_With_No_SPSR(mode))
        {
            // clang-format off
            m_logging_system->Error(fmt::format("Invalid access to the SPSR register (it is not supported in "
                                                "the {} mode)", magic_enum::enum_name(mode)).c_str());
            // clang-format on

            throw exceptions::CReset{};
        }
    }

    std::uint32_t CCPU_Context::Get_SPSR() const
    {
        Verify_SPSR_Accessibility(m_mode);
        return m_spsr.at(m_mode);
    }

    std::uint32_t& CCPU_Context::Get_SPSR(NCPU_Mode mode)
    {
        Verify_SPSR_Accessibility(mode);
        return m_spsr[mode];
    }

    const std::uint32_t& CCPU_Context::Get_SPSR(NCPU_Mode mode) const
    {
        Verify_SPSR_Accessibility(mode);
        return m_spsr.at(mode);
    }

    void CCPU_Context::Set_SPSR(std::uint32_t value)
    {
        Verify_SPSR_Accessibility(m_mode);
        m_spsr[m_mode] = value;
    }

    void CCPU_Context::Set_Flag(NFlag flag, bool set) noexcept
    {
        Set_Flag(m_cpsr[m_mode], flag, set);
    }

    bool CCPU_Context::Is_Flag_Set(NFlag flag) const noexcept
    {
        return Is_Flag_Set(m_cpsr.at(m_mode), flag);
    }

    void CCPU_Context::Set_CPU_Mode(NCPU_Mode mode) noexcept
    {
        if (!Is_Mode_With_No_SPSR(mode))
        {
            // SPSR of the new mode = CPSR of the current mode.
            m_spsr[mode] = m_cpsr[m_mode];
        }

        m_mode = mode;
    }

    CCPU_Context::NCPU_Mode CCPU_Context::Get_CPU_Mode() const noexcept
    {
        return m_mode;
    }

    void CCPU_Context::Set_Flag(std::uint32_t& cpsr, NFlag flag, bool set) noexcept
    {
        if (set)
        {
            cpsr |= static_cast<std::uint32_t>(flag);
        }
        else
        {
            cpsr &= ~static_cast<std::uint32_t>(flag);
        }
    }

    bool CCPU_Context::Is_Flag_Set(std::uint32_t cpsr, NFlag flag) noexcept
    {
        return static_cast<bool>(cpsr & static_cast<std::uint32_t>(flag));
    }

    CCPU_Context::NCPU_Mode CCPU_Context::Get_CPU_Mode(std::uint32_t cpsr) noexcept
    {
        return static_cast<NCPU_Mode>(cpsr & 0b11111U);
    }

    bool CCPU_Context::Is_In_Privileged_Mode() const noexcept
    {
        const auto mode = Get_CPU_Mode();

        return mode != NCPU_Mode::User;
    }

    bool CCPU_Context::Is_Mode_With_No_SPSR(NCPU_Mode mode) noexcept
    {
        return (mode == NCPU_Mode::User) || (mode == NCPU_Mode::System);
    }

} // namespace zero_mate::arm1176jzf_s