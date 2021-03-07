/*****************************************************************************
 * Original work Copyright (c) 2016 Rowley Associates Limited.               *
 * Modified work Copyright (C) 2021 Markus Klein                             *
 *                                                                           *
 * This file may be distributed under the terms of the License Agreement     *
 * provided with this software.                                              *
 *                                                                           *
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE   *
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. *
 *                                                                           *
 *****************************************************************************/


// Register description of the iMXRT1170
var SRC              = 0x40C04000;
var SRC_SCR          = SRC;
var SRC_SRMR         = SRC + 0x4
var SRC_CTRL_MEGA    = SRC + 0x204;
var SRC_STAT_MEGA    = SRC + 0x210;
var SRC_CTRL_DISPLAY = SRC + 0x224;
var SRC_STAT_DISPLAY = SRC + 0x230;
var SRC_CTRL_M7CORE  = SRC + 0x2A4;
var SRC_STAT_M7CORE  = SRC + 0x2B0;
var SRC_CTRL_M4CORE  = SRC + 0x284;
var SRC_STAT_M4CORE  = SRC + 0x290;

var CCM           = 0x40CC0000;
var CCM_CLOCK_ROOT_M7_CONTROL = CCM + (0 * 0x80) + 0;
var CCM_CLOCK_ROOT_M4_CONTROL = CCM + (1 * 0x80) + 0;
var CCM_LPCG      = CCM + 0x6000;
var CCM_LPCG_LMEM = CCM_LPCG + (27 * 0x20); // Index 27 with offset size of 32

var LMEM       = 0xE0082000;
var LMEM_PSCCR = LMEM + 0x800;

var IOMUXC_LPSR_GPR  = 0x40C0C000;
var IOMUXC_LPSR_GPR0 = IOMUXC_LPSR_GPR + 0x0;
var IOMUXC_LPSR_GPR1 = IOMUXC_LPSR_GPR + 0x4;
var IOMUXC_LPSR_GPR2 = IOMUXC_LPSR_GPR + 0x8;

var IOMUXC_GPR       = 0x400E4000;
var IOMUXC_GPR_GPR0  = IOMUXC_GPR;
var IOMUXC_GPR_GPR1  = IOMUXC_GPR + 0x04;
var IOMUXC_GPR_GPR18 = IOMUXC_GPR + 0x48;
var IOMUXC_GPR_GPR26 = IOMUXC_GPR + 0x68;



// This function is called each time the debugger stops the target
// If the cache is enabled, we invalidate it as the JTAG access is not cache coherent
function OnTargetStop_11xx_M4 ()
{
	var DeviceName = GetProjectPartName ();
	if (DeviceName.slice(-4) == "_cm7")
	{
		TargetInterface.message ("## Target " + DeviceName + " has been caled on the M7 Core");
		return;
	}
	else
		TargetInterface.message ("## Target " + DeviceName + " has been stopped");

	// Invalidate the Data Cache
	var LMEM_PSCCR_ENCACHE_MASK = 0x00000001;
//	var LMEM_PSCCR_ENWRBUF_MASK = 0x00000002;
//	var LMEM_PSCCR_INVW0_MASK   = 0x01000000;
//	var LMEM_PSCCR_INVW1_MASK   = 0x04000000;
	var LMEM_PSCCR_PUSHW0_MASK  = 0x02000000;
	var LMEM_PSCCR_PUSHW1_MASK  = 0x08000000;
	var LMEM_PSCCR_GO_MASK      = 0x80000000;

	// Check if the Cache is clockgated
	var CCM_LPCG_LmemDirect = TargetInterface.peekUint32 (CCM_LPCG_LMEM);
	if (CCM_LPCG_LmemDirect == 0)
	{
		TargetInterface.message ("## OnTargetStop_11xx_M4 - cACHE IS CLOCKGATED");
		return;	// cache is clock gate, so no need to clear it
	}

	// Check if the cache is enabled
	var psccr = TargetInterface.peekUint32 (LMEM_PSCCR);
	psccr = psccr & LMEM_PSCCR_ENCACHE_MASK
	if (psccr == 0)
	{
		TargetInterface.message ("## OnTargetStop_11xx_M4 - cache is disabled");
		return;	// no cache enabled, so no need to clear it
	}

	// Enable the processor system bus to push all modified lines.
	psccr |= LMEM_PSCCR_PUSHW0_MASK | LMEM_PSCCR_PUSHW1_MASK | LMEM_PSCCR_GO_MASK;
	TargetInterface.pokeUint32 (LMEM_PSCCR, psccr);
	do
	{
		psccr = TargetInterface.peekUint32 (LMEM_PSCCR);
	}
	while (pscr & LMEM_PSCCR_GO_MASK  != 0);	// Wait until the cache command completes

	// As a precaution clear the bits to avoid inadvertently re-running this command.
	psccr &= ~(LMEM_PSCCR_PUSHW0_MASK | LMEM_PSCCR_PUSHW1_MASK);
	TargetInterface.pokeUint32 (LMEM_PSCCR, psccr);

	TargetInterface.message ("## Target " + DeviceName + " has been stopped - done");
}

// Don't do any register access in this function.
// Use GetPartName() for this.
function Connect ()
{
	var DeviceName = GetProjectPartName ();
	TargetInterface.message ("## Connect to " + DeviceName);
	switch (DeviceName)
	{
		case "MIMXRT633":
		case "MIMXRT685_cm33":
			break;
		case "MIMXRT1011":
		case "MIMXRT1015":
		case "MIMXRT1021":
		case "MIMXRT1024":
		case "MIMXRT1051":
		case "MIMXRT1052":
		case "MIMXRT1061":
		case "MIMXRT1062":
		case "MIMXRT1064":
			// Do nothing
//			TargetInterface.setNSRST();
			break;
		case "MIMXRT1171_cm7":
		case "MIMXRT1172_cm7":
		case "MIMXRT1173_cm7":
		case "MIMXRT1175_cm7":
		case "MIMXRT1176_cm7":
			TargetInterface.setDebugInterfaceProperty ("set_adiv5_AHB_ap_num", 0);
			break;
		case "MIMXRT1173_cm4":
		case "MIMXRT1175_cm4":
		case "MIMXRT1176_cm4":
			TargetInterface.setDebugInterfaceProperty ("set_adiv5_AHB_ap_num", 1);
			break;
		default:
			TargetInterface.message ("Connect - unknown Device: " + DeviceName);
			break;
	}

	TargetInterface.message ("## Connect to " + DeviceName + " - done");
}

