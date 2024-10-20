/*****************************************************************************
 * Original work Copyright (c) 2016 Rowley Associates Limited.               *
 * Modified work Copyright (C) 2019-2024 Markus Klein                        *
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
var SRC_SRMR         = SRC + 0x4;
var SRC_CTRL_MEGA    = SRC + 0x204;
var SRC_STAT_MEGA    = SRC + 0x210;
var SRC_CTRL_DISPLAY = SRC + 0x224;
var SRC_STAT_DISPLAY = SRC + 0x230;
var SRC_CTRL_M7CORE  = SRC + 0x2A4;
var SRC_STAT_M7CORE  = SRC + 0x2B0;
var SRC_CTRL_M4CORE  = SRC + 0x284;
var SRC_STAT_M4CORE  = SRC + 0x290;

var CCM           = 0x40CC0000;
var CCM_CLOCK_ROOT_M7_CONTROL    = CCM + (0 * 0x80) + 0;
var CCM_CLOCK_ROOT_M4_CONTROL    = CCM + (1 * 0x80) + 0;
var CCM_CLOCK_ROOT_TRACE_CONTROL = CCM + (6 * 0x80) + 0;
var CCM_LPCG      = CCM + 0x6000;
var CCM_LPCG_LMEM = CCM_LPCG + (27 * 0x20); // Index 27 with offset size of 32
var CCM_LPCG_Cstrace = CCM_LPCG + (42 * 0x20); // Index 42 with offset size of 32

var LMEM        = 0xE0082000;
var LMEM_PCCCR  = LMEM + 0x0;   // PCode bus Cache control register   --> Instruction
var LMEM_PCCLCR = LMEM + 0x4;   // Cache line control register
var LMEM_PCCSAR = LMEM + 0x8;   // Cache search address register
var LMEM_PCCCVR = LMEM + 0xC;   // Cache read/write value register
var LMEM_PSCCR  = LMEM + 0x800; // PSystem bus Cache control register --> Data
var LMEM_PSCLCR = LMEM + 0x804; // Cache line control register
var LMEM_PSCSAR = LMEM + 0x808; // Cache search address register
var LMEM_PSCCVR = LMEM + 0x80C; // Cache read/write value register

var IOMUXC_LPSR_GPR   = 0x40C0C000;
var IOMUXC_LPSR_GPR0  = IOMUXC_LPSR_GPR + 0x0;
var IOMUXC_LPSR_GPR1  = IOMUXC_LPSR_GPR + 0x4;
var IOMUXC_LPSR_GPR2  = IOMUXC_LPSR_GPR + 0x8;
var IOMUXC_LPSR_GPR26 = IOMUXC_LPSR_GPR + (4 * 26);
var IOMUXC_LPSR_GPR26 = IOMUXC_LPSR_GPR + (4 * 26);

var IOMUXC_GPR       = 0x400E4000;
var IOMUXC_GPR_GPR0  = IOMUXC_GPR;
var IOMUXC_GPR_GPR1  = IOMUXC_GPR + 0x04;
var IOMUXC_GPR_GPR18 = IOMUXC_GPR + 0x48;
var IOMUXC_GPR_GPR26 = IOMUXC_GPR + 0x68;



// This function is called each time the debugger stops the target
// If the cache is enabled, we clean the data cache as the JTAG access is not cache coherent
function OnTargetStop_11xx ()
{
	var DeviceName = GetProjectPartName ();
	if (DeviceName.slice(-4) != "_cm4")
		return; // only necessary on M4-Core

	TargetInterface.message ("## Target " + DeviceName + " has been stopped");
	// Invalidate the data cache
	var LMEM_PSCCR_ENCACHE_MASK = 0x00000001;
//	var LMEM_PSCCR_ENWRBUF_MASK = 0x00000002;
//	var LMEM_PSCCR_INVW0_MASK   = 0x01000000;
//	var LMEM_PSCCR_INVW1_MASK   = 0x04000000;
	var LMEM_PSCCR_PUSHW0_MASK  = 0x02000000;
	var LMEM_PSCCR_PUSHW1_MASK  = 0x08000000;
	var LMEM_PSCCR_GO_MASK      = 0x80000000;

	// Check if the cache is clockgated
	var CCM_LPCG_LmemDirect = TargetInterface.peekUint32 (CCM_LPCG_LMEM);
	if (CCM_LPCG_LmemDirect == 0)
	{
		TargetInterface.message ("## OnTargetStop_11xx - cache is clockgated");
		return;	// cache is clock gate, so no need to clear it
	}

	// Check if the cache is enabled
	var psccr = TargetInterface.peekUint32 (LMEM_PSCCR);
	if ((psccr & LMEM_PSCCR_ENCACHE_MASK) == 0)
	{
		TargetInterface.message ("## OnTargetStop_11xx - cache is disabled");
		return;	// no cache enabled, so no need to clear it
	}

	// Enable the processor system bus to push all modified lines.
	psccr |= LMEM_PSCCR_PUSHW0_MASK | LMEM_PSCCR_PUSHW1_MASK | LMEM_PSCCR_GO_MASK;
	TargetInterface.pokeUint32 (LMEM_PSCCR, psccr);
	do
	{
		psccr = TargetInterface.peekUint32 (LMEM_PSCCR);
	}
	while ((psccr & LMEM_PSCCR_GO_MASK) != 0);	// Wait until the cache command completes

	// As a precaution clear the bits to avoid inadvertently re-running this command.
	psccr &= ~(LMEM_PSCCR_PUSHW0_MASK | LMEM_PSCCR_PUSHW1_MASK);
	TargetInterface.pokeUint32 (LMEM_PSCCR, psccr);

	TargetInterface.message ("## Target " + DeviceName + " has been stopped - done");
}

// This function is called each time the debugger starts the target
// If the cache is enabled, we invalidate the code cash as the JTAG access is not cache coherent
function InvalidateICache_LMEM ()
{
	var DeviceName = GetProjectPartName ();
	if (DeviceName.slice(-4) != "_cm4")
		return; // only necessary on M4-Core

	TargetInterface.message ("## Target " + DeviceName + " has been started");

	var LMEM_PCCCR_ENCACHE_MASK = 0x00000001;
	var LMEM_PCCCR_ENWRBUF_MASK = 0x00000002;
	var LMEM_PCCCR_INVW0_MASK   = 0x01000000;
	var LMEM_PCCCR_INVW1_MASK   = 0x04000000;
	var LMEM_PCCCR_PUSHW0_MASK  = 0x02000000;
	var LMEM_PCCCR_PUSHW1_MASK  = 0x08000000;
	var LMEM_PCCCR_GO_MASK      = 0x80000000;

	// Check if the cache is clockgated
	var CCM_LPCG_LmemDirect = TargetInterface.peekUint32 (CCM_LPCG_LMEM);
	if (CCM_LPCG_LmemDirect == 0)
	{
		TargetInterface.message ("## OnTargetRun_11xx - cache is clockgated");
		return;	// cache is clock gate, so no need to clear it
	}

	// Check if the cache is enabled
	var pcccr = TargetInterface.peekUint32 (LMEM_PCCCR);
	if ((pcccr & LMEM_PCCCR_ENCACHE_MASK) == 0)
	{
		TargetInterface.message ("## OnTargetRun_11xx - cache is disabled");
		return;	// no cache enabled, so no need to clear it
	}

	// Enables the processor code bus to invalidate all lines in both ways.
	// and Initiate the processor code bus code cache command.
	pcccr |= LMEM_PCCCR_INVW0_MASK | LMEM_PCCCR_INVW1_MASK | LMEM_PCCCR_GO_MAS;
	TargetInterface.pokeUint32 (LMEM_PCCCR, pcccr);

	do
	{
		pcccr = TargetInterface.peekUint32 (LMEM_PCCCR);
	}
	while ((pcccr & LMEM_PCCCR_GO_MASK) != 0);	// Wait until the cache command completes

	// As a precaution clear the bits to avoid inadvertently re-running this command.
	pcccr &= ~(LMEM_PCCCR_INVW0_MASK | LMEM_PCCCR_INVW1_MASK);
	TargetInterface.pokeUint32 (LMEM_PCCCR, pcccr);

	TargetInterface.message ("## Target " + DeviceName + " has been started - done");
}

// The script that is called to set/clear software breakpoint instructions.
// When executed this script has the macros $(address), $(instruction), $(size) expanded.
// These specify the address to poke the instruction and the size of the instruction in bytes.
function PokeInstruction (address, instruction, size)
{
	var DeviceName = GetProjectPartName ();
	if (DeviceName.slice(-4) != "_cm4")
		return; // only necessary on M4-Core

	TargetInterface.message ("## PokeInstruction on " + address.toString(16));

	if (size == 4)
		TargetInterface.pokeUint32 (address, instruction);
	else
		TargetInterface.pokeUint16 (address, instruction);
	if (address >= 0x20000000)
	{
		TargetInterface.pokeUint32 (LMEM_PSCLCR, 0x5<<24);
		TargetInterface.pokeUint32 (LMEM_PSCSAR, address|1);
		while (TargetInterface.peekUint32 (LMEM_PSCLCR) & 1)
			;
	}
	else
	{
		TargetInterface.pokeUint32 (LMEM_PCCLCR, 0x5<<24);
		TargetInterface.pokeUint32 (LMEM_PCCSAR, address|1);
		while (TargetInterface.peekUint32 (LMEM_PCCLCR) & 1)
			;
	}
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
			TargetInterface.setDebugInterfaceProperty ("set_adiv5_AHB_ap_num", 0, 0x40000000, 0x00000000); // LPC Solution
			break;
		case "MIMXRT1181":
		case "MIMXRT1182":
		case "MIMXRT1187_cm33":
		case "MIMXRT1189_cm33":
//			TargetInterface.setDeviceTypeProperty ("MIMXRT1189xxxx_M33");
			TargetInterface.setDebugInterfaceProperty ("set_adiv5_AHB_ap_num", 3, 0x40000000, 0x00000000); // LPC Solution
			break;
		case "MIMXRT1187_cm7":
		case "MIMXRT1189_cm7":
//			TargetInterface.setDeviceTypeProperty ("MCIMXRT1180_M7");
			if (TargetInterface.implementation() == "j-link")
				TargetInterface.setDebugInterfaceProperty ("set_adiv5_AHB_ap_num", 2);
			else
				TargetInterface.setDebugInterfaceProperty ("set_adiv5_AHB_ap_num", 3, 0x40000000, 0x00000000); // LPC Solution
			break;
		default:
			// The other member of the family work with the default.
	}


	TargetInterface.message ("## Connect to " + DeviceName + " - done");
}

function FillMemory (address, size, value)
{
	var fillData = new Array (size/4);
	for (var i=0; i<size/4; i++)
		fillData[i] = value;
	TargetInterface.pokeMultUint32 (address, fillData);
}


// This function is used to return the controller type as a string
// we use it also to do some initializations as this function is called right before
// writing the code to the controller
function GetPartName ()
{
	var DeviceName = GetProjectPartName ();
	TargetInterface.message ("## get part name of " + DeviceName);

	var PART = "";

	if (TargetInterface.implementation () != "j-link")
	{
		if (TargetInterface.getProjectProperty ("Target").indexOf ("_cm4") != -1)
		{
			Release_11xx_M4 ();	// Enable M4 core
			TargetInterface.setDebugInterfaceProperty ("set_adiv5_AHB_ap_num", 1);
		}

		switch (DeviceName)
		{
			case "MIMXRT1181":
			case "MIMXRT1182":
			case "MIMXRT1187_cm33":
			case "MIMXRT1189_cm33":
				// Invalidate and disable XCACHEs
//				TargetInterface.pokeUint32 (0x44400000, 0x85000000);
//				TargetInterface.pokeUint32 (0x44400800, 0x85000000);

				TargetInterface.resetDebugInterface ();
				break;
			case "MIMXRT1187_cm7":
			case "MIMXRT1189_cm7":
				TargetInterface.resetDebugInterface ();
				Release_118x_M7 (); // Enable M7 core
				TargetInterface.setDebugInterfaceProperty ("set_adiv5_AHB_ap_num", 2);
			break;
		}
	}

	if (TargetInterface.getProjectProperty ("Target").indexOf ("MIMXRT11") != -1)
	{
		// Read the chip silicon version
		var ANADIG_MISC_DIFPROG = 0x40C84000 + 0x800;
		if (TargetInterface.getProjectProperty ("Target").indexOf ("MIMXRT118") != -1)
		{
			ANADIG_MISC_DIFPROG = 0x44480000 + 0x4800;
			TargetInterface.message ("#### 1180 device???");
		}
		var ChipID = TargetInterface.peekUint32 (ANADIG_MISC_DIFPROG); // ANADIG_MISC-->MISC_DIFPROG (Chip Silicon Version Register)
		ChipID &= 0x00FFFF00;
		ChipID >>= 8;
		TargetInterface.message ("#### Device detected: MIMXRT" + ChipID.toString(16));
/*		PART = "MIMXRT" + ChipID.toString(16); */
	}
	// 1180 --> 0x44484800

	TargetInterface.message ("## get part name of " + DeviceName + " done");

	return PART;
}

