/*****************************************************************************
 * iMXRT2660_Target.js
 *
 * CrossWorks (CrossStudio / CrossConnect) target script for the NXP i.MX RT2660
 * family (RT2661 / RT2662 / RT2663). Single core: Cortex-M85.
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
 * Overview
 * -------------------------------------------------------------------------
 * Based on the NXP LinkServer scripts RT2660_connect.scp and RT2660_reset.scp.
 *
 * NOT TESTED ON HARDWARE. This script was written from the NXP scripts and the
 * reference manual (draft for early customers) only. Statements which are not
 * confirmed by the manual or the NXP scripts are marked with [A] (assumption).
 *
 * Debug access points (RM chapter 7, Table 46):
 *    AP 0  Debug mailbox (ISP-AP, DBGMB, ID register 0x002A0000)
 *    AP 2  SoC AHB-AP (system bus)
 *    AP 3  Cortex-M85 CPU0 (AHB-AP)                  <- the debug target
 *    AP 4  APB-AP to the CoreSight components (APBIC, 0x41F0_0000)
 *    AP 6  MDM-AP
 *
 * Memory accesses of the script use the secure alias of the Cortex-M85
 * (TrustZone): the secure addresses are the non-secure ones + 0x1000_0000
 * (RM 3.2), e.g. the ROM at 0x1080_0000 and POWERCON_SOC_CTRL at 0x5054_8000.
 *
 * What the script does (all in the Reset hook, see Reset_RT2660 ()):
 *    The boot ROM is not allowed to run into the application: the NXP script
 *    halts the core inside the ROM with a DWT instruction match on a "trap"
 *    address of the ROM (the ROM boot flow would otherwise jump into the XiP
 *    image and leave the external flash in an access mode which can not be
 *    read by SFDP any more). The ROM trap address depends on the ROM version.
 *    If the trap did not fire (e.g. the core was halted by something else),
 *    the SoC is brought into a defined state by hand (stack limits, MPU, NVIC).
 *
 * What the script does NOT do (differences to the NXP scripts):
 *    - The NXP connect script resets the chip while connecting. Here the
 *      Connect and GetPartName hooks do not touch the target, so attaching to
 *      a running application does not change its state. The reset is done in
 *      the Reset hook only.
 *    - The NXP reset script returns a reset context (PC, SP, XPSR, VTOR) to the
 *      debugger and performs a soft reset for RAM based applications. CrossWorks
 *      has no such interface; the application is started by the debugger.
 *    - The debug mailbox is not used (the NXP scripts do not use it either).
 *
 *****************************************************************************/


// Version of the boot ROM (secure alias) and the pointer to the trap address.
var ROM_VERSION_ADDRESS_RT2660       = 0x1082FFE8;
var ROM_TRAP_POINTER_ADDRESS_RT2660  = 0x1082FFEC;

// ROM version for which the trap address is known; for all other versions the
// ROM provides the address in ROM_TRAP_POINTER_ADDRESS_RT2660 (bit 0 = Thumb).
var ROM_VERSION_KNOWN_RT2660         = 0x54010000;
var ROM_TRAP_ADDRESS_KNOWN_RT2660    = 0x1080B872;

// DWT_FUNCTIONn value for the ROM trap (Armv8.1-M DWT):
//    bits  3:0  MATCH     = 0b0010  instruction address match
//    bits  5:4  ACTION    = 0b01    generate a debug event (halt the core)
//    bits 11:10 DATAVSIZE = 0b01    halfword (Thumb instruction)
// = 0x412, value of the NXP script.
var DWT_FUNCTION_DEBUG_EVENT_EXEC_RT2660 = 0x00000412;
var DWT_FUNCTION_MATCHED_RT2660          = 0x01000000;   // bit 24: comparator matched

// POWERCON_SOC_CTRL (secure alias) GPR_COLD_0. NXP: "Force cold boot on next
// reset: set bit 0". The meaning of the bit is not documented in the RM [A]
// (the register is reset by a cold reset only).
var POWERCON_GPR_COLD0_RT2660        = 0x50548080;
var POWERCON_GPR_COLD0_FORCE_COLD    = 0x00000001;