// This function is used to return the controller type as a string
// we use it also to do some initializations as this function is called right before
// writing the code to the controller
function GetPartName ()
{
	var DeviceName = GetProjectPartName ();
	TargetInterface.message ("## get part name of " + DeviceName);

	switch (DeviceName)
	{
		case "MIMXRT633":
		case "MIMXRT685_cm33":
			var reg = TargetInterface.idcode ();
			TargetInterface.message ("Connect - JTAG-ID code: 0x" + reg.toString(16));

			// AP2: Debug Access port for the Debug Mailbox.
			// compare the Reference Manual in Chapter: "48.5.1 Debug subsystem"
			// The Debugger Mailbox Access Port (DM AP) offers a register based mailbox accessible by both CPUs and the device debug port DP of the MCU.
			// This port is always enabled and external world can send and receive data to and from ROM. This port is used to implement NXP Debug Authentication Protocol.
			reg = TargetInterface.getDebugRegister (0x020000FC);
			TargetInterface.message ("Connect - AP2-ID code: 0x" + reg.toString(16));


			TargetInterface.setDebugRegister (0x02000000, 0x21);  // RESYNCH_REQ | CHIP_RESET_REQ
			do
			{
				reg = TargetInterface.getDebugRegister (0x02000000);
				TargetInterface.message ("GetPartName - CSW: 0x" + reg.toString(16));
			}
			while (reg != 0);

			TargetInterface.setDebugRegister (0x02000004, 0x07);	// Write DM START_DBG_SESSION to REQUEST register
			do
			{
				eg = TargetInterface.getDebugRegister (0x02000008);
				TargetInterface.message ("GetPartName - REQUEST: 0x" + reg.toString(16));
			}
			while (reg != 0);
			break;
		case "MIMXRT1011":
		case "MIMXRT1015":
		case "MIMXRT1021":
		case "MIMXRT1024":
		case "MIMXRT1051":
		case "MIMXRT1052":
		case "MIMXRT1061":
		case "MIMXRT1062":
		case "MIMXRT1064":
			// Do nothing
			break;
		case "MIMXRT1171_cm7":
		case "MIMXRT1172_cm7":
		case "MIMXRT1173_cm7":
		case "MIMXRT1175_cm7":
		case "MIMXRT1176_cm7":
			TargetInterface.pokeUint32 (SRC_SCR, 0x1);			// SRC->SCR Enable CM4 -> cm4 core reset is released
			TargetInterface.pokeUint32 (SRC_SRM, 0xF << 10);	// Disable system reset caused by sysrstreq from each core
			break;
		case "MIMXRT1173_cm4":
		case "MIMXRT1175_cm4":
		case "MIMXRT1176_cm4":
			TargetInterface.setDebugRegister (0x02000004, SRC_SCR);  // Transfer Address Register
			TargetInterface.setDebugRegister (0x0200000C, 0x1);      // Data Read/Write Register

			TargetInterface.setDebugRegister (0x02000004, SRC_SRM);  // Transfer Address Register
			TargetInterface.setDebugRegister (0x0200000C, 0x1400);   // Data Read/Write Register
			if (!TargetInterface.isStopped ())
				TargetInterface.stop ();
			break;
		default:
			TargetInterface.message ("GetPartName - unknown Device: " + DeviceName);
			break;
	}

	TargetInterface.message ("## get part name of " + DeviceName + " done");

	var PART = "";
	return PART;
}

function MatchPartName (name)
{
	var partName = GetPartName ();

	if (partName == "")
		return false;

	return partName.substring (0, 6) == name.substring (0, 6);
}

function Reset_11xx_M4 ()
{
	// Issue M4 reset
	TargetInterface.pokeUint32 (SRC_CTRL_M4CORE, 0x1);
	
	var reg = 0;
	do
	{
		reg = TargetInterface.peekUint32 (SRC_STAT_M4CORE);
	}
	while (reg & 0x1);
	TargetInterface.delay (10);

	// Vector reset
	TargetInterface.pokeUint32 (0xE000ED0C, 0x5FA0001);
    TargetInterface.delay (10);
}

function Reset_11xx_M7 ()
{
/*
	var PC = TargetInterface.getRegister ("pc");
	var SP = TargetInterface.getRegister ("sp");
	var XPSR = TargetInterface.getRegister ("xpsr");
	var VTOR = TargetInterface.peekUint32 (0xe000ed08);
	if (PC == 0x00223104)
	{
		TargetInterface.message ("## Reset_11xx_M7 - In Statup-Endless Loop");
		SP = TargetInterface.peekUint32 (0x30002000);
		PC = TargetInterface.peekUint32 (0x30002004);

		TargetInterface.setRegister ("sp", SP);
		TargetInterface.setRegister ("pc", PC);
		TargetInterface.setRegister ("XPSR", 0x01000000);
	}
	else
	{
		TargetInterface.setRegister ("px", 0x00223104);
		TargetInterface.setRegister ("sp", 0x20241000);
	}

	TargetInterface.message ("## Reset_11xx_M7 PC:   0x" + PC.toString(16));
	TargetInterface.message ("## Reset_11xx_M7 SP:   0x" + SP.toString(16));
	TargetInterface.message ("## Reset_11xx_M7 XPSR: 0x" + XPSR.toString(16));
	TargetInterface.message ("## Reset_11xx_M7 VTOR: 0x" + VTOR.toString(16)); /* */

	// ResetTarget
	//// Issue DISPLAYMIX reset ////
	TargetInterface.pokeUint32 (SRC_CTRL_DISPLAY, 1);

	// Check DISPLAYMIX reset status
	var reg = TargetInterface.peekUint32 (SRC_STAT_DISPLAY);
	reg &= 0x1;
	while (reg)
	{
		reg = TargetInterface.peekUint32 (SRC_STAT_DISPLAY);
		reg &= 0x1;
	}
	TargetInterface.delay (10);

	//// Issue M7 reset ////
	TargetInterface.pokeUint32 (SRC_CTRL_M7CORE, 1);

	// Check M7 reset status
	reg = TargetInterface.peekUint32 (SRC_STAT_M7CORE);
	reg &= 0x1;
	while (reg)
	{
		reg = TargetInterface.peekUint32 (SRC_STAT_M7CORE);
		reg &= 0x1;	// mask UNDER_RST Flag
	}
	TargetInterface.delay (10);


	//// Reset Peripherals ////
/*	TargetInterface.pokeUint32 (SRC_CTRL_MEGA, 1);			// Issue MEGA reset SRC->CTRL_MEGA = SW_RESET

	// Check MEGA reset status
	var reg = TargetInterface.peekUint32 (SRC_STAT_MEGA);		// SRC->STAT_MEGA
	reg &= 0x1;												// check UNDER_RST flag
	while (reg)
	{
		reg = TargetInterface.peekUint32 (SRC_STAT_MEGA);
		reg &= 0x1;
	}
	TargetInterface.delay (10);
	TargetInterface.message ("## Reset_11xx_M7 Step 3");/* */
}

function Reset ()
{
	if (TargetInterface.implementation() == "crossworks_simulator")
		return;

	var DeviceName = GetProjectPartName ();
	TargetInterface.message ("## Reset " + DeviceName);
	switch (DeviceName)
	{
		case "MIMXRT633":
		case "MIMXRT685_cm33":
			break;
		case "MIMXRT1011":
		case "MIMXRT1015":
		case "MIMXRT1021":
		case "MIMXRT1024":
		case "MIMXRT1051":
		case "MIMXRT1052":
		case "MIMXRT1061":
		case "MIMXRT1062":
		case "MIMXRT1064":
			TargetInterface.resetAndStop (1000);
			DcDc_Init_10xx ();
			break;
		case "MIMXRT1171_cm7":
		case "MIMXRT1172_cm7":
			TargetInterface.resetAndStop (1000);
			Reset_11xx_M7 ();
			break;
		case "MIMXRT1173_cm7":
		case "MIMXRT1175_cm7":
		case "MIMXRT1176_cm7":
			TargetInterface.resetAndStop (1000);
/*			TargetInterface.pokeUint32 (SRC_CTRL_M4CORE, 2);
			Reset_11xx_M7 ();
			TargetInterface.pokeUint32 (SRC_SCR, 0x1);				// SRC->SCR Enable CM4 -> cm4 core reset is released
			TargetInterface.stop (); */
			break;
		case "MIMXRT1173_cm4":
		case "MIMXRT1175_cm4":
		case "MIMXRT1176_cm4":
			Reset_11xx_M4 ();
			TargetInterface.stop ();
			break;
		default:
			TargetInterface.message ("## Reset - unknown Device: " + DeviceName);
			break;
	}
	TargetInterface.message ("## Reset " + DeviceName + " - done");
}