function MatchPartName (name)
{
	TargetInterface.message ("## MatchPartName with " + name);
	var partName = GetPartName ();

	if (partName == "")
		return true;

	return partName.substring (0, 6) == name.substring (0, 6);
}

function Release_11xx_M4 ()
{
	// Setup some spin code into an area of D-TCM (0x2021FF00)
	TargetInterface.pokeUint32 (0x2021FF00, 0xE7FEE7FE); // 0x2021FF00 OCRAM -->0x2001FF00 DTCM
	TargetInterface.pokeUint32 (0x2021FF04, 0x2021FF01);
	// Set top/bottom 16 bits of RAM address into CM4 VTOR iomuxc_lpsr_GPR0/1
	TargetInterface.pokeUint32 (IOMUXC_LPSR_GPR0, 0xFF00);
	TargetInterface.pokeUint32 (IOMUXC_LPSR_GPR1, 0x2021);

	// Set m4_clk_root to OSC_RC_400M / 2: CLOCK_ROOT1 = mux(2), div(1)
	TargetInterface.pokeUint32 (CCM_CLOCK_ROOT_M4_CONTROL, 0x201);

	// Save current reset SRMR and prevent M4 SW reset affecting the system
	var srmr = TargetInterface.peekUint32 (SRC_SRMR);
	TargetInterface.pokeUint32 (SRC_SRMR, 0x00000C00);
	TargetInterface.pokeUint32 (SRC_CTRL_M4CORE, 0x1);
	TargetInterface.pokeUint32 (SRC_SRMR, srmr);

	// Release M4 if needed
	var scr = TargetInterface.peekUint32 (SRC_SCR);
	if ((scr & 0x1) == 0)
	{
		// Releasing M4
		scr |= 1;
		TargetInterface.pokeUint32 (SRC_SCR, scr);
	}
}

