/*****************************************************************************
 * iMXRT_Helper.js
 *
 * Helper functions for iMXRT target 
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
 *****************************************************************************/

// Formats a 32 bit value as hex string for log messages.
function Hex (value)
{
   return "0x" + (value >>> 0).toString (16);
}

// Fill a memory area with a 32 bit value
function FillMemory (address, size, value)
{
	var fillData = new Array (size/4);
	for (var i=0; i<size/4; i++)
		fillData[i] = value;
	TargetInterface.pokeMultUint32 (address, fillData);
}

// Read-modify-write of a 32 bit register: (old & ~clearMask) | setMask.
function ModifyRegister (address, clearMask, setMask)
{
   var value = TargetInterface.peekUint32 (address);
   value = (value & ~clearMask) | setMask;
   TargetInterface.pokeUint32 (address, value);
}


function GetProjectPartName ()
{
	var targetFullName = TargetInterface.getProjectProperty ("Target");
	var targetShort;
	if (targetFullName.charAt(9) > '9')
		targetShort = targetFullName.substring (0, 9);	// Three digits number in the target name
	else
		targetShort = targetFullName.substring (0, 10);	// Four digits number in the target name

	switch (targetFullName.slice(-4))
	{
		case "_cm7":
			targetShort += '_cm7';
			break;
		case "_cm4":
			targetShort += '_cm4';
			break;
		case "cm33":
			targetShort += '_cm33';
			break;
		case "ore0":
			targetShort += '_core0';
			break;
		case "ore1":
			targetShort += '_core1';
			break;
		default:
			// Do nothing
	}

	return targetShort;
}

// ---------------------------------------------------------------------------
// Boot ROM stall reset with debug mailbox fallback
// Shared by the i.MX RT600 and the i.MX RT700 (CPU0), see iMXRT600_Target.js
// and iMXRT700_Target.js for the background. RM chapter numbers in the
// comments refer to the RT600 reference manual.
// ---------------------------------------------------------------------------

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

// Time the boot ROM gets to enable the debug access after the plain debug
// mailbox request (ms), see StartDebugSessionVerified (). The ROM services the
// request within a few ms [M: RT700-EVK]; if it does not at all, waiting longer
// does not help.
var DEBUG_SESSION_SETTLE_MS = 200;

// Poll interval while waiting for the debug mailbox to complete the
// resynchronisation / chip reset requested with CSW = 0x21 (ms).
var DM_CHIP_RESET_POLL_MS = 10;

// Poll interval while waiting for the halt (ms).
var HALT_POLL_MS = 10;