function DcDc_Init_10xx ()
{
	ocotp_base = 0x401F4000;
	ocotp_fuse_bank0_base = ocotp_base + 0x400;
	dcdc_base = 0x40080000;

	dcdc_trim_loaded = 0;

	reg = TargetInterface.peekUint32 (ocotp_fuse_bank0_base + 0x90);
	if (reg & (1<<10))
	{
		// DCDC: REG0->VBG_TRM
		trim_value = (reg & (0x1F << 11)) >> 11; 
		reg = (TargetInterface.peekUint32 (dcdc_base + 0x4) & ~(0x1F << 24)) | (trim_value << 24);
		TargetInterface.pokeUint32 (dcdc_base + 0x4, reg);
		dcdc_trim_loaded = 1;
	}

	reg = TargetInterface.peekUint32 (ocotp_fuse_bank0_base + 0x80);
	if (reg & (1<<30))
	{
		index = (reg & (3 << 28)) >> 28;
		if (index < 4)
		{
			// DCDC: REG3->TRG 
			reg = (TargetInterface.peekUint32 (dcdc_base + 0xC) & ~(0x1F)) | ((0xF + index));
			TargetInterface.pokeUint32 (dcdc_base + 0xC, reg);
			dcdc_trim_loaded = 1;
		}
	}

	if (dcdc_trim_loaded)
	{
		// delay about 400us till dcdc is stable.
		TargetInterface.delay (1);
	}

	TargetInterface.setRegister ("r0", 1);
	TargetInterface.setRegister ("r1", 1);
}

function EnableTrace (traceInterfaceType)
{
	// TODO: Enable trace
}

function GetProjectPartName ()
{
	var TargetFullName = TargetInterface.getProjectProperty ("Target");
	if (TargetFullName == null)
		return "";

	var TargetShort
	if (TargetFullName.charAt(9) > '9')
		TargetShort = TargetFullName.substring (0, 9);	// Three digits number in the target name
	else
		TargetShort = TargetFullName.substring (0, 10);	// Four digits number in the target name

	switch (TargetFullName.slice(-4))
	{
		case "_cm7":
			TargetShort += '_cm7';
			break;
		case "_cm4":
			TargetShort += '_cm4';
			break;
		case "cm33":
			TargetShort += '_cm33';
			break;
		default:
			// Do nothing
	}

	return TargetShort;
}

function Clock_Init ()
{
	var DeviceName = GetProjectPartName ();
	TargetInterface.message ("Clock initialize for " + DeviceName);

	switch (DeviceName)
	{
		case "MIMXRT1011":
		case "MIMXRT1015":
		case "MIMXRT1021":
		case "MIMXRT1024":
			Clock_Init_1021 ();
			break;
		case "MIMXRT1051":
		case "MIMXRT1052":
		case "MIMXRT1061":
		case "MIMXRT1062":
		case "MIMXRT1064":
			Clock_Init_105x ();
			break;
		case "MIMXRT1171_cm7":
		case "MIMXRT1172_cm7":
		case "MIMXRT1173_cm7":
		case "MIMXRT1175_cm7":
		case "MIMXRT1176_cm7":
			Clock_Init_117x ();
			break;
		default:
			TargetInterface.message ("Clock_Init - unknown Device: " + DeviceName);
			break;
	}
}

function ClockGate_EnableAll_10xx ()
{
	// Enable all clocks
	TargetInterface.pokeUint32 (0x400FC068, 0xffffffff);	// CCM->CCGR0
	TargetInterface.pokeUint32 (0x400FC06C, 0xffffffff);	// CCM->CCGR1
	TargetInterface.pokeUint32 (0x400FC070, 0xffffffff);	// CCM->CCGR2
	TargetInterface.pokeUint32 (0x400FC074, 0xffffffff);	// CCM->CCGR3
	TargetInterface.pokeUint32 (0x400FC078, 0xffffffff);	// CCM->CCGR4
	TargetInterface.pokeUint32 (0x400FC07C, 0xffffffff);	// CCM->CCGR5
	TargetInterface.pokeUint32 (0x400FC080, 0xffffffff);	// CCM->CCGR6
}

function DisableMPU ()
{
	TargetInterface.message ("### DisableMPU");

	var MPU = 0xE000E000 + 0x0D90;	// SCS_BASE + MPU offset
	// Disable MPU which will be enabled by ROM to prevent code execution
	var reg = TargetInterface.peekUint32 (MPU + 0x04);		// MPU->CTRL
	reg &= ~0x1;											// Disable Enable Flag
	TargetInterface.pokeUint32 (MPU + 0x04, reg);
}

function Clock_Init_1021 () 
{
	TargetInterface.message ("Clock_Init_1021");
	ClockGate_EnableAll_10xx ();	// Enable all clocks

	// IPG_PODF: 2 divide by 3
	TargetInterface.pokeUint32 (0x400FC014,0x000A8200);		// CCM->CBCDR
	// PERCLK_PODF: 1 divide by 2
	TargetInterface.pokeUint32 (0x400FC01C, 0x04900001);	// CCM->CSCMR1 
	// Enable SYS PLL but keep it bypassed.
	TargetInterface.pokeUint32 (0x400D8030, 0x00012001);	// CCM_ANALOG->PLL_SYS 
	var reg;
	do
	{
		reg = TargetInterface.peekUint32 (0x400D8030);		// CCM_ANALOG->PLL_SYS 
	}
	while ((reg & 0x80000000) == 0);

	// Disable bypass of SYS PLL
	TargetInterface.pokeUint32 (0x400D8030, 0x00002001);	// CCM_ANALOG->PLL_SYS 

	// Ungate SYS PLL PFD2
	reg = TargetInterface.peekUint32 (0x400D8100);			// CCM_ANALOG->PFD_528 
	reg &= ~0x00800000;										// CCM_ANALOG_PFD_528_PFD2_CLKGATE_MASK
	TargetInterface.pokeUint32 (0x400D8100, reg);			// CCM_ANALOG->PFD_528

	// SEMC_ALT_CLK_SEL: 0 PLL2 (SYS PLL) PFD2
	// SEMC_CLK_SEL: 1 SEMC_ALT_CLK
	// SEMC_PODF: 2 divide by 3
	reg = TargetInterface.peekUint32 (0x400FC014);			// CCM->CBCDR
	reg |= 0x20040;
	TargetInterface.pokeUint32 (0x400FC014, reg);			// CCM->CBCDR

	// Disable MPU which will be enabled by ROM to prevent code execution
	DisableMPU ();
	TargetInterface.message ("Clock_Init_1021 - Done");
}

function Clock_Init_105x () 
{
	TargetInterface.message ("Clock_Init_105x");
	ClockGate_EnableAll_10xx ();	// Enable all clocks

	// PERCLK_PODF: 1 divide by 2
	TargetInterface.pokeUint32 (0x400FC01C, 0x04900001);	// CCM->CSCMR1 
	// Enable SYS PLL but keep it bypassed.
	TargetInterface.pokeUint32 (0x400D8030, 0x00012001);	// CCM_ANALOG->PLL_SYS 
	var reg;
	do
	{
		reg = TargetInterface.peekUint32 (0x400D8030);		// CCM_ANALOG->PLL_SYS 
	}
	while ((reg & 0x80000000) == 0);
	// Disable bypass of SYS PLL
	TargetInterface.pokeUint32 (0x400D8030, 0x00002001);	// CCM_ANALOG->PLL_SYS 

	// PFD2_FRAC: 29, PLL2 PFD2=528*18/PFD2_FRAC=327
	// Ungate SYS PLL PFD2
	TargetInterface.pokeUint32 (0x400D8100, 0x001d0000);	// CCM_ANALOG->PFD_528

	// SEMC_PODF: 001, AHB_PODF: 011, IPG_PODF: 01
	// SEMC_ALT_CLK_SEL: 0 PLL2 (SYS PLL) PFD2
	// SEMC_CLK_SEL: 1 SEMC_ALT_CLK
	TargetInterface.pokeUint32 (0x400FC014, 0x00010D40);	// CCM->CBCDR
	TargetInterface.message ("Clock_Init_105x - Done");
}

