/*****************************************************************************
 * iMXRT700_Target.js
 *
 * CrossWorks (CrossStudio / CrossConnect) target script for the NXP i.MX RT700
 * family (RT735 / RT758 / RT798). Dual core: two Cortex-M33,
 * CPU0 = compute domain, CPU1 = sense domain.
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
 * Based on the NXP LinkServer scripts RT700_connect.scp, RT700_reset.scp and
 * LS_preconnect_MIMXRT700.scp.
 *
 * Debug access points (RM chapter "System Debug"):
 *    AP 0  Cortex-M33 CPU0  (compute domain)     -> target "_core0"
 *    AP 1  Cortex-M33 CPU1  (sense domain)       -> target "_core1"
 *    AP 2  Debug mailbox (ISP-AP, same as RT600)
 *    AP 3  APB-AP (HiFi4 / HiFi1 / EZH debug)
 *    AP 4  system bus AHB-AP
 *
 * CPU0:
 *    Same boot ROM problem as on the RT600: the ROM blocks the debug access and
 *    a reset vector catch does not work. The core is therefore halted right
 *    after the ROM with a DWT watchpoint on a register the ROM touches at its
 *    hand-over (NXP: "bootromstall" 0x50002094, SYSCON0 SYSTEM_NSTICK_CALIB).
 *    The procedure is shared with the RT600 and lives in the helper script
 *    (ResetWithBootRomStall).
 *
 * CPU1:
 *    CPU1 is held in reset (RSTCTL3 PRSTCTL0, SYSCON3 CPU_STATUS.CPU_WAIT) after
 *    a power-on reset and does not have a valid vector table. Its AP is not
 *    usable until CPU0 has woken it up. The wake-up sequence (from NXP
 *    RT700_connect.scp) is only executed when CPU1 is the debug target and
 *    when CPU1 is still held in reset (attaching to a running CPU1 does not
 *    change the state of the target):
 *       1. CPU1 expects its vector table at 0x00580000: set the entry address
 *          and a branch-to-self there,
 *       2. unlock the SENSE domain access to the RAM partitions (GLIKEY1 /
 *          AHBSC3),
 *       3. enable the clock of CPU1 and release its reset.
 *    All of that is done through the AHB-AP of CPU0.
 *
 *****************************************************************************/


// Address the boot ROM touches when it is done (SYSCON0 SYSTEM_NSTICK_CALIB,
// secure alias). Same value as "bootromstall" in the NXP device files.
var BOOTROM_STALL_ADDRESS_RT700 = 0x50002094;

// 1: perform the debug mailbox "Start Debug Session" request (from NXP
// LS_preconnect_MIMXRT700.scp) in Connect(), and as a fallback in Reset() of
// CPU0 when the ROM blocks debug access after the reset.
// Needed if the boot ROM blocks the debug access (no valid image in flash,
// ISP mode, erased flash); NXP's LinkServer always does this for the RT700.
// The request is only sent when the debug access of CPU0 is blocked (see
// Connect_RT700 ()), so attaching to a running application is not affected as
// long as the debug access is enabled. Set to 0 to never send it.
// Tested on the RT700-EVK [M], also with an invalid image / blank flash (the
// plain request is not serviced there, the variant with chip reset is, see
// StartDebugSessionVerified ()).
var USE_DEBUG_MAILBOX_RT700 = 1;

// Start of the XSPI0 flash window (secure alias, the AHB-AP 0 accesses are
// secure).
var XSPI0_FLASH_BASE_RT700 = 0x38000000;

// Default vector table of CPU1 after the reset (NXP script; the SVTOR of CPU1
// counts in units of 128 bytes: reset value 0xB000 = 0x00580000) and the
// address of the spin loop which is used as entry point.
var CPU1_VECTOR_TABLE_RT700 = 0x00580000;
var CPU1_SPIN_ADDRESS_RT700 = 0x00580200;

// Registers used to wake CPU1 (all accessed through CPU0).
var SLEEPCON0_RUNCFG_CLR_RT700           = 0x40003030;   // SLEEPCON0 RUNCFG_CLR
var SLEEPCON0_SENSEP_MAINCLK_SHUTOFF     = 0x00000002;   // bit 1: main clock of the sense domain shut off
var CLKCTL3_PSCCTL0_SET_RT700            = 0x40061040;   // CLKCTL3 PSCCTL0_SET
var CLKCTL3_PSCCTL0_CPU1                 = 0x00000001;   // bit 0: clock of CPU1
var RSTCTL3_PRSTCTL0_RT700               = 0x40060010;   // RSTCTL3 PRSTCTL0 (reset state)
var RSTCTL3_PRSTCTL0_CLR_RT700           = 0x40060070;   // RSTCTL3 PRSTCTL0_CLR
var RSTCTL3_PRSTCTL0_CPU1                = 0x80000000;   // bit 31: CPU1 is held in reset
var SYSCON3_CPU_STATUS_RT700             = 0x4006208C;   // SYSCON3 CPU_STATUS (bit 0 CPU_WAIT)
var GLIKEY1_CTRL_0_RT700                 = 0x40220C00;   // GLIKEY1 write enable state machine
var GLIKEY1_CTRL_1_RT700                 = 0x40220C04;
var AHBSC3_SENSE_ARB1RAM_ACCESS_ENABLE   = 0x40220F84;   // AHBSC3 SENSE_ARB1RAM_ACCESS_ENABLE
var AHBSC3_RAM_PARTITIONS_ACCESS_EN      = 0x00000FFF;   // all RAM partitions