function Release_11xx_M7 ()
{
	// Setup some spin code into an area of OCRAM1 (0x2024FF00)
	TargetInterface.pokeUint32 (0x2024FF00, 0xE7FEE7FE);
	TargetInterface.pokeUint32 (0x2024FF04, 0x2024FF01);
	// Set addr >> 7 into CM7 VTOR iomuxc_lpsr_GPR26
	TargetInterface.pokeUint32 (IOMUXC_LPSR_GPR26, 0x4049FE);

	// Set m7_clk_root to OSC_RC_48M / 2: CLOCK_ROOT0 = mux(0), div(1)
	TargetInterface.pokeUint32 (CCM_CLOCK_ROOT_M7_CONTROL, 0x001);

	// Save current reset SRMR and prevent M7 SW reset affecting the system
	var srmr = TargetInterface.peekUint32 (SRC_SRMR);
	TargetInterface.pokeUint32 (SRC_SRMR, 0x00003000);
	TargetInterface.pokeUint32 (SRC_CTRL_M7CORE, 0x1);
	TargetInterface.pokeUint32 (SRC_SRMR, srmr);

	// Release M7 if needed
	var scr = TargetInterface.peekUint32 (SRC_SCR);
	if ((scr & 0x2) == 0)
	{
		// Releasing M7
		scr |= 2;
		TargetInterface.pokeUint32 (SRC_SCR, scr);
	}
}


function Release_118x_M7 ()
{
	TargetInterface.message ("************* Begin Operations to Enable CM7 ***********************");

	// Clock Preparation
	TargetInterface.message ("******** Prepare Clock *********");
	TargetInterface.pokeUint32 (0x54484310, 0x007901F2);	// ANADIG_OSC->OSC_RC24M_CTRL
	TargetInterface.pokeUint32 (0x54484320, 0x40000014);	// ANADIG_OSC->OSC_24M_CTRL
	TargetInterface.pokeUint32 (0x54450080, 0x0);			// CCM->CLOCK_ROOT1_CONTROL
	TargetInterface.delay (50);
	TargetInterface.pokeUint32 (0x54484680, 0x105);
	TargetInterface.delay (50);
	TargetInterface.pokeUint32 (0x54484710, 0x70);
	TargetInterface.pokeUint32 (0x54484000, 0x400020a6);
	TargetInterface.delay (50);
	// check 54484000
	TargetInterface.pokeUint32 (0x54484000, 0x000060a6);

	// DCDC_SetVDD1P0BuckModeTargetVoltage(DCDC, kDCDC_1P0Target1P1V);
	TargetInterface.pokeUint32 (0x5452000C, 0x980000);
	TargetInterface.pokeUint32 (0x54520024, 0x100C14);

	// M7 Clk
	TargetInterface.pokeUint32 (0x54450000, 0x201);      // CCM->CLOCK_ROOT0_CONTROL

	// VTOR
	TargetInterface.pokeUint32 (0x544F0080, 0x10);

	// Release CM7
	TargetInterface.pokeUint32 (0x54460010, 0x1);

	// DMA initialization
	TargetInterface.message ("******** DMA operation *********");
	InitCM7DMA (0x303C0000);
	InitCM7DMA (0x303E0000);
	InitCM7DMA (0x30400000);
	InitCM7DMA (0x30420000);

	// Making Landing Zone
	TargetInterface.message ("******** Creating Landing Zone *********");
	TargetInterface.pokeUint32 (0x303C0000, 0x20020000);
	TargetInterface.pokeUint32 (0x303C0004, 0x00000009);
	TargetInterface.pokeUint32 (0x303C0008, 0xE7FEE7FE);
	TargetInterface.peekUint32 (0x303C0000);
	TargetInterface.peekUint32 (0x303C0004);
	TargetInterface.peekUint32 (0x303C0008);

	// Trigger S401 - EdgeLock APC Request
	TargetInterface.message ("******** S401 Trigger *********");
	TargetInterface.pokeUint32 (0x57540200, 0x17d20106); // MU_RT_S3MUA->TR[0]
	var resp1 = TargetInterface.peekUint32 (0x57540280); // MU_RT_S3MUA->RR[0]
	var resp2 = TargetInterface.peekUint32 (0x57540284); // MU_RT_S3MUA->RR[1]
	TargetInterface.message ("RESP1 : " + resp1);  
	TargetInterface.message ("RESP2 : " + resp2);  

	// Deassert CM7 Wait
	TargetInterface.message ("******** Kickoff CM7 *********");
	TargetInterface.pokeUint32 (0x544F0080, 0x0);
}

function InitCM7DMA (targetAddr)
{
	TargetInterface.pokeUint32 (0x52010020, 0x20200000);	// DMA4->TCD[0].SADDR
	TargetInterface.pokeUint32 (0x52010030, targetAddr);	// DMA4->TCD[0].DADDR
	TargetInterface.pokeUint32 (0x52010028, 0x00020000);	// DMA4->TCD[0].NBYTES_MLOFFNO
	TargetInterface.pokeUint16 (0x52010036, 0x1);			// DMA4->TCD[0].ELINKNO
	TargetInterface.pokeUint16 (0x5201003E, 0x1);			// DMA4->TCD[0].BITER_ELINKNO
	TargetInterface.pokeUint16 (0x52010026, 0x0303);		// DMA4->TCD[0].ATTR
	TargetInterface.pokeUint16 (0x52010024, 0x0);			// DMA4->TCD[0].SOFF
	TargetInterface.pokeUint16 (0x52010034, 0x8);			// DMA4->TCD[0].DOFF
	TargetInterface.pokeUint32 (0x52010000, 0x7);			// DMA4->TDC[0].CH_CSR
	TargetInterface.pokeUint16 (0x5201003C, 0x8);			// DMA4->TCD[0].CSR
	TargetInterface.pokeUint32 (0x5201003C, 0x9);			// DMA4->TCD[0].CSR
	TargetInterface.delay (50);
	TargetInterface.pokeUint32 (0x52010000, 0x40000006);
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
//	TargetInterface.pokeUint32 (0xE000ED0C, 0x5FA0001);
//	TargetInterface.delay (10);
}