// 1: call TargetInterface.resetDebugInterface() after SYSRESETREQ.
// The RM says the debug domain survives non-POR resets, and the NXP script
// does not reconnect, so this is OFF. (Measured on an RT1062 with LinkServer:
// the DAP is lost after SYSRESETREQ there. If the RT600 behaves like that,
// turn this on.) [A]
var RESYNC_DAP_AFTER_SYSRESET = 0;

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
//   1. Only with chipReset = 1: request re-synchronisation of the mailbox and
//      reset the chip (CSW = RESYNCH_REQ | CHIP_RESET_REQ = 0x21), wait until
//      CSW reads 0. [M: RT685] On the RT700 this step is only a fallback (see
//      StartDebugSessionVerified ()): with a valid image the debug access of
//      CPU0 stays dead after the chip reset request even after the command
//      0x07 [M: RT700-EVK]. NXP's LinkServer sends only the command.
//   2. Write command 0x07 (Start DBG Session) to REQUEST, wait until RETURN
//      reads 0 (success). The ROM then removes ISP/boot peripherals, enables
//      the CPU0 debug access and waits in a while(1) loop.
// Returns 1 on success, 0 on error (message is printed).
function StartDebugSession (chipReset)
{
	TargetInterface.message ("## debug mailbox - start debug session");

	// The first access after a reset may fail silently (an AP register read
	// then returns 0 instead of raising an error [M]), so retry a few times.
	var id = 0;
	var maxAttempts = (chipReset != 0) ? 3 : 20;
	for (var attempt = 0; attempt < maxAttempts; attempt++)
	{
		id = TargetInterface.getDebugRegister (DM_ID);
		if (id == 0x002A0000)
			break;
		// Nothing: the debug port may be powered down. Re-establish it, but not
		// in quick succession [M: RT700, resets 10 ms apart never revived the DAP,
		// one after a pause of 100 ms did].
		TargetInterface.delay (100);
		TargetInterface.resetDebugInterface ();
	}
	TargetInterface.message ("## DM-AP ID after " + attempt + " attempts: " + Hex (id));
	if (id != 0x002A0000)
	{
		TargetInterface.message ("## DM-AP not found at AP index 2 (ID=" + Hex (id) + ", expected 0x2a0000)");
		// With an unserviced request pending (DM-AP CSW.REQ_PENDING) the ID of the
		// DM-AP reads as 0, but the CSW write of the resynchronisation is still
		// accepted and clears that state [M: RT700-EVK]. So the variant with chip
		// reset goes on, the plain request does not.
		if (chipReset == 0)
			return 0;
	}

	if (chipReset != 0)
	{
		// The chip reset takes the debug port down: it has to be powered up again
		// before every access, otherwise writes are lost silently and reads
		// return 0, which looks like "done" [M: RT700-EVK]. Also re-establish it
		// before the write: a previous failed access leaves an error behind that
		// swallows the write.
		TargetInterface.resetDebugInterface ();
		TargetInterface.setDebugRegister (DM_CSW, DM_CSW_RESYNCH_AND_CHIP_RESET);
		// Note: no "!" on numbers in this script dialect, "!1" evaluates to true
		// [M: crossscript.exe]. Compare with == 0 instead.
		var waited = 0;
		var csw = 1;
		while (waited < 1000)
		{
			TargetInterface.delay (DM_CHIP_RESET_POLL_MS);
			waited += DM_CHIP_RESET_POLL_MS;
			TargetInterface.resetDebugInterface ();
			csw = TargetInterface.getDebugRegister (DM_CSW);
			if ((csw & 0xFFFF) == 0)
				break;
		}
		if ((csw & 0xFFFF) != 0)
		{
			TargetInterface.message ("## DM-AP resynchronisation timed out, CSW=" + Hex (csw));
			return 0;
		}
		TargetInterface.resetDebugInterface ();
	}

	TargetInterface.setDebugRegister (DM_REQUEST, DM_CMD_START_DBG_SESSION);
	if (WaitDmRegisterZero (DM_RETURN, 1000) == 0)
	{
		TargetInterface.message ("## DM-AP start debug session timed out, RETURN=" + Hex (TargetInterface.getDebugRegister (DM_RETURN)));
		return 0;
	}

	TargetInterface.message ("## debug session started");
	return 1;
}


// "Start Debug Session" with a check and a fallback, for chips where the boot
// ROM does not always service the plain request (RT700).
// 1. Send the plain request (StartDebugSession (0)), exactly like NXP's
//    LinkServer, and wait until the memory access of AHB-AP 0 is enabled.
// 2. If it is not (the request stays pending, DM-AP CSW.REQ_PENDING, RETURN
//    stays 0 and therefore looks like a success): resynchronise the mailbox
//    and reset the chip (CSW = 0x21), then send the request again
//    [M: RT700-EVK with an invalid image in the flash; the plain request is
//    never serviced there, the variant with chip reset is].
// Returns 1 if the debug access of CPU0 is enabled afterwards, otherwise 0.
function StartDebugSessionVerified ()
{
	StartDebugSession (0);
	if (WaitForDebugAccessResync (DEBUG_SESSION_SETTLE_MS) != 0)
		return 1;

	TargetInterface.message ("## WARNING - debug mailbox request was not serviced, retrying with chip reset");
	StartDebugSession (1);
	if (WaitForDebugAccessResync (HALT_TIMEOUT_MS) == 0)
	{
		TargetInterface.message ("## WARNING - debug access of CPU0 is still blocked");
		return 0;
	}
	return 1;
}

// Like WaitForAhbAccess (), but re-establishes the debug port before every
// check. After the debug mailbox request (and especially after its chip reset)
// the debug port has to be powered up again, otherwise the AP registers read as
// 0 [M: RT700-EVK]. Returns 1 if DeviceEn became 1 within timeoutMs.
function WaitForDebugAccessResync (timeoutMs)
{
	var waited = 0;
	while (waited < timeoutMs)
	{
		// Give the ROM time first, it is busy with the request.
		TargetInterface.delay (20);
		waited += 20;
		TargetInterface.resetDebugInterface ();
		if ((TargetInterface.getDebugRegister (AHB_AP0_CSW) & AP_CSW_DEVICE_EN) != 0)
			return 1;
	}
	return 0;
}