// Called from Connect ()
// Only debug interface properties and the debug mailbox (DM-AP) are used
// here, no target memory access.
// Replaces the NXP lines
//    wireswdconnect p%        (SWD line reset / connect  -> CrossWorks engine)
//    selectprobecore p% 0     (select CPU0 = Cortex-M33 AHB-AP index 0)
//    cminitapdp this          (power up DP, init AP      -> CrossWorks engine)
function Connect_RT700 ()
{
	// Use AHB-AP 0 (= Cortex-M33 CPU0) for the memory accesses of the script.
	// This is also needed for the CPU1 target to wake the second core, and it
	// is the AP the engine identifies in the ROM table right after Connect ().
	// CSW bit 30 = SPROT: 0 = "secure" transfers. The Cortex-M33 has TrustZone;
	// without this the debugger would access only the non-secure alias and e.g.
	// 0x5000_2094 (secure alias of SYSCON0) would not work.
	TargetInterface.setDebugInterfaceProperty ("set_adiv5_AHB_ap_num", 0, 0x40000000, 0x00000000);

	// Optional, from NXP LS_preconnect_MIMXRT700.scp.
	// Must happen here and not in Reset(): the engine identifies the core
	// right after Connect (), and with ROM-blocked debug access (erased flash /
	// ISP mode) that fails with "Can't find debug component in ROM table".
	// The DM-AP (AP 2) is reachable at this point.
	// The request is only sent when the debug access of CPU0 is blocked: when it
	// is already enabled (e.g. a session was started before and survived the
	// reset), the request makes CPU0 inaccessible [M: RT700-EVK].
	if (USE_DEBUG_MAILBOX_RT700)
	{
		if (IsDebugAccessEnabled_RT700 () == 0)
		{
			// Plain request first (like NXP); with an invalid image in the flash
			// it is never serviced, then the variant with chip reset is used.
			StartDebugSessionVerified ();
		}
	}
}


// Returns 1 if the memory access of AHB-AP 0 (CPU0) is enabled (CSW.DeviceEn),
// otherwise 0 (blocked by the boot ROM, or the debug port does not respond).
// The debug port is not necessarily powered up yet when Connect () runs: the
// DM-AP ID (always readable) is used to check that the DAP answers, and the
// DAP is re-established otherwise [M: RT700-EVK, reads return 0 until then].
function IsDebugAccessEnabled_RT700 ()
{
	for (var attempt = 0; attempt < 5; attempt++)
	{
		if (TargetInterface.getDebugRegister (DM_ID) == 0x002A0000)
		{
			if ((TargetInterface.getDebugRegister (AHB_AP0_CSW) & AP_CSW_DEVICE_EN) != 0)
				return 1;
			return 0;
		}
		TargetInterface.delay (100);
		TargetInterface.resetDebugInterface ();
	}
	return 0;
}


// Called from GetPartName ()
// This function is also called when attaching to a running application: the
// state of the target is not changed here as long as CPU1 is already released.
function GetPartName_RT700 (DeviceName)
{
	// CPU0 is the target: AHB-AP 0 was already selected in Connect ().
	if (DeviceName.indexOf ("_core1") == -1)
		return;

	// Memory accesses to wake CPU1 go through CPU0
	TargetInterface.setDebugInterfaceProperty ("set_adiv5_AHB_ap_num", 0, 0x40000000, 0x00000000);

	// Note: no "!" on numbers in this script dialect, compare with != 0.
	if ((TargetInterface.peekUint32 (RSTCTL3_PRSTCTL0_RT700) & RSTCTL3_PRSTCTL0_CPU1) != 0)
	{
		Release_RT700_CPU1 ();
	}

	// CPU1 is the target: AHB-AP 1. Clear bit 30 of CSW for secure access.
	TargetInterface.setDebugInterfaceProperty ("set_adiv5_AHB_ap_num", 1, 0x40000000, 0x00000000);
	// Reconnect to detect CPU1 (NXP: wireswdconnect / SelectProbeCore a% 1)
	TargetInterface.resetDebugInterface ();
}