function Reset_11xx_M7 ()
{
	// ARM Sys Reset
	TargetInterface.pokeUint32 (SRC_SRMR, 0x1400);

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

	// Reconnect after reset
	TargetInterface.delay (10);
	TargetInterface.resetDebugInterface ();
	TargetInterface.stop (1000);
}

function Reset ()
{
	if (TargetInterface.implementation() == "crossworks_simulator")
		return;

	if (TargetInterface.implementation() == "j-link")
	{
//		TargetInterface.resetAndStop (1000);
		TargetInterface.stop ();
		FlexRAM_Restore ();
		return;
	}

	var DeviceName = GetProjectPartName ();
	TargetInterface.message ("## Reset " + DeviceName);
	switch (DeviceName)
	{
		case "MIMXRT633":
		case "MIMXRT685_cm33":
/*			TargetInterface.message ("Reset external flash");
			TargetInterface.pokeUint32 (0x40004130, 0x130);
			TargetInterface.pokeUint32 (0x40021044, 0x4);
			TargetInterface.pokeUint32 (0x40020074, 0x4);
			TargetInterface.pokeUint32 (0x40102008, 0x1000);
			TargetInterface.pokeUint32 (0x40102288, 0x1000);
			TargetInterface.delay (10);
			TargetInterface.pokeUint32 (0x40102208, 0x1000);

			TargetInterface.message ("Set watch point");
			TargetInterface.pokeUint32 (0xE0001020, 0x50002034);
			TargetInterface.pokeUint32 (0xE0001028, 0x00000814);

			TargetInterface.message ("Execute SYSRESETREQ via AIRCR");
			TargetInterface.pokeUint32 (0xE000ED0C, 0x05FA0004);

			TargetInterface.delay (100);

			if (TargetInterface.isStopped ())
			{
				TargetInterface.message ("Clear watch point");
				TargetInterface.pokeUint32 (0xE0001020, 0x00000000);
				TargetInterface.pokeUint32 (0xE0001028, 0x00000000);
			}
			else */
				TargetInterface.resetAndStop (1000);
			break;
		case "MIMXRT1011":
		case "MIMXRT1015":
		case "MIMXRT1021":
		case "MIMXRT1024":
		case "MIMXRT1041":
		case "MIMXRT1042":
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
			if (!TargetInterface.isStopped ())
				TargetInterface.stop ();
			break;
		case "MIMXRT1165_cm7":
		case "MIMXRT1166_cm7":
		case "MIMXRT1173_cm7":
		case "MIMXRT1175_cm7":
		case "MIMXRT1176_cm7":
			TargetInterface.resetAndStop (1000);
			Reset_11xx_M7 ();
			Release_11xx_M4 ();	// Enable the M4 Core
			if (!TargetInterface.isStopped ())
				TargetInterface.stop ();
			break;
		case "MIMXRT1165_cm4":
		case "MIMXRT1166_cm4":
		case "MIMXRT1173_cm4":
		case "MIMXRT1175_cm4":
		case "MIMXRT1176_cm4":
			Reset_11xx_M4 ();
			TargetInterface.stop ();
			if (!TargetInterface.isStopped ())
				TargetInterface.stop ();
			break;
		case "MIMXRT1181":
		case "MIMXRT1182":
		case "MIMXRT1187_cm33":
		case "MIMXRT1189_cm33":
			TargetInterface.resetAndStop (1000);
			FillMemory (0x1FFE0000, 0x20000, 0); // ITCM
			FillMemory (0x20000000, 0x20000, 0); // DTCM
			FillMemory (0xE000E180, 0x40,    0xFFFFFFFF); // NVIC_ICER
			FillMemory (0xE000E280, 0x40,    0xFFFFFFFF); // NVIC_ICPR
			break;
		case "MIMXRT1187_cm7":
		case "MIMXRT1189_cm7":
			TargetInterface.stop ();
			// Disable pending interrupts
			FillMemory (0xE000E180, 0x40,    0xFFFFFFFF); // NVIC_ICER
			FillMemory (0xE000E280, 0x40,    0xFFFFFFFF); // NVIC_ICPR
			break;
		default:
			TargetInterface.message ("## Reset - unknown Device: " + DeviceName);
			break;
	}

	FlexRAM_Restore ();
	TargetInterface.message ("## Reset " + DeviceName + " - done");
}