function Clock_Init_117x () 
{
	////////// Enable PLL LDO //////////
	// ANADIG_MISC_VDDSOC_AI_CTRL
	var val = TargetInterface.peekUint32 (0x40C84820);  // ANADIG_MISC->VDDSOC_AI_CTRL
	val &= ~(0x10000 | 0xFF); 
	TargetInterface.pokeUint32 (0x40C84820, val);

	// ANADIG_MISC_VDDSOC_AI_WDATA 
	TargetInterface.pokeUint32 (0x40C84830, 0x105);     // ANADIG_MISC->VDDSOC_AI_WDATA

	// ANADIG_PMU_PMU_LDO_PLL
	val = TargetInterface.peekUint32 (0x40C84500);      // ANADIG_PMU->PMU_LDO_PLL
	val ^= 0x10000;
	TargetInterface.pokeUint32 (0x40C84500, val);


	////////// Initialize System PLL2 //////////
	// ANADIG_PLL_PLL_528_CTRL
	val = TargetInterface.peekUint32 (0x40C84240);      // ANADIG_PLL->PLL_528_CTRL
	if (val & 0x800000)
	{
		// SysPll2 has been initialized
		val &= ~0x40000000;
		TargetInterface.pokeUint32 (0x40C84240, val);   // ANADIG_PLL->PLL_528_CTRL
		TargetInterface.message ("syspll2 has been initialized already");
		return;
	}

	val = TargetInterface.peekUint32 (0x40C84270);      // ANADIG_PLL->PLL_528_PFD
	val |= 0x80808080;
	TargetInterface.pokeUint32 (0x40C84270, val);

	val = TargetInterface.peekUint32 (0x40C84240);      // ANADIG_PLL->PPLL_528_CTRL
	val &= ~(0x802000);
	val |= 0x40000000;
	TargetInterface.pokeUint32 (0x40C84240, val);       // ANADIG_PLL->PLL_528_CTRL

	TargetInterface.pokeUint32 (0x40C84280, 0);         // ANADIG_PLL->PLL_528_MFN
	TargetInterface.pokeUint32 (0x40C84290, 22);        // ANADIG_PLL->PLL_528_MFI
	TargetInterface.pokeUint32 (0x40C842A0, 0x0FFFFFFF);// ANADIG_PLL->PLL_528_MFD

	// ANADIG_PLL_PLL_528_CTRL
	TargetInterface.pokeUint32 (0x40C84240, 0x8 | 0x40000000);
	TargetInterface.delay (30);

	// ANADIG_PLL_PLL_528_CTRL
	val = TargetInterface.peekUint32 (0x40C84240);      // ANADIG_PLL->PLL_528_CTRL
	val |= 0x800000 | 0x800;
	TargetInterface.pokeUint32 (0x40C84240, val);       // ANADIG_PLL->PLL_528_CTRL
	TargetInterface.delay (250);

	val = TargetInterface.peekUint32 (0x40C84240);      // ANADIG_PLL->PLL_528_CTRL
	val &= ~0x800;
	TargetInterface.pokeUint32 (0x40C84240, val);       // ANADIG_PLL->PLL_528_CTRL

	do
	{
		val = TargetInterface.peekUint32 (0x40C84240);  // ANADIG_PLL->PLL_528_CTRL
	} while ((val & 0x20000000) == 0);

	val |= 0x2000;
	TargetInterface.pokeUint32 (0x40C84240, val);       // ANADIG_PLL->PLL_528_CTRL

	val &= ~0x40000000;
	TargetInterface.pokeUint32 (0x40C84240, val);       // ANADIG_PLL->PLL_528_CTRL


	////////// Initialize System PLL2 PFD1 //////////
	val = TargetInterface.peekUint32 (0x40C84270);      // ANADIG_PLL->PLL_528_PFD
	if (((val & 0x8000) != 0) || (((val & 0x3F00) >> 8) != 16))
	{
		var stable = val & 0x4000;

		val |= 0x8000;
		TargetInterface.pokeUint32 (0x40C84270, val);   // ANADIG_PLL->PLL_528_PFD

		val = TargetInterface.peekUint32 (0x40C84270);  // ANADIG_PLL->PLL_528_PFD
		val &= ~0x3F00;
		val |= 16 << 8;
		TargetInterface.pokeUint32 (0x40C84270, val);   // ANADIG_PLL->PLL_528_PFD

		val = TargetInterface.peekUint32 (0x40C84250);  // ANADIG_PLL->PLL_528_UPDATE
		val ^= 0x4;
		TargetInterface.pokeUint32 (0x40C84250, val);   // ANADIG_PLL->PLL_528_UPDATE

		val = TargetInterface.peekUint32 (0x40C84270);  // ANADIG_PLL->PLL_528_PFD
		val &= ~0x8000;
		TargetInterface.pokeUint32 (0x40C84270, val);   // ANADIG_PLL->PLL_528_PFD
		do
		{
			val = TargetInterface.peekUint32 (0x40C84270) & 0x4000;      // ANADIG_PLL->PLL_528_PFD
		} while (val == stable);
	}
	else
	{
		TargetInterface.message ("syspll2 pfd1 has been initialized already");
		val &= ~0x8000;
		TargetInterface.pokeUint32 (0x40C84270, val);   // ANADIG_PLL->PLL_528_PFD
	}


	////////// Set SEMC root clock //////////
	var reg = 0x40CC0200;
	// Use sys pll2 pfd1 divided by 3: 198Mhz
	val = 0x602;
	TargetInterface.pokeUint32 (reg, val);
}

function Clock_Restore_117x () 
{
	var reg = 0x40CC0000;	// CCM
	var i = 0;
	while (i < 80)
	{
		TargetInterface.pokeUint32 (reg + (i * 0x80), 0);	// Set CCM->CLOCK_ROOT[0 .. 79].CONTROL = 0
		i++;
	}
}


function SDRAM_WaitIpCmdDone (SEMC) 
{
	var reg;
	do
	{
		reg = TargetInterface.peekUint32 (SEMC + 0x3C);	// INTR - Interrupt Enable Register
	}
	while ((reg & 0x3) == 0);

	TargetInterface.pokeUint32 (SEMC + 0x3C,0x00000003); // clear IPCMDERR and IPCMDDONE bits
	TargetInterface.message ("SDRAM_WaitIpCmdDone");
}

function SDRAM_Init ()
{
	var DeviceName = GetProjectPartName ();
	TargetInterface.message ("SDRAM interface initialize for " + DeviceName);
	switch (DeviceName)
	{
		case "MIMXRT1011":
		case "MIMXRT1015":
			TargetInterface.message ("SDRAM_Init: " + DeviceName - " has no memory interface");
			break;
		case "MIMXRT1021":
		case "MIMXRT1024":
		case "MIMXRT1051":
		case "MIMXRT1052":
		case "MIMXRT1061":
		case "MIMXRT1062":
		case "MIMXRT1064":
			SDRAM_Init_10xx ();
			break;
		case "MIMXRT1171_cm7":
		case "MIMXRT1172_cm7":
		case "MIMXRT1173_cm7":
		case "MIMXRT1175_cm7":
		case "MIMXRT1176_cm7":
			SDRAM_Init_1170 ();
			break;
		default:
			TargetInterface.message ("SDRAM_Init - unknown Device: " + DeviceName);
			return;
	}

	TargetInterface.message ("SDRAM_Init - Done");
}

