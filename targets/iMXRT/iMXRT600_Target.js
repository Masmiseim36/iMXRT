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

// DWT_FUNCTIONn value for the watchpoint (Armv8-M DWT):
//    bits  3:0  MATCH     = 0b0100  data address match, read or write
//    bits  5:4  ACTION    = 0b01    generate a debug event (halt the core)
//    bits 11:10 DATAVSIZE = 0b10    word (32 bit) access
// = 0x814.
// LinkServer's "cmwatchset ... RW" writes 0x14 (without DATAVSIZE) [M].
// The NXP script uses "RW". The RM (49.9.3) only requires a READ access;
// a read-only watchpoint would be 0x816. All three values halted the core on
// a read access in a test on the RT685 [M].
var DWT_FUNCTION_DEBUG_EVENT_RW_WORD = 0x00000814;

// Time to wait after SYSRESETREQ before the first DHCSR access (ms).
// Gives the ROM time to start; the core is expected to halt on the watchpoint.
var RESET_SETTLE_MS = 100;

// Maximum time to wait for the core to halt on the watchpoint (ms).
var HALT_TIMEOUT_MS = 1000;

// Poll interval while waiting for the halt (ms).
var HALT_POLL_MS = 10;

// 1: call TargetInterface.resetDebugInterface() after SYSRESETREQ.
// The RM says the debug domain survives non-POR resets, and the NXP script
// does not reconnect, so this is OFF. (Measured on an RT1062 with LinkServer:
// the DAP is lost after SYSRESETREQ there. If the RT600 behaves like that,
// turn this on.) [A]
var RESYNC_DAP_AFTER_SYSRESET = 0;

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
// Cortex-M33 debug registers (System Control Space / DWT)
// ---------------------------------------------------------------------------
var DHCSR = 0xE000EDF0;   // Debug Halting Control and Status Register
var DFSR  = 0xE000ED30;   // Debug Fault Status Register
var DEMCR = 0xE000EDFC;   // Debug Exception and Monitor Control Register
var AIRCR = 0xE000ED0C;   // Application Interrupt and Reset Control Register

var DWT_COMP0     = 0xE0001020;   // DWT comparator 0 (RT600: 4 comparators, RM 49.5.1)
var DWT_FUNCTION0 = 0xE0001028;   // DWT function register 0

var DHCSR_S_HALT       = 0x00020000;   // bit 17: core is halted
var DEMCR_VC_CORERESET = 0x00000001;   // bit 0: halt on core reset (vector catch)
var DEMCR_TRCENA       = 0x01000000;   // bit 24: enable DWT/ITM/trace blocks
var DFSR_DWTTRAP       = 0x00000004;   // bit 2: halt caused by a DWT match
var AIRCR_SYSRESETREQ  = 0x05FA0004;   // VECTKEY 0x05FA + SYSRESETREQ (bit 2)

// ---------------------------------------------------------------------------
// Debug mailbox (DM-AP) registers, RM 49.5.5
// ---------------------------------------------------------------------------
// Register address for get/setDebugRegister: (AP number << 24) | offset
var DM_AP        = 0x02000000;            // DM-AP is AP index 2 (AP0 = Cortex-M33, AP1 = HiFi4)
var DM_CSW       = DM_AP | 0x000;         // command/status; bit 0 RESYNCH_REQ, bit 5 CHIP_RESET_REQ
var DM_REQUEST   = DM_AP | 0x004;         // request from debugger to ROM
var DM_RETURN    = DM_AP | 0x008;         // response from ROM to debugger
var DM_ID        = DM_AP | 0x0FC;         // identification, expected 0x002A0000
// AHB-AP 0 (Cortex-M33) CSW register and its DeviceEn bit (ADIv5 MEM-AP)
var AHB_AP0_CSW       = 0x00000000;   // (AP 0 << 24) | offset 0x00
var AP_CSW_DEVICE_EN  = 0x00000040;   // bit 6: memory access enabled [M: 0 while the ROM blocks debug access]

var DM_CSW_RESYNCH_AND_CHIP_RESET = 0x21; // RESYNCH_REQ | CHIP_RESET_REQ
var DM_CMD_START_DBG_SESSION      = 7;    // command code 0x07 "Start DBG Session"


// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// Polls DHCSR.S_HALT.
// Returns 1 if the core halted within timeoutMs, otherwise 0.
// (TargetInterface.isStopped() is deliberately not used: in the reset /
// download context it always returns false, see tools/crossscript E-6.)
function WaitForHalt (timeoutMs)
{
	var waited = 0;
	while (waited < timeoutMs)
	{
		if ((TargetInterface.peekUint32 (DHCSR) & DHCSR_S_HALT) != 0)
			return 1;
		TargetInterface.delay (HALT_POLL_MS);
		waited += HALT_POLL_MS;
	}
	return ((TargetInterface.peekUint32 (DHCSR) & DHCSR_S_HALT) != 0) ? 1 : 0;
}