function DcDc_Init_10xx ()
{
	var ocotp_base = 0x401F4000;
	var ocotp_fuse_bank0_base = ocotp_base + 0x400;
	var dcdc_base = 0x40080000;

	var dcdc_trim_loaded = 0;

	var reg = TargetInterface.peekUint32 (ocotp_fuse_bank0_base + 0x90);
	if (reg & (1<<10))
	{
		// DCDC: REG0->VBG_TRM
		var trim_value = (reg & (0x1F << 11)) >> 11; 
		reg = (TargetInterface.peekUint32 (dcdc_base + 0x4) & ~(0x1F << 24)) | (trim_value << 24);
		TargetInterface.pokeUint32 (dcdc_base + 0x4, reg);
		dcdc_trim_loaded = 1;
	}

	reg = TargetInterface.peekUint32 (ocotp_fuse_bank0_base + 0x80);
	if (reg & (1<<30))
	{
		var index = (reg & (3 << 28)) >> 28;
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
}

function EnableTrace (traceInterfaceType)
{
	if (traceInterfaceType == "ETB" || traceInterfaceType == "SWO")
	{
		TargetInterface.message ("## Trace with '" + traceInterfaceType + "' not supported");
	}
	else if (traceInterfaceType == "TracePort")
	{
		var DeviceName = GetProjectPartName ();

		TargetInterface.message ("## EnableTrace with 'TracePort' on " + DeviceName);

		if (TargetInterface.getProjectProperty ("Target").indexOf ("MIMXRT10") != -1)
		{
			var IOMUXC                = 0x401F8000;
			var IOMUXC_SW_MUX_CTL_PAD = IOMUXC;
			var IOMUXC_SW_PAD_CTL_PAD = IOMUXC;

			if (DeviceName == "MIMXRT1011")
			{
				IOMUXC_SW_MUX_CTL_PAD = IOMUXC + 0x10;
				IOMUXC_SW_PAD_CTL_PAD = IOMUXC + 0xC0;

				TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x30, 7);		// Trace clock mux config
				TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x30, 0xF1);	// Trace clock pin config  Speed 200 MHz - Fast_Slew_Rate

				TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x38, 7);		// Trace Pin0 mux config
				TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x38, 0xF1);	// Trace Pin0 pin config  Speed 200 MHz - Fast_Slew_Rate

				TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x78, 7);		// Trace Pin1 mux config
				TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x78, 0xF1);	// Trace Pin1 pin config  Speed 200 MHz - Fast_Slew_Rate

				TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x7C, 7);		// Trace Pin2 mux config
				TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x7C, 0xF1);	// Trace Pin2 pin config  Speed 200 MHz - Fast_Slew_Rate

				TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x80, 7);		// Trace Pin3 mux config
				TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x80, 0xF1);	// Trace Pin3 pin config  Speed 200 MHz - Fast_Slew_Rate
			}
			else if (DeviceName == "MIMXRT1015")
			{
				IOMUXC_SW_MUX_CTL_PAD = IOMUXC + 0x24;
				IOMUXC_SW_PAD_CTL_PAD = IOMUXC + 0x198;

				TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0xC0, 6);		// Trace clock mux config
				TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0xC0, 0xF1);	// Trace clock pin config  Speed 200 MHz - Fast_Slew_Rate

				TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0xC8, 6);		// Trace Pin0 mux config
				TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0xC8, 0xF1);	// Trace Pin0 pin config  Speed 200 MHz - Fast_Slew_Rate

				TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0xCC, 6);		// Trace Pin1 mux config
				TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0xCC, 0xF1);	// Trace Pin1 pin config  Speed 200 MHz - Fast_Slew_Rate

				TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0xD0, 6);		// Trace Pin2 mux config
				TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0xD0, 0xF1);	// Trace Pin2 pin config  Speed 200 MHz - Fast_Slew_Rate

				TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0xD4, 6);		// Trace Pin3 mux config
				TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0xD4, 0xF1);	// Trace Pin3 pin config  Speed 200 MHz - Fast_Slew_Rate
			}
			else if (DeviceName == "MIMXRT1021" || DeviceName == "MIMXRT1024")
			{
				IOMUXC_SW_MUX_CTL_PAD = IOMUXC + 0x14;
				IOMUXC_SW_PAD_CTL_PAD = IOMUXC + 0x188;

				TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0xD0, 6);		// Trace clock mux config
				TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0xD0, 0xF1);	// Trace clock pin config  Speed 200 MHz - Fast_Slew_Rate

				TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0xD8, 6);		// Trace Pin0 mux config
				TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0xD8, 0xF1);	// Trace Pin0 pin config  Speed 200 MHz - Fast_Slew_Rate

				TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0xDC, 6);		// Trace Pin1 mux config
				TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0xDC, 0xF1);	// Trace Pin1 pin config  Speed 200 MHz - Fast_Slew_Rate

				TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0xE0, 6);		// Trace Pin2 mux config
				TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0xE0, 0xF1);	// Trace Pin2 pin config  Speed 200 MHz - Fast_Slew_Rate

				TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0xE4, 6);		// Trace Pin3 mux config
				TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0xE4, 0xF1);	// Trace Pin3 pin config  Speed 200 MHz - Fast_Slew_Rate
			}
			else /* 1040, 1050, 1060 */
			{
				IOMUXC_SW_MUX_CTL_PAD = IOMUXC + 0x14;
				IOMUXC_SW_PAD_CTL_PAD = IOMUXC + 0x204;

				TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x158, 2);		// Trace clock mux config
				TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x158, 0xF1);	// Trace clock pin config  Speed 200 MHz - Fast_Slew_Rate

				TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x138, 3);		// Trace Pin0 mux config
				TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x138, 0xF1);	// Trace Pin0 pin config  Speed 200 MHz - Fast_Slew_Rate

				TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x13C, 3);		// Trace Pin1 mux config
				TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x13C, 0xF1);	// Trace Pin1 pin config  Speed 200 MHz - Fast_Slew_Rate

				TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x140, 3);		// Trace Pin2 mux config
				TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x140, 0xF1);	// Trace Pin2 pin config  Speed 200 MHz - Fast_Slew_Rate

				TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x144, 3);		// Trace Pin3 mux config
				TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x144, 0xF1);	// Trace Pin3 pin config  Speed 200 MHz - Fast_Slew_Rate
			}

			var CCM        = 0x400FC000;
			var CCM_CBCMR  = CCM + 0x18;
			var CCM_CSCDR1 = CCM + 0x24;
			var CCM_CCGR0  = CCM + 0x68;
			// Use PLL2 as clock Source for the trace clock
			AlterRegister (CCM_CBCMR, 0xC000, 0);
			// Set the divider to four (which is the default) to get the maximum frequency of 132 MHz
			AlterRegister (CCM_CSCDR1, 0x1E000000, 0x6000000);
			// Clock Gate the trace-clock
			AlterRegister (CCM_CCGR0, 0, 0xC00000);
		}
		else if (TargetInterface.getProjectProperty ("Target").indexOf ("MIMXRT11") != -1)
		{
			var IOMUXC                = 0x400E8000;
			var IOMUXC_SW_MUX_CTL_PAD = IOMUXC + 0x10;
			var IOMUXC_SW_PAD_CTL_PAD = IOMUXC + 0x254;

			TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x21C, 3);		// Trace clock mux config
			TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x21C, 0);		// Trace clock pin config

			TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x20C, 3);		// Trace Pin0 mux config
			TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x20C, 0);		// Trace Pin0 pin config

			TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x210, 3);		// Trace Pin1 mux config
			TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x210, 0);		// Trace Pin1 pin config

			TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x214, 3);		// Trace Pin2 mux config
			TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x214, 0);		// Trace Pin2 pin config

			TargetInterface.pokeUint32 (IOMUXC_SW_MUX_CTL_PAD + 0x218, 3);		// Trace Pin3 mux config
			TargetInterface.pokeUint32 (IOMUXC_SW_PAD_CTL_PAD + 0x218, 0);		// Trace Pin3 pin config

			// Set trace_clk_root to SYS_PLL2_CLK / 4: CLOCK_ROOT0 = mux(7), div(3)
			TargetInterface.pokeUint32 (CCM_CLOCK_ROOT_TRACE_CONTROL, 0x703);

			// Enable clock gate
			TargetInterface.pokeUint32 (CCM_LPCG_Cstrace, 1); 
		}
		else
			return;
	}
	else
		TargetInterface.message ("## Unknown Trace type '" + traceInterfaceType + "'");
}