function SDRAM_Init_10xx ()
{
	var DeviceName = GetProjectPartName ();

	// Configure IOMUX for SDRAM
	var IOMUXC                = 0x401F8000;
	var IOMUXC_SW_MUX_CTL_PAD = IOMUXC;
	var IOMUXC_SW_PAD_CTL_PAD = IOMUXC;
	var PAD_Settings          = 0;
	if (DeviceName == "MIMXRT1021" || DeviceName == "MIMXRT1024")
	{
		var IOMUXC_SW_MUX_CTL_PAD = IOMUXC + 0x14;
		var IOMUXC_SW_PAD_CTL_PAD = IOMUXC + 0x188;
		var PAD_Settings          = 0x000000F1;
	}
	else
	{
		var IOMUXC_SW_MUX_CTL_PAD = IOMUXC + 0x14;
		var IOMUXC_SW_PAD_CTL_PAD = IOMUXC + 0x204;
		var PAD_Settings          = 0x000110F9;
	}
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x00, 0x00000000); // EMC_00
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x04, 0x00000000); // EMC_01
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x08, 0x00000000); // EMC_02
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x0C, 0x00000000); // EMC_03
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x10, 0x00000000); // EMC_04
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x14, 0x00000000); // EMC_05
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x18, 0x00000000); // EMC_06
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x1C, 0x00000000); // EMC_07
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x20, 0x00000000); // EMC_08
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x24, 0x00000000); // EMC_09
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x28, 0x00000000); // EMC_10
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x2C, 0x00000000); // EMC_11
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x30, 0x00000000); // EMC_12
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x34, 0x00000000); // EMC_13
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x38, 0x00000000); // EMC_14
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x3C, 0x00000000); // EMC_15
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x40, 0x00000000); // EMC_16
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x44, 0x00000000); // EMC_17
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x48, 0x00000000); // EMC_18
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x4C, 0x00000000); // EMC_19
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x50, 0x00000000); // EMC_20
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x54, 0x00000000); // EMC_21
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x58, 0x00000000); // EMC_22
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x5C, 0x00000000); // EMC_23
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x60, 0x00000000); // EMC_24
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x64, 0x00000000); // EMC_25
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x68, 0x00000000); // EMC_26
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x6C, 0x00000000); // EMC_27
	if (DeviceName == "MIMXRT1021" || DeviceName == "MIMXRT1024")
		TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x70, 0x00000010); // EMC_28, DQS PIN, enable SION
	else
		TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x70, 0x00000000); // EMC_28
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x74, 0x00000000); // EMC_29
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x78, 0x00000000); // EMC_30
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x7C, 0x00000000); // EMC_31
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x80, 0x00000000); // EMC_32
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x84, 0x00000000); // EMC_33
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x88, 0x00000000); // EMC_34
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x8C, 0x00000000); // EMC_35
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x90, 0x00000000); // EMC_36
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x94, 0x00000000); // EMC_37
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x98, 0x00000000); // EMC_38
	if (DeviceName == "MIMXRT1021" || DeviceName == "MIMXRT1024")
		TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x9C, 0x00000000); // EMC_39, DQS PIN, enable SION
	else
		TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x9C, 0x00000010); // EMC_39
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0xA0, 0x00000000); // EMC_40
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0xA4, 0x00000000); // EMC_41

	// PAD ctrl
	// drive strength = 0x7 to increase drive strength
	// otherwise the data7 bit may fail.
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x00, PAD_Settings); // EMC_00
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x04, PAD_Settings); // EMC_01
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x08, PAD_Settings); // EMC_02
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x0C, PAD_Settings); // EMC_03
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x10, PAD_Settings); // EMC_04
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x14, PAD_Settings); // EMC_05
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x18, PAD_Settings); // EMC_06
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x1C, PAD_Settings); // EMC_07
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x20, PAD_Settings); // EMC_08
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x24, PAD_Settings); // EMC_09
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x28, PAD_Settings); // EMC_10
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x2C, PAD_Settings); // EMC_11
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x30, PAD_Settings); // EMC_12
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x34, PAD_Settings); // EMC_13
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x38, PAD_Settings); // EMC_14
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x3C, PAD_Settings); // EMC_15
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x40, PAD_Settings); // EMC_16
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x44, PAD_Settings); // EMC_17
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x48, PAD_Settings); // EMC_18
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x4C, PAD_Settings); // EMC_19
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x50, PAD_Settings); // EMC_20
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x54, PAD_Settings); // EMC_21
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x58, PAD_Settings); // EMC_22
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x5C, PAD_Settings); // EMC_23
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x60, PAD_Settings); // EMC_24
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x64, PAD_Settings); // EMC_25
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x68, PAD_Settings); // EMC_26
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x6C, PAD_Settings); // EMC_27
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x70, PAD_Settings); // EMC_28
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x74, PAD_Settings); // EMC_29
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x78, PAD_Settings); // EMC_30
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x7C, PAD_Settings); // EMC_31
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x80, PAD_Settings); // EMC_32
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x84, PAD_Settings); // EMC_33
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x88, PAD_Settings); // EMC_34
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x8C, PAD_Settings); // EMC_35
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x90, PAD_Settings); // EMC_36
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x94, PAD_Settings); // EMC_37
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x98, PAD_Settings); // EMC_38
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x9C, PAD_Settings); // EMC_39
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0xA0, PAD_Settings); // EMC_40
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0xA4, PAD_Settings); // EMC_41

	// Configure SDR Controller Registers
	var SEMC = 0x402F0000;
	TargetInterface.message ("SDR Controller Registers");
	TargetInterface.pokeUint32 (SEMC + 0x00, 0x10000004); // MCR
	TargetInterface.pokeUint32 (SEMC + 0x08, 0x00030524); // BMCR0
	TargetInterface.pokeUint32 (SEMC + 0x0C, 0x06030524); // BMCR1
	TargetInterface.pokeUint32 (SEMC + 0x10, 0x8000001B); // BR0 - Base Register 0 (For SDRAM CS0 device), 32MB --> 0x8000001D for 64MB
	TargetInterface.pokeUint32 (SEMC + 0x14, 0x8200001B); // BR1, 32MB
	TargetInterface.pokeUint32 (SEMC + 0x18, 0x8400001B); // BR2, 32MB
	TargetInterface.pokeUint32 (SEMC + 0x1C, 0x8600001B); // BR3, 32MB
	TargetInterface.pokeUint32 (SEMC + 0x20, 0x90000021); // BR4,
	TargetInterface.pokeUint32 (SEMC + 0x24, 0xA0000019); // BR5,
	TargetInterface.pokeUint32 (SEMC + 0x28, 0xA8000017); // BR6,
	TargetInterface.pokeUint32 (SEMC + 0x2C, 0xA900001B); // BR7,
	TargetInterface.pokeUint32 (SEMC + 0x30, 0x00000021); // BR8,
	TargetInterface.pokeUint32 (SEMC + 0x04, 0x000079A8); //IOCR,SEMC_CCSX0 as NOR CE, SEMC_CSX1 as PSRAM CE, SEMC_CSX2 as NAND CE, SEMC_CSX3 as DBI CE.

	TargetInterface.pokeUint32 (SEMC + 0x40, 0x00000F31); // SDRAMCR0
	TargetInterface.pokeUint32 (SEMC + 0x44, 0x00652922); // SDRAMCR1
	TargetInterface.pokeUint32 (SEMC + 0x48, 0x00010920); // SDRAMCR2
	TargetInterface.pokeUint32 (SEMC + 0x4C, 0x50210A09); // SDRAMCR3

	TargetInterface.pokeUint32 (SEMC + 0x40, 0x00000F31); // SDRAMCR0
	TargetInterface.pokeUint32 (SEMC + 0x44, 0x00652922); // SDRAMCR1
	TargetInterface.pokeUint32 (SEMC + 0x48, 0x00010920); // SDRAMCR2
	TargetInterface.pokeUint32 (SEMC + 0x4C, 0x50210A09); // SDRAMCR3

	TargetInterface.pokeUint32 (SEMC + 0x80, 0x00000021); // DBICR0
	TargetInterface.pokeUint32 (SEMC + 0x84, 0x00888888); // DBICR1
	TargetInterface.pokeUint32 (SEMC + 0x94, 0x00000002); // IPCR1
	TargetInterface.pokeUint32 (SEMC + 0x98, 0x00000000); // IPCR2

	TargetInterface.pokeUint32 (SEMC + 0x90, 0x80000000); // IPCR0
	TargetInterface.pokeUint32 (SEMC + 0x9C, 0xA55A000F); // IPCMD, SD_CC_IPREA
	SDRAM_WaitIpCmdDone        (SEMC);
	TargetInterface.pokeUint32 (SEMC + 0x90, 0x80000000); // IPCR0
	TargetInterface.pokeUint32 (SEMC + 0x9C, 0xA55A000C); // SD_CC_IAF
	SDRAM_WaitIpCmdDone        (SEMC);
	TargetInterface.pokeUint32 (SEMC + 0x90, 0x80000000); // IPCR0
	TargetInterface.pokeUint32 (SEMC + 0x9C, 0xA55A000C); // SD_CC_IAF
	SDRAM_WaitIpCmdDone        (SEMC);
	TargetInterface.pokeUint32 (SEMC + 0xA0, 0x00000033); // IPTXDAT
	TargetInterface.pokeUint32 (SEMC + 0x90, 0x80000000); // IPCR0
	TargetInterface.pokeUint32 (SEMC + 0x9C, 0xA55A000A); // SD_CC_IMS
	SDRAM_WaitIpCmdDone        (SEMC);
	TargetInterface.pokeUint32 (SEMC + 0x4C, 0x50210A09); // enable SDRAM self refresh again after initialization done.
}