// Waits until the memory access of AHB-AP 0 is enabled again (CSW.DeviceEn,
// bit 6). While the boot ROM runs, the ROM blocks CPU0 debug access
// (Boot-ROM protection, RM 49.6): DeviceEn reads 0, and every memory access
// through the AP (peekUint32 ...) fails with an error that aborts the script.
// Reading the AP register itself does not fail [M], so this is a safe way to
// find out when memory accesses are possible again.
// Returns 1 if DeviceEn became 1 within timeoutMs, otherwise 0.
function WaitForAhbAccess (timeoutMs)
{
	var waited = 0;
	while (waited < timeoutMs)
	{
		if ((TargetInterface.getDebugRegister (AHB_AP0_CSW) & AP_CSW_DEVICE_EN) != 0)
			return 1;
		TargetInterface.delay (HALT_POLL_MS);
		waited += HALT_POLL_MS;
	}
	return ((TargetInterface.getDebugRegister (AHB_AP0_CSW) & AP_CSW_DEVICE_EN) != 0) ? 1 : 0;
}

// Waits until the low 16 bits of a DM-AP register read as zero.
// Returns 1 on success, 0 on timeout.
function WaitDmRegisterZero (registerAddress, timeoutMs)
{
	var waited = 0;
	while (waited < timeoutMs)
	{
		if ((TargetInterface.getDebugRegister (registerAddress) & 0xFFFF) == 0)
			return 1;
		TargetInterface.delay (1);
		waited += 1;
	}
	return 0;
}