// 1: call TargetInterface.resetDebugInterface() after the SYSRESETREQ.
// The NXP connect script re-attaches the debug link after the reset
// (WireSwdConnect / APList / CMInitApDp). The NXP reset script does not. It is
// harmless to re-establish the debug port: the debug registers of the core
// (DWT, DEMCR, DHCSR) are not touched by it [A].
var RESYNC_DAP_AFTER_SYSRESET_RT2660 = 1;

// Cortex-M85 registers used for the SoC cleanup (NXP: "CM85 SoC cleanup")
var DCRSR_RT2660           = 0xE000EDF4;   // Debug Core Register Selector Register
var DCRDR_RT2660           = 0xE000EDF8;   // Debug Core Register Data Register
var DCRSR_WRITE_MSPLIM_NS  = 0x0001001C;   // REGWnR = 1 (write), REGSEL = 0x1C (MSPLIM_NS)
var DCRSR_WRITE_MSPLIM_S   = 0x0001001D;   // REGWnR = 1 (write), REGSEL = 0x1D (MSPLIM_S)
var MSPLIM_RESET_VALUE_RT2660 = 0x20000000;   // start of the D-TCM
var MPU_CTRL_RT2660        = 0xE000ED94;
var MPU_RNR_RT2660         = 0xE000ED98;
var MPU_RBAR_RT2660        = 0xE000ED9C;
var MPU_RLAR_RT2660        = 0xE000EDA0;
var MPU_REGIONS_RT2660     = 16;
var NVIC_ICER0_RT2660      = 0xE000E180;   // NVIC_ICER0..7: 8 registers, 32 interrupt enable bits each


// Called from Connect ()
// Only debug interface properties are used here, no target memory access.
// Replaces the NXP lines
//    wireswdconnect p%        (SWD line reset / connect  -> CrossWorks engine)
//    selectprobecore p% 3     (select CPU0 = Cortex-M85 AHB-AP index 3)
//    cminitapdp this          (power up DP, init AP      -> CrossWorks engine)
function Connect_RT2660 ()
{
	// Use AHB-AP 3 (= Cortex-M85) for the memory accesses of the script and for
	// the identification of the core (the other APs: debug mailbox, SoC AHB-AP,
	// ...). CSW bit 30 = SPROT: 0 = "secure" transfers. The Cortex-M85 has
	// TrustZone; without this the debugger would access only the non-secure
	// alias and e.g. the ROM at 0x1080_0000 would not be accessible.
	TargetInterface.setDebugInterfaceProperty ("set_adiv5_AHB_ap_num", 3, 0x40000000, 0x00000000);
}