function SDRAM_Init_1170 ()
{
	var IOMUXC                = 0x400E8000;
	var IOMUXC_SW_MUX_CTL_PAD = IOMUXC + 0x10;
	var IOMUXC_SW_PAD_CTL_PAD = IOMUXC + 0x254;

	// Configure the IOMUX for the SDRAM interface. First set the Mux configuration
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x000, 0x00000000); // IOMUXC_GPIO_EMC_B1_00_SEMC_DATA00
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x004, 0x00000000); // IOMUXC_GPIO_EMC_B1_01_SEMC_DATA01
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x008, 0x00000000); // IOMUXC_GPIO_EMC_B1_02_SEMC_DATA02
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x00C, 0x00000000); // IOMUXC_GPIO_EMC_B1_03_SEMC_DATA03
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x010, 0x00000000); // IOMUXC_GPIO_EMC_B1_04_SEMC_DATA04
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x014, 0x00000000); // IOMUXC_GPIO_EMC_B1_05_SEMC_DATA05
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x018, 0x00000000); // IOMUXC_GPIO_EMC_B1_06_SEMC_DATA06
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x01C, 0x00000000); // IOMUXC_GPIO_EMC_B1_07_SEMC_DATA07
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x020, 0x00000000); // IOMUXC_GPIO_EMC_B1_08_SEMC_DM00
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x024, 0x00000000); // IOMUXC_GPIO_EMC_B1_09_SEMC_ADDR00
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x028, 0x00000000); // IOMUXC_GPIO_EMC_B1_10_SEMC_ADDR01
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x02C, 0x00000000); // IOMUXC_GPIO_EMC_B1_11_SEMC_ADDR02
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x030, 0x00000000); // IOMUXC_GPIO_EMC_B1_12_SEMC_ADDR03
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x034, 0x00000000); // IOMUXC_GPIO_EMC_B1_13_SEMC_ADDR04
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x038, 0x00000000); // IOMUXC_GPIO_EMC_B1_14_SEMC_ADDR05
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x03C, 0x00000000); // IOMUXC_GPIO_EMC_B1_15_SEMC_ADDR06
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x040, 0x00000000); // IOMUXC_GPIO_EMC_B1_16_SEMC_ADDR07
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x044, 0x00000000); // IOMUXC_GPIO_EMC_B1_17_SEMC_ADDR08
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x048, 0x00000000); // IOMUXC_GPIO_EMC_B1_18_SEMC_ADDR09
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x04C, 0x00000000); // IOMUXC_GPIO_EMC_B1_19_SEMC_ADDR11
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x050, 0x00000000); // IOMUXC_GPIO_EMC_B1_20_SEMC_ADDR12
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x054, 0x00000000); // IOMUXC_GPIO_EMC_B1_21_SEMC_BA0
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x058, 0x00000000); // IOMUXC_GPIO_EMC_B1_22_SEMC_BA1
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x05C, 0x00000000); // IOMUXC_GPIO_EMC_B1_23_SEMC_ADDR10
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x060, 0x00000000); // IOMUXC_GPIO_EMC_B1_24_SEMC_CAS
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x064, 0x00000000); // IOMUXC_GPIO_EMC_B1_25_SEMC_RAS
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x068, 0x00000000); // IOMUXC_GPIO_EMC_B1_26_SEMC_CLK
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x06C, 0x00000000); // IOMUXC_GPIO_EMC_B1_27_SEMC_CKE
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x070, 0x00000000); // IOMUXC_GPIO_EMC_B1_28_SEMC_WE
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x074, 0x00000000); // IOMUXC_GPIO_EMC_B1_29_SEMC_CS0
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x078, 0x00000000); // IOMUXC_GPIO_EMC_B1_30_SEMC_DATA08
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x07C, 0x00000000); // IOMUXC_GPIO_EMC_B1_31_SEMC_DATA09
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x080, 0x00000000); // IOMUXC_GPIO_EMC_B1_32_SEMC_DATA10
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x084, 0x00000000); // IOMUXC_GPIO_EMC_B1_33_SEMC_DATA11
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x088, 0x00000000); // IOMUXC_GPIO_EMC_B1_34_SEMC_DATA12
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x08C, 0x00000000); // IOMUXC_GPIO_EMC_B1_35_SEMC_DATA13
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x090, 0x00000000); // IOMUXC_GPIO_EMC_B1_36_SEMC_DATA14
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x094, 0x00000000); // IOMUXC_GPIO_EMC_B1_37_SEMC_DATA15
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x098, 0x00000000); // IOMUXC_GPIO_EMC_B1_38_SEMC_DM01
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x09C, 0x00000010); // IOMUXC_GPIO_EMC_B1_39_SEMC_DQS	-	EMC_39, DQS PIN, enable SION
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x0A0, 0x00000000); // IOMUXC_GPIO_EMC_B1_40_SEMC_RDY
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x0A4, 0x00000000); // IOMUXC_GPIO_EMC_B1_41_SEMC_CSX00
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x0A8, 0x00000000); // IOMUXC_GPIO_EMC_B2_00_SEMC_DATA16
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x0AC, 0x00000000); // IOMUXC_GPIO_EMC_B2_01_SEMC_DATA17
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x0B0, 0x00000000); // IOMUXC_GPIO_EMC_B2_02_SEMC_DATA18
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x0B4, 0x00000000); // IOMUXC_GPIO_EMC_B2_03_SEMC_DATA19
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x0B8, 0x00000000); // IOMUXC_GPIO_EMC_B2_04_SEMC_DATA20
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x0BC, 0x00000000); // IOMUXC_GPIO_EMC_B2_05_SEMC_DATA21
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x0C0, 0x00000000); // IOMUXC_GPIO_EMC_B2_06_SEMC_DATA22
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x0C4, 0x00000000); // IOMUXC_GPIO_EMC_B2_07_SEMC_DATA23
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x0C8, 0x00000000); // IOMUXC_GPIO_EMC_B2_08_SEMC_DM02
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x0CC, 0x00000000); // IOMUXC_GPIO_EMC_B2_09_SEMC_DATA24
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x0D0, 0x00000000); // IOMUXC_GPIO_EMC_B2_10_SEMC_DATA25
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x0D4, 0x00000000); // IOMUXC_GPIO_EMC_B2_11_SEMC_DATA26
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x0D8, 0x00000000); // IOMUXC_GPIO_EMC_B2_12_SEMC_DATA27
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x0DC, 0x00000000); // IOMUXC_GPIO_EMC_B2_13_SEMC_DATA28
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x0E0, 0x00000000); // IOMUXC_GPIO_EMC_B2_14_SEMC_DATA29
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x0E4, 0x00000000); // IOMUXC_GPIO_EMC_B2_15_SEMC_DATA30
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x0E8, 0x00000000); // IOMUXC_GPIO_EMC_B2_16_SEMC_DATA31
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x0EC, 0x00000000); // IOMUXC_GPIO_EMC_B2_17_SEMC_DM03
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x0F0, 0x00000000); // IOMUXC_GPIO_EMC_B2_18_SEMC_DQS4
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x0F4, 0x00000000); // IOMUXC_GPIO_EMC_B2_19_SEMC_CLKX00
	TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x0F8, 0x00000000); // IOMUXC_GPIO_EMC_B2_20_SEMC_CLKX01

	// now set the PAD configuration
	// PDRV = 1b (normal); PULL = 10b (PD)
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x000, 0x00000008); // IOMUXC_GPIO_EMC_B1_00_SEMC_DATA00
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x004, 0x00000008); // IOMUXC_GPIO_EMC_B1_01_SEMC_DATA01
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x008, 0x00000008); // IOMUXC_GPIO_EMC_B1_02_SEMC_DATA02
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x00c, 0x00000008); // IOMUXC_GPIO_EMC_B1_03_SEMC_DATA03
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x010, 0x00000008); // IOMUXC_GPIO_EMC_B1_04_SEMC_DATA04
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x014, 0x00000008); // IOMUXC_GPIO_EMC_B1_05_SEMC_DATA05
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x018, 0x00000008); // IOMUXC_GPIO_EMC_B1_06_SEMC_DATA06
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x01c, 0x00000008); // IOMUXC_GPIO_EMC_B1_07_SEMC_DATA07
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x020, 0x00000008); // IOMUXC_GPIO_EMC_B1_08_SEMC_DM00
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x024, 0x00000008); // IOMUXC_GPIO_EMC_B1_09_SEMC_ADDR00
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x028, 0x00000008); // IOMUXC_GPIO_EMC_B1_10_SEMC_ADDR01
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x02c, 0x00000008); // IOMUXC_GPIO_EMC_B1_11_SEMC_ADDR02
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x030, 0x00000008); // IOMUXC_GPIO_EMC_B1_12_SEMC_ADDR03
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x034, 0x00000008); // IOMUXC_GPIO_EMC_B1_13_SEMC_ADDR04
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x038, 0x00000008); // IOMUXC_GPIO_EMC_B1_14_SEMC_ADDR05
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x03c, 0x00000008); // IOMUXC_GPIO_EMC_B1_15_SEMC_ADDR06
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x040, 0x00000008); // IOMUXC_GPIO_EMC_B1_16_SEMC_ADDR07
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x044, 0x00000008); // IOMUXC_GPIO_EMC_B1_17_SEMC_ADDR08
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x048, 0x00000008); // IOMUXC_GPIO_EMC_B1_18_SEMC_ADDR09
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x04c, 0x00000008); // IOMUXC_GPIO_EMC_B1_19_SEMC_ADDR11
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x050, 0x00000008); // IOMUXC_GPIO_EMC_B1_20_SEMC_ADDR12
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x054, 0x00000008); // IOMUXC_GPIO_EMC_B1_21_SEMC_BA0
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x058, 0x00000008); // IOMUXC_GPIO_EMC_B1_22_SEMC_BA1
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x05c, 0x00000008); // IOMUXC_GPIO_EMC_B1_23_SEMC_ADDR10
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x060, 0x00000008); // IOMUXC_GPIO_EMC_B1_24_SEMC_CAS
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x064, 0x00000008); // IOMUXC_GPIO_EMC_B1_25_SEMC_RAS
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x068, 0x00000008); // IOMUXC_GPIO_EMC_B1_26_SEMC_CLK
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x06c, 0x00000008); // IOMUXC_GPIO_EMC_B1_27_SEMC_CKE
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x070, 0x00000008); // IOMUXC_GPIO_EMC_B1_28_SEMC_WE
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x074, 0x00000008); // IOMUXC_GPIO_EMC_B1_29_SEMC_CS0
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x078, 0x00000008); // IOMUXC_GPIO_EMC_B1_30_SEMC_DATA08
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x07c, 0x00000008); // IOMUXC_GPIO_EMC_B1_31_SEMC_DATA09
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x080, 0x00000008); // IOMUXC_GPIO_EMC_B1_32_SEMC_DATA10
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x084, 0x00000008); // IOMUXC_GPIO_EMC_B1_33_SEMC_DATA11
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x088, 0x00000008); // IOMUXC_GPIO_EMC_B1_34_SEMC_DATA12
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x08c, 0x00000008); // IOMUXC_GPIO_EMC_B1_35_SEMC_DATA13
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x090, 0x00000008); // IOMUXC_GPIO_EMC_B1_36_SEMC_DATA14
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x094, 0x00000008); // IOMUXC_GPIO_EMC_B1_37_SEMC_DATA15
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x098, 0x00000008); // IOMUXC_GPIO_EMC_B1_38_SEMC_DM01
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x09c, 0x00000008); // IOMUXC_GPIO_EMC_B1_39_SEMC_DQS	-	EMC_39, DQS PIN, enable SION
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x0a0, 0x00000008); // IOMUXC_GPIO_EMC_B1_40_SEMC_RDY
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x0a4, 0x00000008); // IOMUXC_GPIO_EMC_B1_41_SEMC_CSX00
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x0a8, 0x00000008); // IOMUXC_GPIO_EMC_B2_00_SEMC_DATA16
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x0ac, 0x00000008); // IOMUXC_GPIO_EMC_B2_01_SEMC_DATA17
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x0b0, 0x00000008); // IOMUXC_GPIO_EMC_B2_02_SEMC_DATA18
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x0b4, 0x00000008); // IOMUXC_GPIO_EMC_B2_03_SEMC_DATA19
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x0b8, 0x00000008); // IOMUXC_GPIO_EMC_B2_04_SEMC_DATA20
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x0bc, 0x00000008); // IOMUXC_GPIO_EMC_B2_05_SEMC_DATA21
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x0c0, 0x00000008); // IOMUXC_GPIO_EMC_B2_06_SEMC_DATA22
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x0c4, 0x00000008); // IOMUXC_GPIO_EMC_B2_07_SEMC_DATA23
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x0c8, 0x00000008); // IOMUXC_GPIO_EMC_B2_08_SEMC_DM02
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x0cc, 0x00000008); // IOMUXC_GPIO_EMC_B2_09_SEMC_DATA24
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x0d0, 0x00000008); // IOMUXC_GPIO_EMC_B2_10_SEMC_DATA25
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x0d4, 0x00000008); // IOMUXC_GPIO_EMC_B2_11_SEMC_DATA26
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x0d8, 0x00000008); // IOMUXC_GPIO_EMC_B2_12_SEMC_DATA27
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x0dc, 0x00000008); // IOMUXC_GPIO_EMC_B2_13_SEMC_DATA28
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x0e0, 0x00000008); // IOMUXC_GPIO_EMC_B2_14_SEMC_DATA29
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x0e4, 0x00000008); // IOMUXC_GPIO_EMC_B2_15_SEMC_DATA30
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x0e8, 0x00000008); // IOMUXC_GPIO_EMC_B2_16_SEMC_DATA31
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x1ac, 0x00000008); // IOMUXC_GPIO_EMC_B2_17_SEMC_DM03
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x1b0, 0x00000008); // IOMUXC_GPIO_EMC_B2_18_SEMC_DQS4
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x1b4, 0x00000008); // IOMUXC_GPIO_EMC_B2_19_SEMC_CLKX00
	TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x1b8, 0x00000008); // IOMUXC_GPIO_EMC_B2_20_SEMC_CLKX01

	// Configure SDR Controller Registers/
	var SEMC = 0x400D4000;
	TargetInterface.pokeUint32 (SEMC + 0x00, 0x10000004); // MCR
	TargetInterface.pokeUint32 (SEMC + 0x08, 0x00030524); // BMCR0
	TargetInterface.pokeUint32 (SEMC + 0x0C, 0x06030524); // BMCR1
	TargetInterface.pokeUint32 (SEMC + 0x10, 0x8000001D); // BR0, 64MB

	TargetInterface.pokeUint32 (SEMC + 0x40, 0x00000F32); // SDRAMCR0, 32bit
	TargetInterface.pokeUint32 (SEMC + 0x44, 0x00772A22); // SDRAMCR1
	TargetInterface.pokeUint32 (SEMC + 0x48, 0x00010A0D); // SDRAMCR2
	TargetInterface.pokeUint32 (SEMC + 0x4C, 0x21210408); // SDRAMCR3

	TargetInterface.pokeUint32 (SEMC + 0x90, 0x80000000); // IPCR0
	TargetInterface.pokeUint32 (SEMC + 0x94, 0x00000002); // IPCR1
	TargetInterface.pokeUint32 (SEMC + 0x98, 0x00000000); // IPCR2

	TargetInterface.pokeUint32 (SEMC + 0x9C, 0xA55A000F); // IPCMD, SD_CC_IPREA
	SDRAM_WaitIpCmdDone        (SEMC);
	TargetInterface.pokeUint32 (SEMC + 0x9C, 0xA55A000C); // SD_CC_IAF
	SDRAM_WaitIpCmdDone        (SEMC);
	TargetInterface.pokeUint32 (SEMC + 0x9C, 0xA55A000C); // SD_CC_IAF
	SDRAM_WaitIpCmdDone        (SEMC);
	TargetInterface.pokeUint32 (SEMC + 0xA0, 0x00000033); // IPTXDAT
	TargetInterface.pokeUint32 (SEMC + 0x9C, 0xA55A000A); // SD_CC_IMS
	SDRAM_WaitIpCmdDone        (SEMC);

	TargetInterface.pokeUint32 (SEMC + 0x150, 0x00000017); // DCCR - Delay Chain Control Register
	TargetInterface.pokeUint32 (SEMC + 0x4C, 0x21210409); // enable SDRAM self refresh after initialization done.
}



