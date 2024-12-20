// ---------------------------------------------------------------------------------------------------------------------
/// \file cp10_window.hpp
/// \date 07. 09. 2023
/// \author Jakub Silhavy (jakub.silhavy.cz@gmail.com)
///
/// \brief This file defines a window that displays data (information) related to coprocessor 10 (FPU).
// ---------------------------------------------------------------------------------------------------------------------

#pragma once

#include "../window.hpp"
#include "../../core/coprocessors/cp10/cp10.hpp"

namespace zero_mate::gui
{
    // -----------------------------------------------------------------------------------------------------------------
    /// \class CCP10_Window
    /// \brief This class represents a window that displays information related to CP10 (coprocessor 10).
    // -----------------------------------------------------------------------------------------------------------------
    class CCP10_Window : public IGUI_Window
    {
    public:
        // -------------------------------------------------------------------------------------------------------------
        /// \brief Creates an instance of the class.
        /// \param cp10 Reference to coprocessor 10
        // -------------------------------------------------------------------------------------------------------------
        explicit CCP10_Window(std::shared_ptr<coprocessor::cp10::CCP10> cp10);

        // -------------------------------------------------------------------------------------------------------------
        /// \brief Renders the window (IGUI_Window interface).
        // -------------------------------------------------------------------------------------------------------------
        void Render() override;

    private:
        // -------------------------------------------------------------------------------------------------------------
        /// \brief Renders single precision registers of the FPU (float).
        // -------------------------------------------------------------------------------------------------------------
        void Render_Registers();

        // -------------------------------------------------------------------------------------------------------------
        /// \brief Renders the flags in the FPSCR register.
        // -------------------------------------------------------------------------------------------------------------
        void Render_FPSCR();

        // -------------------------------------------------------------------------------------------------------------
        /// \brief Renders the flags in the FPEXC register.
        // -------------------------------------------------------------------------------------------------------------
        void Render_FPEXC();

    private:
        std::shared_ptr<coprocessor::cp10::CCP10> m_cp10; ///< Coprocessor 10
    };

} // namespace zero_mate::gui