function GetProjectPartName ()
{
	var TargetFullName = TargetInterface.getProjectProperty ("Target");
	var TargetShort;
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

function FlexRAM_Restore ()
{
	var IOMUXC_GPR       = 0x400AC000;
	var IOMUXC_GPR_GPR16 = IOMUXC_GPR + 0x40;
	var IOMUXC_GPR_GPR17 = IOMUXC_GPR + 0x44;
	var DeviceName = GetProjectPartName ();
	TargetInterface.message ("## FlexRAM_Restore on " + DeviceName);

	switch (DeviceName)
	{
		case "MIMXRT1011":
		case "MIMXRT1015":
			TargetInterface.pokeUint32 (IOMUXC_GPR_GPR17, 0xE5);		// 64 KByte OCRAM - 32 kByte ITCM - 32 kByte DTCM
			break;
		case "MIMXRT1021":
		case "MIMXRT1024":
			TargetInterface.pokeUint32 (IOMUXC_GPR_GPR17, 0x5FA5);		// 128 KByte OCRAM - 64 kByte ITCM - 64 kByte DTCM
			break;
		case "MIMXRT1041":
		case "MIMXRT1042":
		case "MIMXRT1051":
		case "MIMXRT1052":
		case "MIMXRT1061":
		case "MIMXRT1062":
		case "MIMXRT1064":
			TargetInterface.pokeUint32 (IOMUXC_GPR_GPR17, 0x55AFFA55);	// 256 KByte OCRAM - 128 kByte ITCM - 128 kByte DTCM
			break;
		case "MIMXRT1165_cm7":
		case "MIMXRT1166_cm7":
		case "MIMXRT1171_cm7":
		case "MIMXRT1172_cm7":
		case "MIMXRT1173_cm7":
		case "MIMXRT1175_cm7":
		case "MIMXRT1176_cm7":
			var FLEXRAM              = 0x40028000;
			var FLEXRAM_FLEXRAM_CTRL = FLEXRAM + 0x108;
			// disable FLEXRAM-ECC (enabled by fuse or ROM) and restore the factory defaults (0)
			TargetInterface.pokeUint32 (FLEXRAM_FLEXRAM_CTRL, 0);

			IOMUXC_GPR           = 0x400E4000;
			IOMUXC_GPR_GPR16     = IOMUXC_GPR + 0x40;
			IOMUXC_GPR_GPR17     = IOMUXC_GPR + 0x44;
			var IOMUXC_GPR_GPR18 = IOMUXC_GPR + 0x48;

			TargetInterface.pokeUint32 (IOMUXC_GPR_GPR17, 0xFFAA);		// 0 KByte OCRAM - 256 kByte ITCM - 256 kByte DTCM
			TargetInterface.pokeUint32 (IOMUXC_GPR_GPR18, 0xFFAA);
			break;
		case "MIMXRT1165_cm4":
		case "MIMXRT1166_cm4":
		case "MIMXRT1173_cm4":
		case "MIMXRT1175_cm4":
		case "MIMXRT1176_cm4":
			// Ignore the M4-Core which has no FlexRAM
			break;
		case "MIMXRT1181":
		case "MIMXRT1182":
		case "MIMXRT1187_cm33":
		case "MIMXRT1189_cm33":
//			var BLK_CTRL_S_AONMIX = 0x444F0000;
//			var BLK_CTRL_S_AONMIX_M33_CFG = BLK_CTRL_S_AONMIX + 0x60;
			// BLK_CTRL_S_AONMIX --> M33_CFG[TCM_SIZE]
			// * 00 = REGULAR_TCM: 128kB Code TCM and 128kB Sys TCM (default)
			// * 01 = DOUBLE_CODE_TCM. 256kB Code TCM and 0 Sys TCM
			// * 10 = DOUBLE_SYS_TCM. 256kB Sys TCM and 0 Code TCM
			// * 11 = Reserved
//			AlterRegister (BLK_CTRL_S_AONMIX_M33_CFG, 0x3 << 3, 0);
			break;
		case "MIMXRT1187_cm7":
		case "MIMXRT1189_cm7":
//			var BLK_CTRL_S_AONMIX = 0x444F0000;
//			var BLK_CTRL_S_AONMIX_M7_CFG = BLK_CTRL_S_AONMIX + 0x80
			// BLK_CTRL_S_AONMIX --> M7_CFG[TCM_SIZE]
			// * 000b - Regular TCM, 256 KB ITCM and 256 KB DTCM (default)
			// * 001b - Double ITCM, 512 KB ITCM
			// * 010b - Double DTCM, 512 KB DTCM
			// * 100b - HALF ITCM, 128 KB ITCM and 384 KB DTCM
			// * 101b - HALF DTCM, 384 KB ITCM and 128 KB DTCM
			// * 011b - Reserved
			// * 110b - Reserved
			// * 111b - Reserved
//			AlterRegister (BLK_CTRL_S_AONMIX_M7_CFG, 0x7, 0);
			break;
		default:
			TargetInterface.message ("FlexRAM_Restore - unknown Device: " + DeviceName);
			return;
	}

	if (TargetInterface.getProjectProperty ("Target").indexOf ("MIMXRT118") == -1)
	{
		var gpr16 = TargetInterface.peekUint32 (IOMUXC_GPR_GPR16);
		gpr16 |= (1 << 2);	// Set FLEXRAM_BANK_CFG_SEL Flag to use FLEXRAM_BANK_CFG configuration
		TargetInterface.pokeUint32 (IOMUXC_GPR_GPR16, gpr16);
	}

	TargetInterface.message ("## FlexRAM_Restore - done");
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
		case "MIMXRT1041":
		case "MIMXRT1042":
		case "MIMXRT1051":
		case "MIMXRT1052":
		case "MIMXRT1061":
		case "MIMXRT1062":
		case "MIMXRT1064":
			Clock_Init_105x ();
			break;
		case "MIMXRT1165_cm7":
		case "MIMXRT1166_cm7":
		case "MIMXRT1171_cm7":
		case "MIMXRT1172_cm7":
		case "MIMXRT1173_cm7":
		case "MIMXRT1175_cm7":
		case "MIMXRT1176_cm7":
			Clock_Init_117x ();
			break;
		case "MIMXRT1181":
		case "MIMXRT1182":
		case "MIMXRT1187_cm33":
		case "MIMXRT1189_cm33":
			Clock_Init_118x ();
			break;
		default:
			TargetInterface.message ("Clock_Init - unknown Device: " + DeviceName);
			break;
	}
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
	////////// Initialize System PLL2 //////////
	var val = TargetInterface.peekUint32 (0x40C84240);			// ANADIG_PLL->PLL_528_CTRL
	if (val & 0x800000)
	{
		// SysPll2 has been initialized
		val &= ~0x40000000;
		TargetInterface.pokeUint32 (0x40C84240, val);		// ANADIG_PLL->PLL_528_CTRL
		TargetInterface.message ("syspll2 has been initialized already");
		return;
	}

	AlterRegister (0x40C84270, 0, 0x80808080);				// ANADIG_PLL->PLL_528_PFD
	AlterRegister (0x40C84240, 0x802000, 0x40000000);		// ANADIG_PLL->PPLL_528_CTRL
	TargetInterface.pokeUint32 (0x40C84280, 0);				// ANADIG_PLL->PLL_528_MFN
	TargetInterface.pokeUint32 (0x40C84290, 22);			// ANADIG_PLL->PLL_528_MFI
	TargetInterface.pokeUint32 (0x40C842A0, 0x0FFFFFFF);	// ANADIG_PLL->PLL_528_MFD

	TargetInterface.pokeUint32 (0x40C84240, 0x8 | 0x40000000); // ANADIG_PLL->PLL_528_CTRL
	TargetInterface.delay (30);

	AlterRegister (0x40C84240, 0, 0x800000 | 0x800);		// ANADIG_PLL_PLL_528_CTRL
	TargetInterface.delay (250);

	AlterRegister (0x40C84240, 0x800, 0);					// ANADIG_PLL->PLL_528_CTRL
	do
	{
		val = TargetInterface.peekUint32 (0x40C84240);		// ANADIG_PLL->PLL_528_CTRL
	} while ((val & 0x20000000) == 0);

	val |= 0x2000;
	TargetInterface.pokeUint32 (0x40C84240, val);			// ANADIG_PLL->PLL_528_CTRL

	val &= ~0x40000000;
	TargetInterface.pokeUint32 (0x40C84240, val);			// ANADIG_PLL->PLL_528_CTRL


	////////// Initialize System PLL2 PFD1 //////////
	val = TargetInterface.peekUint32 (0x40C84270);			// ANADIG_PLL->PLL_528_PFD
	if (((val & 0x8000) != 0) || (((val & 0x3F00) >> 8) != 16))
	{
		var stable = val & 0x4000;

		val |= 0x8000;
		TargetInterface.pokeUint32 (0x40C84270, val);		// ANADIG_PLL->PLL_528_PFD

		AlterRegister (0x40C84270, 0x3F00, 16 << 8);		// ANADIG_PLL->PLL_528_PFD

		val = TargetInterface.peekUint32 (0x40C84250);		// ANADIG_PLL->PLL_528_UPDATE
		val ^= 0x4;
		TargetInterface.pokeUint32 (0x40C84250, val);		// ANADIG_PLL->PLL_528_UPDATE

		AlterRegister (0x40C84270, 0x8000, 0);				// ANADIG_PLL->PLL_528_PFD
		do
		{
			val = TargetInterface.peekUint32 (0x40C84270) & 0x4000;	  // ANADIG_PLL->PLL_528_PFD
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

function Clock_Init_118x () 
{
	////////// Initialize System PLL1 //////////
	AlterRegister (0x44484180, 0, 0x10000); // ANATOP_PllBypass ETHERNET_PLL true
	AlterRegister (0x44484100, 0, 0x2000);  // ANATOP_SysPll1SwEnClk

	// ANATOP_PllConfigure
	TargetInterface.pokeUint32 (0x444841a0, 0x0aaaaaaa);
	TargetInterface.pokeUint32 (0x444841b0, 0x0fffffff);
	AlterRegister (0x44484180, 0x7F, (0x29 & 0x7F) | 0x400000);
	TargetInterface.delay (100);
	AlterRegister (0x44484180, 0, 0x4000 | 0x2000);

	// ANATOP_PllToggleHoldRingOff
	AlterRegister (0x44484180, 0, 0x2000);
	TargetInterface.delay (225);
	AlterRegister (0x44484180, 0x2000, 0);

	// ANATOP_SysPll1WaitStable
	var val = 0;
	do
	{
		val = TargetInterface.peekUint32 (0x44484100);
	} while ((val & 0x20000000) != 0x20000000);

	AlterRegister (0x44484180, 0, 0x8000);    // ANATOP_PllEnableClk
	AlterRegister (0x44484100, 0x4000, 0);    // ANATOP_SysPll1Gate
	AlterRegister (0x44484100, 0, 0x2000000); // ANATOP_SysPll1Div2En
	AlterRegister (0x44484100, 0, 0x4000000); // ANATOP_SysPll1Div5En
	AlterRegister (0x44484180, 0x10000, 0);   // ANATOP_PllBypass ETHERNET_PLL false

	// Set SEMC root clock
	TargetInterface.pokeUint32 (0x44451580, 0x204); // Use sys pll1 divided by 5: 200Mhz
}

function Clock_Restore_117x () 
{
	var reg = CCM_CLOCK_ROOT_M7_CONTROL;	// CCM
	var i = 0;
	while (i < 80)
	{
		TargetInterface.pokeUint32 (reg + (i * 0x80), 0);	// Set CCM->CLOCK_ROOT[0 .. 79].CONTROL = 0
		i++;
	}
}

function DeinitPlls_11xx ()
{
	// set the gated Flag to all PLLs (0 : clock is not gated; 1 : clock is gated)
	var ANADIG_PLL = 0x40C84000;
	TargetInterface.pokeUint32 (ANADIG_PLL + 0x200, 0x40000000);	// ARM PLL	. ANADIG_PLL_Type->PLL_ARM_CTRL
	TargetInterface.pokeUint32 (ANADIG_PLL + 0x240, 0x40000000);	// Sys2 PLL	- ANADIG_PLL_Type->PLL_528_CTRL
	TargetInterface.pokeUint32 (ANADIG_PLL + 0x210, 0x40000000);	// Sys3 PLL	- ANADIG_PLL_Type->PLL_480_CTRL
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
	TargetInterface.message ("### SDRAM_WaitIpCmdDone");
}

function SDRAM_Init ()
{
	var DeviceName = GetProjectPartName ();
	TargetInterface.message ("## SDRAM interface initialize for " + DeviceName);
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
		case "MIMXRT1165_cm7":
		case "MIMXRT1166_cm7":
		case "MIMXRT1171_cm7":
		case "MIMXRT1172_cm7":
		case "MIMXRT1173_cm7":
		case "MIMXRT1175_cm7":
		case "MIMXRT1176_cm7":
			SDRAM_Init_1170 ();
			break;
		case "MIMXRT1181":
		case "MIMXRT1182":
		case "MIMXRT1187_cm33":
		case "MIMXRT1189_cm33":
			SDRAM_Init_1180 ();
			break;
		default:
			TargetInterface.message ("SDRAM_Init - unknown Device: " + DeviceName);
			return;
	}

	TargetInterface.message ("## SDRAM_Init - Done");
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
		IOMUXC_SW_MUX_CTL_PAD = IOMUXC + 0x14;
		IOMUXC_SW_PAD_CTL_PAD = IOMUXC + 0x188;
		PAD_Settings          = 0x000000F1;
	}
	else
	{
		IOMUXC_SW_MUX_CTL_PAD = IOMUXC + 0x14;
		IOMUXC_SW_PAD_CTL_PAD = IOMUXC + 0x204;
		PAD_Settings          = 0x000110F9;
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
	var SEMC                  = 0x400D4000;

	SDRAM_Init_11xx (IOMUXC_SW_MUX_CTL_PAD, IOMUXC_SW_PAD_CTL_PAD, SEMC, 32, 64);
}

function SDRAM_Init_1180 ()
{
	var IOMUXC                = 0x42A10000;
	var IOMUXC_SW_MUX_CTL_PAD = IOMUXC + 0x10;
	var IOMUXC_SW_PAD_CTL_PAD = IOMUXC + 0x258;
	var SEMC                  = 0x42910000;

	SDRAM_Init_11xx (IOMUXC_SW_MUX_CTL_PAD, IOMUXC_SW_PAD_CTL_PAD, SEMC, 16, 32);
}

function SDRAM_Init_11xx (IOMUXC_SW_MUX_CTL_PAD, IOMUXC_SW_PAD_CTL_PAD, SEMC, bits, sizeInMbytes)
{
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
	if (bits == 32)
	{
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
	}

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
	if (bits == 32)
	{
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
	}

	// Configure SDR Controller Registers
	TargetInterface.pokeUint32 (SEMC + 0x00, 0x10000004); // MCR 
	TargetInterface.pokeUint32 (SEMC + 0x08, 0x00030524); // BMCR0								 - 1170: 0x00030524 - 1180: 0x00000081
	TargetInterface.pokeUint32 (SEMC + 0x0C, 0x06030524); // BMCR1								 - 1170: 0x06030524 - 1180: 0x00000081
	switch (sizeInMbytes)
	{
		case 1:
			TargetInterface.pokeUint32 (SEMC + 0x10, 0x80000010); // BR0, 1MB
			break;
		case 2:
			TargetInterface.pokeUint32 (SEMC + 0x10, 0x80000013); // BR0, 2MB
			break;
		case 4:
			TargetInterface.pokeUint32 (SEMC + 0x10, 0x80000015); // BR0, 4MB
			break;
		case 8:
			TargetInterface.pokeUint32 (SEMC + 0x10, 0x80000017); // BR0, 8MB
			break;
		case 16:
			TargetInterface.pokeUint32 (SEMC + 0x10, 0x80000019); // BR0, 16MB
			break;
		case 32:
			TargetInterface.pokeUint32 (SEMC + 0x10, 0x8000001B); // BR0, 32MB
			break;
		case 64:
			TargetInterface.pokeUint32 (SEMC + 0x10, 0x8000001D); // BR0, 64MB
			break;
		case 128:
			TargetInterface.pokeUint32 (SEMC + 0x10, 0x8000001F); // BR0, 128MB
			break;
			
	}
	if (bits == 32)
		TargetInterface.pokeUint32 (SEMC + 0x40, 0x00000F32); // SDRAMCR0, 32bit
	else
		TargetInterface.pokeUint32 (SEMC + 0x40, 0x00000F31); // SDRAMCR0, 16bit
	TargetInterface.pokeUint32 (SEMC + 0x44, 0x00772A22); // SDRAMCR1
	TargetInterface.pokeUint32 (SEMC + 0x48, 0x00010A0D); // SDRAMCR2
	TargetInterface.pokeUint32 (SEMC + 0x4C, 0x21210408); // SDRAMCR3

	TargetInterface.pokeUint32 (SEMC + 0x90, 0x80000000); // IPCR0
	TargetInterface.pokeUint32 (SEMC + 0x94, 0x00000002); // IPCR1
	TargetInterface.pokeUint32 (SEMC + 0x98, 0x00000000); // IPCR2

	TargetInterface.pokeUint32 (SEMC + 0x9C, 0xA55A000F); // IPCMD, SD_CC_IPREA - Precharge all
	SDRAM_WaitIpCmdDone        (SEMC);
	TargetInterface.pokeUint32 (SEMC + 0x9C, 0xA55A000C); // IPCMD, SD_CC_IAF - AutoRefresh
	SDRAM_WaitIpCmdDone        (SEMC);
	TargetInterface.pokeUint32 (SEMC + 0x9C, 0xA55A000C); // IPCMD, SD_CC_IAF - AutoRefresh
	SDRAM_WaitIpCmdDone        (SEMC);
	TargetInterface.pokeUint32 (SEMC + 0xA0, 0x00000033); // IPTXDAT
	TargetInterface.pokeUint32 (SEMC + 0x9C, 0xA55A000A); // IPCMD, SD_CC_IMS - Modeset
	SDRAM_WaitIpCmdDone        (SEMC);

	TargetInterface.pokeUint32 (SEMC + 0x150, 0x00000017); // DCCR - Delay Chain Control Register    - 1170: 0x00000017 - 1180: 0x0000000B
	TargetInterface.pokeUint32 (SEMC + 0x4C,  0x21210409); // SDRAMCR3, enable SDRAM self refresh after initialization done.
}



function AlterRegister (Addr, Clear, Set)
{
	var temp = TargetInterface.peekUint32 (Addr);
	temp &= ~Clear;
	temp |= Set;
	TargetInterface.pokeUint32 (Addr, temp);
}

// Index of the FlexSPI. Depending on the device, can be one or two.
function FlexSPI_GetBaseAddress (FlexSPI)
{
	var FlexSPI1 = 0x402A8000;
	var FlexSPI2 = 0x402A4000;

	var DeviceName = GetProjectPartName ();
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
		case "MIMXRT1165_cm7":
		case "MIMXRT1166_cm7":
		case "MIMXRT1171_cm7":
		case "MIMXRT1172_cm7":
		case "MIMXRT1173_cm7":
		case "MIMXRT1175_cm7":
		case "MIMXRT1176_cm7":
			FlexSPI1 = 0x400CC000;
			FlexSPI2 = 0x400D0000;
			break;
		case "MIMXRT1181":
		case "MIMXRT1182":
		case "MIMXRT1187_cm33":
		case "MIMXRT1189_cm33":
		case "MIMXRT1187_cm7":
		case "MIMXRT1189_cm7":
			FlexSPI1 = 0x525E0000; // Non Secure: 0x425E0000
			FlexSPI2 = 0x545E0000; // Non Secure: 0x445E0000
			break;
		default:
			TargetInterface.message ("FlexSPI_GetBaseAddress - unknown Device: " + DeviceName);
			return;
	}

	var base = 0;
	switch (FlexSPI)
	{
		case 1:
			return FlexSPI1;
		case 2:
			return FlexSPI2;
		default:
			TargetInterface.message ("FlexSPI_GetBaseAddress - Invalid Interface");
	}

	return 0;
}

function FlexSPI_ModuleReset (FlexSPI)
{
	TargetInterface.message ("## FlexSPI_ModuleReset ");
	var base = FlexSPI_GetBaseAddress (FlexSPI);
	var mcr0 = TargetInterface.peekUint32 (base);
	if ((mcr0 & 0x02) == 0)  // Module enabled
	{
		TargetInterface.message ("## FlexSPI_ModuleReset - Module is enabled");
		TargetInterface.pokeUint32 (base, mcr0 | 0x1);
		do
		{
			mcr0 = TargetInterface.peekUint32 (base);
		}
		while ((mcr0 & 0x1) != 0);
	}
	TargetInterface.message ("## FlexSPI_ModuleReset - done");
}

function FlexSPI_WaitBusIdle (base)
{
	TargetInterface.message ("## FlexSPI_WaitBusIdle ");
	var mcr0 = TargetInterface.peekUint32 (base);
	if ((mcr0 & 0x02) == 0)  // Module enabled
	{
		TargetInterface.message ("## FlexSPI_WaitBusIdle - Module is enabled");
		do
		{
			mcr0 = TargetInterface.peekUint32 (base);
		}
		while ((mcr0 & 0x3) != 0x3);
	}
	TargetInterface.message ("## FlexSPI_WaitBusIdle - done");
}

function FlexSPI_Init (FlexSPI)
{
	var CCM = 0x400FC000;
	var CCM_CBCMR  = CCM + 0x18;
	var CCM_CSCMR1 = CCM + 0x1C;
	var CCM_CSCMR2 = CCM + 0x20;

	var CCM_ANALOG = 0x400D8000;
	var CCM_ANALOG_PFD480 = CCM_ANALOG + 0xF0;

	var base = FlexSPI_GetBaseAddress (FlexSPI);

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


function Reset_Loader ()
{
	TargetInterface.message ("## Reset_Loader");
	Reset ();
	TargetInterface.setRegister ("r0", 1);
	TargetInterface.setRegister ("r1", 1);
	TargetInterface.message ("## Reset_Loader - done");
}