// Debug mailbox "Start Debug Session" (translation of the mailbox part of
// NXP LS_preconnect_MIMXRT600.scp: SelectProbeCore a% 2 / DebugMailboxReq 7).
// RM 49.6.1 / 49.9.1:
//   1. Request re-synchronisation of the mailbox and reset the chip
//      (CSW = RESYNCH_REQ | CHIP_RESET_REQ = 0x21), wait until CSW reads 0.
//   2. Write command 0x07 (Start DBG Session) to REQUEST, wait until RETURN
//      reads 0 (success). The ROM then removes ISP/boot peripherals, enables
//      the CPU0 debug access and waits in a while(1) loop.
// Returns 1 on success, 0 on error (message is printed).
function StartDebugSession ()
{
	TargetInterface.message ("## RT600: debug mailbox - start debug session");

	// The first access after a reset may fail silently (an AP register read
	// then returns 0 instead of raising an error [M]), so retry a few times.
	var id = 0;
	for (var attempt = 0; attempt < 10; attempt++)
	{
		id = TargetInterface.getDebugRegister (DM_ID);
		if (id == 0x002A0000)
		break;
		if (attempt == 2)
		{
			// Still nothing: the debug port may be powered down.
			TargetInterface.resetDebugInterface ();
		}
		TargetInterface.delay (10);
	}
	if (id != 0x002A0000)
	{
		TargetInterface.message ("## RT600: DM-AP not found at AP index 2 (ID=" + Hex (id) + ", expected 0x2a0000)");
		return 0;
	}

	TargetInterface.setDebugRegister (DM_CSW, DM_CSW_RESYNCH_AND_CHIP_RESET);
	// Note: no "!" on numbers in this script dialect, "!1" evaluates to true
	// [M: crossscript.exe]. Compare with == 0 instead.
	if (WaitDmRegisterZero (DM_CSW, 1000) == 0)
	{
		TargetInterface.message ("## RT600: DM-AP resynchronisation timed out, CSW=" + Hex (TargetInterface.getDebugRegister (DM_CSW)));
		return 0;
	}

	TargetInterface.setDebugRegister (DM_REQUEST, DM_CMD_START_DBG_SESSION);
	if (WaitDmRegisterZero (DM_RETURN, 1000) == 0)
	{
		TargetInterface.message ("## RT600: DM-AP start debug session timed out, RETURN=" + Hex (TargetInterface.getDebugRegister (DM_RETURN)));
		return 0;
	}

	TargetInterface.message ("## RT600: debug session started");
	return 1;
}


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
		StartDebugSession ();
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

	// Step 1 - NXP: cmhalt
	// Halt the core (DHCSR.C_DEBUGEN | C_HALT). C_DEBUGEN is required so that
	// DWT matches generate a debug event (halt) instead of a DebugMonitor
	// exception.
	TargetInterface.stop (HALT_TIMEOUT_MS);

	// Step 2 - NXP: cmwatchset this 0 0x50002034 RW
	// Arm DWT comparator 0 to halt the core on a 32 bit access to the boot ROM
	// stall address. CrossWorks has no watchpoint API in the script
	// interface, therefore the DWT registers are written directly.
	// DEMCR.TRCENA must be 1, otherwise the DWT registers are not accessible
	// [A: set explicitly here, the NXP tool does this inside cmwatchset].
	ModifyRegister (DEMCR, 0, DEMCR_TRCENA);
	TargetInterface.pokeUint32 (DWT_FUNCTION0, 0);                               // disable first (prevents a spurious match while changing COMP0)
	TargetInterface.pokeUint32 (DWT_COMP0, BOOTROM_STALL_ADDRESS);               // address to watch
	TargetInterface.pokeUint32 (DWT_FUNCTION0, DWT_FUNCTION_DEBUG_EVENT_RW_WORD); // enable
	TargetInterface.message ("## RT600: watchpoint armed on " + Hex (BOOTROM_STALL_ADDRESS));

	// Step 3 - NXP: cmresetvectorcatchclear
	// Switch off the reset vector catch. It would halt the core already at the
	// reset (inside the ROM), where the ROM blocks debug access.
	ModifyRegister (DEMCR, DEMCR_VC_CORERESET, 0);

	// Step 4 - NXP: cmsysresetreq
	// Request a system reset via AIRCR.SYSRESETREQ. The core and the
	// peripherals are reset; the debug domain (DWT/DEMCR/DHCSR) keeps its
	// state (RM 49.7). The boot ROM starts and eventually accesses 0x50002034
	// -> DWT match -> core halts.
	TargetInterface.message ("## RT600: SYSRESETREQ");
	TargetInterface.pokeUint32 (AIRCR, AIRCR_SYSRESETREQ);

	TargetInterface.delay (RESET_SETTLE_MS);
	if (RESYNC_DAP_AFTER_SYSRESET)
	{
		// Re-establish the debug port without touching the target.
		TargetInterface.resetDebugInterface ();
	}

	// Step 5 - (implicit in the NXP tool) wait for the halt
	// cmsysresetreq in LinkServer returns when the target is accessible
	// again. Here this is done explicitly in two stages:
	//  a) wait until the ROM releases the memory access of AHB-AP 0
	//     (without a valid image in flash that NEVER happens: the ROM sits in
	//     its ISP loop and keeps debug access disabled),
	//  b) wait until the core has halted on the watchpoint.
	var accessible = WaitForAhbAccess (HALT_TIMEOUT_MS);
	var halted = 0;
	if (accessible != 0)
	{
		halted = WaitForHalt (HALT_TIMEOUT_MS);
	}

	if (halted != 0)
	{
		var dfsr = TargetInterface.peekUint32 (DFSR);
		if ((dfsr & DFSR_DWTTRAP) != 0)
			TargetInterface.message ("## RT600: core halted by the boot ROM watchpoint (DFSR=" + Hex (dfsr) + ")");
		else
			TargetInterface.message ("## RT600: core halted, but not by DWT (DFSR=" + Hex (dfsr) + ")");
	}
	else
	{
		if (accessible == 0)
		{
			// Debug access stays blocked by the ROM (erased flash, invalid
			// image, ISP mode). The only way to get control of the core again is
			// the debug mailbox: it resets the chip, the ROM then enables debug
			// access and waits with the core halted (RM 49.6.1).
			if (USE_DEBUG_MAILBOX == 0)
			{
				TargetInterface.error ("RT600: the boot ROM blocks debug access after the reset (no valid image in flash?). Set USE_DEBUG_MAILBOX = 1 in the target script.");
			}
			TargetInterface.message ("## RT600: WARNING - ROM blocks debug access (no valid boot image?), falling back to the debug mailbox");
			if (StartDebugSession () == 0)
			{
				TargetInterface.error ("RT600: debug mailbox start debug session failed");
			}
		}
		else
		{
			// The ROM released the debug access but never touched the watched
			// address, the core is running.
			TargetInterface.message ("## RT600: WARNING - watchpoint did not fire within " + HALT_TIMEOUT_MS + " ms, halting the core manually");
		}
		// Halt the core so that the debugger gets a defined state.
		TargetInterface.stop (HALT_TIMEOUT_MS);
	}

	// Step 6 - NXP: cmresetvectorcatchset
	// Re-enable the reset vector catch. Resets issued later by the debugger
	// (e.g. after the application has started) are then caught at the
	// application's reset vector.
	ModifyRegister (DEMCR, 0, DEMCR_VC_CORERESET);

	// Step 7 - NXP: cmwatchclear this 0
	// Disarm the watchpoint again so that it does not influence the
	// application (it would halt the core at the next SysTick calibration
	// access).
	TargetInterface.pokeUint32 (DWT_FUNCTION0, 0);
	TargetInterface.pokeUint32 (DWT_COMP0, 0);

	TargetInterface.message ("## Reset i.MX RT600 - done");
}