// Resets the system of CPU0 and halts the core right after the boot ROM.
// stallAddress:    address the boot ROM touches when it has finished
//                  (NXP: "bootromstall" in the device files).
// useDebugMailbox: 1 = fall back to the debug mailbox when the ROM keeps the
//                  debug access blocked, see USE_DEBUG_MAILBOX.
// mailboxResetsChip: 0 / 1 are passed to StartDebugSession () as chipReset,
//                  2 = StartDebugSessionVerified () (plain request first, the
//                  variant with chip reset if the request is not serviced).
// Only AHB-AP 0 and the DM-AP (AP 2) are used.
function ResetWithBootRomStall (stallAddress, useDebugMailbox, mailboxResetsChip)
{
	// Step 1 - NXP: cmhalt
	// Halt the core (DHCSR.C_DEBUGEN | C_HALT). C_DEBUGEN is required so that
	// DWT matches generate a debug event (halt) instead of a DebugMonitor
	// exception.
	TargetInterface.stop (HALT_TIMEOUT_MS);

	// Step 2 - NXP: cmwatchset this 0 <stall address> RW
	// Arm DWT comparator 0 to halt the core on a 32 bit access to the boot ROM
	// stall address. CrossWorks has no watchpoint API in the script
	// interface, therefore the DWT registers are written directly.
	// DEMCR.TRCENA must be 1, otherwise the DWT registers are not accessible
	// [A: set explicitly here, the NXP tool does this inside cmwatchset].
	ModifyRegister (DEMCR, 0, DEMCR_TRCENA);
	TargetInterface.pokeUint32 (DWT_FUNCTION0, 0);                               // disable first (prevents a spurious match while changing COMP0)
	TargetInterface.pokeUint32 (DWT_COMP0, stallAddress);               // address to watch
	TargetInterface.pokeUint32 (DWT_FUNCTION0, DWT_FUNCTION_DEBUG_EVENT_RW_WORD); // enable
	TargetInterface.message ("## watchpoint armed on " + Hex (stallAddress));

	// Step 3 - NXP: cmresetvectorcatchclear
	// Switch off the reset vector catch. It would halt the core already at the
	// reset (inside the ROM), where the ROM blocks debug access.
	ModifyRegister (DEMCR, DEMCR_VC_CORERESET, 0);

	// Step 4 - NXP: cmsysresetreq
	// Request a system reset via AIRCR.SYSRESETREQ. The core and the
	// peripherals are reset; the debug domain (DWT/DEMCR/DHCSR) keeps its
	// state (RM 49.7). The boot ROM starts and eventually accesses the stall
	// address -> DWT match -> core halts.
	TargetInterface.message ("## SYSRESETREQ");
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
			TargetInterface.message ("## core halted by the boot ROM watchpoint (DFSR=" + Hex (dfsr) + ")");
		else
			TargetInterface.message ("## core halted, but not by DWT (DFSR=" + Hex (dfsr) + ")");
	}
	else
	{
		if (accessible == 0)
		{
			// Debug access stays blocked by the ROM (erased flash, invalid
			// image, ISP mode). The only way to get control of the core again is
			// the debug mailbox: it resets the chip, the ROM then enables debug
			// access and waits with the core halted (RM 49.6.1).
			if (useDebugMailbox == 0)
			{
				TargetInterface.error ("the boot ROM blocks debug access after the reset (no valid image in flash?). Set USE_DEBUG_MAILBOX = 1 in the target script.");
			}
			TargetInterface.message ("## WARNING - ROM blocks debug access (no valid boot image?), falling back to the debug mailbox");
			var started = 0;
			if (mailboxResetsChip == 2)
				started = StartDebugSessionVerified ();
			else
				started = StartDebugSession (mailboxResetsChip);
			if (started == 0)
			{
				TargetInterface.error ("debug mailbox start debug session failed");
			}
		}
		else
		{
			// The ROM released the debug access but never touched the watched
			// address, the core is running.
			TargetInterface.message ("## WARNING - watchpoint did not fire within " + HALT_TIMEOUT_MS + " ms, halting the core manually");
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
	// application (it would halt the core at the next access to the stall
	// address).
	TargetInterface.pokeUint32 (DWT_FUNCTION0, 0);
	TargetInterface.pokeUint32 (DWT_COMP0, 0);
}