function AlterRegister (Addr, Clear, Set)
{
	var temp = TargetInterface.peekUint32 (Addr);
	temp &= ~Clear;
	temp |= Set;
	TargetInterface.pokeUint32 (Addr, temp);
}

function FLEXSPI_Init (FlexSPI)
{
	var FlexSPI1 = 0x402A8000;
	var FlexSPI2 = 0x402A4000;

	switch (DeviceName)
	{
		case "MIMXRT1011":
		case "MIMXRT1015":
		case "MIMXRT1021":
		case "MIMXRT1051":
		case "MIMXRT1052":
			FlexSPI2 = 0;			// no second flexSPI
			break;
		case "MIMXRT1061":
		case "MIMXRT1062":
		case "MIMXRT1064":
			break;					// do nothing
		case "MIMXRT1171_cm7":
		case "MIMXRT1172_cm7":
		case "MIMXRT1173_cm7":
		case "MIMXRT1175_cm7":
		case "MIMXRT1176_cm7":
			FlexSPI1 = 0x400CC000;
			FlexSPI2 = 0x400D0000;
			break;
		default:
			TargetInterface.message ("FLEXSPI_Init - unknown Device: " + DeviceName);
			return;
	}

	var base = 0;
	switch (FlexSPI)
	{
		case 1:
			base = FlexSPI1;
			break;
		case 2:
			base = FlexSPI2;
			break;
		default:
			TargetInterface.message ("FLEXSPI_Init - Invalid Interface");
	}

	var CCM = 0x400FC000;
	var CCM_CBCMR  = CCM + 0x18;
	var CCM_CSCMR1 = CCM + 0x1C;
	var CCM_CSCMR2 = CCM + 0x20;

	var CCM_ANALOG = 0x400D8000;
	var CCM_ANALOG_PFD480 = CCM_ANALOG + 0xF0;

	// Set flexSPI root clock to 166MHZ.
	AlterRegister (CCM_ANALOG_PFD480, 0xBF, 0x80);	// Disable the clock output first.
	AlterRegister (CCM_ANALOG_PFD480, 0xBF, 26);	// Set the new value and enable output.

	// Choose PLL3 PFD0 clock as flexSPI source clock and set internal clock 83 megahertz
	AlterRegister (CCM_CSCMR1, 0x60000000 | 0x3800000, (3 << 29) | (3 << 23));

	var FlexSPI_MCR0        = base + 0x00;
	var FlexSPI_MCR1        = base + 0x04;
	var FlexSPI_MCR2        = base + 0x08;
	var FlexSPI_AHBCR       = base + 0x0C;
	var FlexSPI_AHBRXBUFCR0 = base + 0x20;
	var FlexSPI_IPRXFCR     = base + 0xB8;
	var FlexSPI_IPTXFCR     = base + 0xBC;
	var FlexSPI_FLSHCR0     = base + 0x60;
	AlterRegister (FlexSPI_MCR0, 2, 0);	// Enable flexSPI

	TargetInterface.pokeUint32     (FlexSPI_MCR0,  0xffff3032);
	TargetInterface.pokeUint32     (FlexSPI_MCR1,  0xffffffff);
	TargetInterface.pokeUint32     (FlexSPI_MCR2,  0x200801f7);
	TargetInterface.pokeUint32     (FlexSPI_AHBCR, 0x00000078);
	TargetInterface.pokeMultUint32 (FlexSPI_AHBRXBUFCR0, [0x20, 0x20, 0x20, 0x20]);
	TargetInterface.pokeUint32     (FlexSPI_IPRXFCR, 0);
	TargetInterface.pokeUint32     (FlexSPI_IPTXFCR, 0);
	TargetInterface.pokeMultUint32 (FlexSPI_FLSHCR0, [0x0, 0x0, 0x0, 0x0]);

	AlterRegister                  (FlexSPI_MCR0,  0, 1);	// Perform a SW-Reset
}