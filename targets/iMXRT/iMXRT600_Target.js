/*****************************************************************************
 * iMXRT600_Target.js
 *
 * CrossWorks (CrossStudio / CrossConnect) target script for the NXP i.MX RT600
 * family (RT633 / RT685, Cortex-M33 = CPU0).
 * 
 * Copyright (C) 2019-2026 Markus Klein
 * https://github.com/Masmiseim36/iMXRT
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * -------------------------------------------------------------------------
 * Why this script exists - the problem it solves
 * -------------------------------------------------------------------------
 *    The RT600 boot ROM blocks the usual "halt at reset vector" trick:
 *      - While the PC is in ROM, the debug access of CPU0 is disabled (Boot-ROM
 *        protection, RM chapter 49.6) and a reset vector catch does not work.
 *      - RM chapter 49.9.3 ("Halting execution immediately following ROM
 *        execution") describes the official workaround: set a DWT watchpoint
 *        on address 0x50002034. The ROM accesses this address when it has
 *        finished its preparations, the watchpoint halts the core there, and
 *        the debugger gets control right after the ROM and before the
 *        application. LinkServer calls this setting "bootromstall"
 *        ("bootromstall": "0x50002034" in the device JSON files).
 *      - 0x50002034 is SYSCTL0 (secure alias 0x5000_2000) + offset 0x34 =
 *        SYSCTL0_SYSTEM_STICK_CALIB (secure SysTick calibration, RM 4.5.5.5).
 *        Why exactly this register is touched by the ROM is not documented
 *        in the RM (assumption: the ROM sets up SysTick calibration for the
 *        application at its hand-over).
 *
 *    The NXP script therefore does:
 *       halt -> arm watchpoint -> clear reset vector catch -> SYSRESETREQ
 *       -> (core runs ROM, watchpoint fires, core halts)
 *       -> set reset vector catch -> disarm watchpoint
 *
 *    The debug domain (DP, Cortex-M33 AP, DM-AP) is only reset by a power-on
 *    reset (RM 49.7). DWT/DEMCR/DHCSR settings therefore survive the
 *    SYSRESETREQ. The whole procedure relies on this.
 *
 *****************************************************************************/


// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------

// Address the boot ROM touches when it is done (SYSCTL0_SYSTEM_STICK_CALIB,
// secure alias). Same value as "bootromstall" in the NXP device files.
var BOOTROM_STALL_ADDRESS = 0x50002034;

// 1: perform the debug mailbox "Start Debug Session" request (from NXP
// LS_preconnect_MIMXRT600.scp) in Connect(), and as a fallback in Reset()
// when the ROM blocks debug access after the reset.
// Needed if the boot ROM blocks the debug access (no valid image in flash,
// ISP mode, erased flash); NXP's LinkServer always does this for the RT600.
// Side effect: every connect resets the chip (CHIP_RESET_REQ), so attaching
// to a running application without resetting it is not possible with 1.
// Set to 0 only if a valid image is in flash and debug is enabled in it.
// Tested on the RT685 [M].
var USE_DEBUG_MAILBOX = 1;

// 1: write the FlexSPI flash reset pin configuration to the shadow OTP
// register BOOT_CFG1 (from NXP LS_preconnect_MIMXRT600.scp). Ignored when 0.
// WARNING: value is board specific (NXP default: PIO2_12 as flash reset).
var SET_FLEXSPI_RESET_PIN = 0;

// SHADOW_OTP BOOT_CFG1 = (1 << 14) | (PORT << 15) | (PIN << 18)
//    PORT 2, PIN 12: 0x4000 | 0x10000 | 0x300000 = 0x00314000
var OTP_SHADOW_BOOT_CFG1_ADDRESS = 0x40130184;   // OTP controller 0x4013_0000, [A: offset from NXP script]
var OTP_SHADOW_BOOT_CFG1_VALUE   = 0x00314000;


// ---------------------------------------------------------------------------
// Connect hook
// ---------------------------------------------------------------------------

// Called first, BEFORE the engine identifies the core (AP 0, ROM table).
// Only debug interface properties and the debug mailbox (DM-AP) are used
// here, no target memory access.
//
// Replaces the NXP lines
//    wireswdconnect p%        (SWD line reset / connect  -> CrossWorks engine)
//    selectprobecore p% 0     (select CPU0 = Cortex-M33 AHB-AP index 0)
//    cminitapdp this          (power up DP, init AP      -> CrossWorks engine)
function Connect_RT600 ()
{
	TargetInterface.message ("## Connect to i.MX RT600 (Cortex-M33)");

	// Use AHB-AP 0 (= Cortex-M33 core, RM Figure 277) for all memory accesses.
	// 2nd argument 0x40000000: bits cleared in the AP CSW register,
	// 3rd argument 0x00000000: bits set in the AP CSW register.
	// CSW bit 30 = SPROT: 0 = "secure" transfers. The Cortex-M33 has
	// TrustZone; without this the debugger would access only the non-secure
	// alias and e.g. 0x5000_2034 (secure alias of SYSCTL0) would not work.
	TargetInterface.setDebugInterfaceProperty ("set_adiv5_AHB_ap_num", 0, 0x40000000, 0x00000000);

	// Optional (default off), from NXP LS_preconnect_MIMXRT600.scp.
	// Must happen here and not in Reset(): the engine identifies the core
	// (AP 0, ROM table) right after Connect(), and with ROM-blocked debug
	// access (erased flash / ISP mode) that fails with "Can't find debug
	// component in ROM table". The DM-AP (AP 2) is reachable at this point
	// [M: DM-AP ID read from Connect() returned 0x002a0000].
	if (USE_DEBUG_MAILBOX)
	{
		StartDebugSession (1);
	}

	TargetInterface.message ("## Connect - done");
}


// ---------------------------------------------------------------------------
// Reset hook
// ---------------------------------------------------------------------------

// Called after every reset requested by the debugger (download, Debug >
// Reset, start of debug session).
// Result: the core is halted right after the boot ROM has finished,
// before the application runs. Note: the PC then is inside the ROM
// (immediately after the access to 0x50002034), not at the application's
// reset vector.
function Reset_RT600 ()
{
	if (TargetInterface.implementation () == "crossworks_simulator")
		return;

	TargetInterface.message ("## Reset i.MX RT600");

	// Optional (default off), from NXP LS_preconnect_MIMXRT600.scp:
	// tell the ROM which GPIO resets the FlexSPI flash. The ROM evaluates this
	// during the following boot (BOOT_CFG1, RM 42 "FlexSPI boot": bit 14
	// FLEXSPI_RESET_PIN_ENABLE, then port/pin).
	if (SET_FLEXSPI_RESET_PIN)
	{
		TargetInterface.message ("## RT600: set FlexSPI flash reset pin in shadow OTP BOOT_CFG1");
		TargetInterface.pokeUint32 (OTP_SHADOW_BOOT_CFG1_ADDRESS, OTP_SHADOW_BOOT_CFG1_VALUE);
	}

	ResetWithBootRomStall (BOOTROM_STALL_ADDRESS, USE_DEBUG_MAILBOX, 1);

	TargetInterface.message ("## Reset i.MX RT600 - done");
}