// Called from Reset ()
// Result: the core is halted inside the boot ROM (at the trap address of the
// ROM), before the ROM has started the application.
function Reset_RT2660 ()
{
	// NXP: CMHalt
	// C_DEBUGEN is required so that the DWT match generates a debug event (halt)
	// instead of a DebugMonitor exception.
	TargetInterface.stop (HALT_TIMEOUT_MS);

	// NXP: select the trap address from the ROM version
	var trapAddress = GetRomTrapAddress_RT2660 ();
	TargetInterface.message ("## ROM trap address: " + Hex (trapAddress));

	// NXP: DWT_COMP0 = trap address, DWT_FUNC0 = 0x412 (exec match, debug event)
	// DEMCR.TRCENA must be 1, otherwise the DWT registers are not accessible
	// [A: set explicitly here, the NXP script relies on the halt for it].
	ModifyRegister (DEMCR, 0, DEMCR_TRCENA);
	TargetInterface.pokeUint32 (DWT_FUNCTION0, 0);	// disable first (prevents a spurious match while changing COMP0)
	TargetInterface.pokeUint32 (DWT_COMP0, trapAddress);
	TargetInterface.pokeUint32 (DWT_FUNCTION0, DWT_FUNCTION_DEBUG_EVENT_EXEC_RT2660);

	// A reset vector catch would halt the core already at the reset vector of
	// the ROM, and the trap would never be reached [A: same as RT600/RT700].
	// It is re-enabled below.
	ModifyRegister (DEMCR, DEMCR_VC_CORERESET, 0);

	// NXP: force a cold boot of the ROM on the next reset
	ModifyRegister (POWERCON_GPR_COLD0_RT2660, 0, POWERCON_GPR_COLD0_FORCE_COLD);

	// NXP: system reset request via AIRCR (0xE000ED0C = 0x05FA0004)
	TargetInterface.message ("## SYSRESETREQ");
	TargetInterface.pokeUint32 (AIRCR, AIRCR_SYSRESETREQ);
	TargetInterface.delay (RESET_SETTLE_MS);
	if (RESYNC_DAP_AFTER_SYSRESET_RT2660 != 0)
	{
		TargetInterface.resetDebugInterface ();
	}

	// The ROM runs now and the core is expected to halt at the trap address.
	// NXP: CMHalt (halt the core if it is not halted yet)
	if (WaitForHalt (HALT_TIMEOUT_MS) == 0)
	{
		TargetInterface.message ("## WARNING - the ROM trap did not halt the core within " + HALT_TIMEOUT_MS + " ms, halting the core manually");
		TargetInterface.stop (HALT_TIMEOUT_MS);
	}

	// NXP: check DWT_FUNC0 bit 24 (MATCHED). If the trap did not fire, the SoC
	// is in an unknown state: clean it up.
	var function0 = TargetInterface.peekUint32 (DWT_FUNCTION0);
	if ((function0 & DWT_FUNCTION_MATCHED_RT2660) != 0)
	{
		TargetInterface.message ("## core halted by the ROM trap (DWT_FUNCTION0=" + Hex (function0) + ")");
	}
	else
	{
		TargetInterface.message ("## ROM trap did not fire (DWT_FUNCTION0=" + Hex (function0) + ") - cleaning up the SoC");
		Cleanup_RT2660 ();
	}

	// Disarm the trap so that it does not influence the application
	TargetInterface.pokeUint32 (DWT_FUNCTION0, 0);
	TargetInterface.pokeUint32 (DWT_COMP0, 0);

	// Re-enable the reset vector catch: resets issued later by the debugger
	// are caught at the reset vector then.
	ModifyRegister (DEMCR, 0, DEMCR_VC_CORERESET);
}


// Returns the address of the ROM trap: the boot ROM calls this address when it
// has finished its preparations and is about to start the application.
// The core must be halted and the memory access has to be secure.
function GetRomTrapAddress_RT2660 ()
{
	var romVersion = TargetInterface.peekUint32 (ROM_VERSION_ADDRESS_RT2660);
	TargetInterface.message ("## ROM version: " + Hex (romVersion));

	if (romVersion == ROM_VERSION_KNOWN_RT2660)
		return ROM_TRAP_ADDRESS_KNOWN_RT2660;

	// Newer ROM: the ROM provides the address, bit 0 is the Thumb bit
	var trapPointer = TargetInterface.peekUint32 (ROM_TRAP_POINTER_ADDRESS_RT2660);
	return trapPointer & 0xFFFFFFFE;
}


// Bring the Cortex-M85 into a defined state (NXP "CM85 SoC cleanup")
// The core must be halted.
function Cleanup_RT2660 ()
{
	TargetInterface.message ("## CM85 SoC cleanup");

	// Set MSPLIM_NS and MSPLIM_S to the start of the D-TCM via DCRDR / DCRSR.
	TargetInterface.pokeUint32 (DCRDR_RT2660, MSPLIM_RESET_VALUE_RT2660);
	TargetInterface.pokeUint32 (DCRSR_RT2660, DCRSR_WRITE_MSPLIM_NS);
	TargetInterface.pokeUint32 (DCRDR_RT2660, MSPLIM_RESET_VALUE_RT2660);
	TargetInterface.pokeUint32 (DCRSR_RT2660, DCRSR_WRITE_MSPLIM_S);

	// Disable and clear the MPU (16 regions)
	TargetInterface.pokeUint32 (MPU_CTRL_RT2660, 0);
	for (var region = 0; region < MPU_REGIONS_RT2660; region++)
	{
		TargetInterface.pokeUint32 (MPU_RNR_RT2660,  region);
		TargetInterface.pokeUint32 (MPU_RBAR_RT2660, 0);
		TargetInterface.pokeUint32 (MPU_RLAR_RT2660, 0);
	}

	// Disable all interrupts: NVIC_ICER0..7
	FillMemory (NVIC_ICER0_RT2660, 8 * 4, 0xFFFFFFFF);
}