// Called from Reset ()
function Reset_RT700 (DeviceName)
{
	if (DeviceName.indexOf ("_core1") != -1)
	{
		// CPU1 target: only the sense core is reset by the debugger, CPU0 is not touched.
		TargetInterface.stop (HALT_TIMEOUT_MS);
	}
	else
	{
		if (USE_DEBUG_MAILBOX_RT700 != 0 && IsFlashImageMissing_RT700 () != 0)
		{
			// No valid image in flash: the ROM sits in its loop. Every reset (the
			// SYSRESETREQ as well as a chip reset through the debug mailbox) takes
			// the whole debug port down in this state: all APs, including the
			// DM-AP, read 0 and only a new connect revives them [M: RT700-EVK].
			// NXP's RT700_reset.scp does not reset in this case either
			// ("Don't SYSRESET if the image doesn't start with FCFB"). The core
			// stays as it is (halted by the debugger), the debug access is enabled:
			// code can be loaded to RAM or flash.
			TargetInterface.message ("## WARNING - no valid image in flash, the target is not reset");
			TargetInterface.stop (HALT_TIMEOUT_MS);
		}
		else
		{
			// Halt CPU0 after the boot ROM, see ResetWithBootRomStall ()
			ResetWithBootRomStall (BOOTROM_STALL_ADDRESS_RT700, USE_DEBUG_MAILBOX_RT700, 2);
		}

		// The system reset puts CPU1 in reset again. Enable its clock so that the core
		// is visible in the debug port (NXP RT700_reset.scp).
		Enable_RT700_CPU1_Clock ();
	}
}


// Returns 1 if there is no valid image in the flash: the boot ROM has then not
// configured the XSPI0 flash window, which reads 0 (or 0xFFFFFFFF for a
// configured but erased flash) [M: RT700-EVK, blank flash]. The data at the
// start of a valid image (FCB tag or vector table) is neither [A].
// In this state the boot ROM runs its loop / ISP after every reset and takes
// the whole debug port down, see Reset_RT700 ().
function IsFlashImageMissing_RT700 ()
{
	var flash = TargetInterface.peekUint32 (XSPI0_FLASH_BASE_RT700);
	return (flash == 0 || flash == 0xFFFFFFFF) ? 1 : 0;
}

// Enable the clock of CPU1. Must be executed through CPU0.
function Enable_RT700_CPU1_Clock ()
{
	// SLEEPCON0->RUNCFG_CLR = SENSEP_MAINCLK_SHUTOFF
	TargetInterface.pokeUint32 (SLEEPCON0_RUNCFG_CLR_RT700, SLEEPCON0_SENSEP_MAINCLK_SHUTOFF);
	// CLKCTL3->PSCCTL0_SET = CPU1
	TargetInterface.pokeUint32 (CLKCTL3_PSCCTL0_SET_RT700, CLKCTL3_PSCCTL0_CPU1);
}


// Wake up CPU1 and leave it in a spin-loop (NXP RT700_connect.scp).
// Must be executed through CPU0 (AHB-AP 0).
function Release_RT700_CPU1 ()
{
	TargetInterface.message ("## Release CPU1 of the i.MX RT700");

	// By default, CPU1 expects its vector table at 0x00580000
	// Set the entry address in the vector table (Thumb bit set)
	TargetInterface.pokeUint32 (CPU1_VECTOR_TABLE_RT700 + 0x4, CPU1_SPIN_ADDRESS_RT700 | 1);
	// Set a branch to self at the entry address
	TargetInterface.pokeUint32 (CPU1_SPIN_ADDRESS_RT700, 0xE7FEE7FE);

	// GlikeyWriteEnable (GLIKEY1, 9U)
	TargetInterface.pokeUint32 (GLIKEY1_CTRL_0_RT700, 0x00060000);
	TargetInterface.pokeUint32 (GLIKEY1_CTRL_0_RT700, 0x00020009);
	TargetInterface.pokeUint32 (GLIKEY1_CTRL_0_RT700, 0x00010009);
	TargetInterface.pokeUint32 (GLIKEY1_CTRL_1_RT700, 0x00290000);
	TargetInterface.pokeUint32 (GLIKEY1_CTRL_0_RT700, 0x00020009);
	TargetInterface.pokeUint32 (GLIKEY1_CTRL_1_RT700, 0x00280000);
	TargetInterface.pokeUint32 (GLIKEY1_CTRL_0_RT700, 0x00000009);
	// AHBSC3->SENSE_ARB1RAM_ACCESS_ENABLE = RAM_PARTITIONn_ACCESS_EN
	TargetInterface.pokeUint32 (AHBSC3_SENSE_ARB1RAM_ACCESS_ENABLE, AHBSC3_RAM_PARTITIONS_ACCESS_EN);
	// GlikeyClearConfig (GLIKEY1)
	TargetInterface.pokeUint32 (GLIKEY1_CTRL_0_RT700, 0x00040009);

	// Enable the clock of CPU1
	Enable_RT700_CPU1_Clock ();

	// RSTCTL3->PRSTCTL0_CLR = CPU1
	TargetInterface.pokeUint32 (RSTCTL3_PRSTCTL0_CLR_RT700, RSTCTL3_PRSTCTL0_CPU1);
	// SYSCON3->CPU_STATUS = ~CPU_WAIT
	TargetInterface.pokeUint32 (SYSCON3_CPU_STATUS_RT700, 0);

	// Give CPU1 some time to start the spin-loop and its debug port to come up
	TargetInterface.delay (10);
}
