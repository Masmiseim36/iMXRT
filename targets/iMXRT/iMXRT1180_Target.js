/*****************************************************************************
 * iMXRT1180_Target.js
 *
 * CrossWorks (CrossStudio / CrossConnect) target script for the NXP i.MX RT1180
 * family (Cortex-M33 and Cortex-M7).
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


// Called from Connect ()
function Connect_RT1180 ()
{
	// MIMXRT118x doesn't like attempt to access AP5
	TargetInterface.setDebugInterfaceProperty ("valid_ap_num", (1<<0)|(1<<1)|(1<<2)|(1<<3)|(1<<4)|(1<<6));
	TargetInterface.setDebugInterfaceProperty ("component_base", 0xe000e000); // SCS
	TargetInterface.setDebugInterfaceProperty ("component_base", 0xe0001000); // DWT
	TargetInterface.setDebugInterfaceProperty ("component_base", 0xe0002000); // FPB
	TargetInterface.setDebugInterfaceProperty ("component_base", 0xe0000000); // ITM
	TargetInterface.setDebugInterfaceProperty ("component_base", 0xe0041000); // ETM
	//TargetInterface.setDebugInterfaceProperty ("component_base", 0xe0042000); // CTI
	TargetInterface.setDebugInterfaceProperty ("component_base", 0xe0043000); // CSTF
	TargetInterface.setDebugInterfaceProperty ("use_adiv5_APB",  0x51020000, 0x50000); // APB access
	TargetInterface.setDebugInterfaceProperty ("component_base", 0x51020000); // CS_SWO
	TargetInterface.setDebugInterfaceProperty ("component_base", 0x51030000); // CS_ETF
	//TargetInterface.setDebugInterfaceProperty ("component_base", 0x51040000); // CS_ETR
	//TargetInterface.setDebugInterfaceProperty ("component_base", 0x51050000); // CS_CT0
	//TargetInterface.setDebugInterfaceProperty ("component_base", 0x51060000); // CS_CT1
}


// Called from GetPartName ()
function GetPartName_RT1180 (DeviceName)
{
	TargetInterface.setDebugInterfaceProperty ("set_adiv5_AHB_ap_num", 3, 0x40000000, 0x00000000); // M33 Clear bit 30 of csw for secure access
	TargetInterface.resetDebugInterface ();

	// This function is also called when attaching to a running application: don't change the state of
	// the target here, as long as the M7 is already released (= nothing to do).
	// The M7 is only woken up, when it is the target. There are devices without an M7 (MIMXRT1181/1182)
	// and an M33 application may want to start the M7 by itself.
	if (DeviceName.indexOf ("_cm7") != -1)
	{
		if (IsReleased_118x_M7 () == 0)
		{
			// Bring the M33 into a defined state (like the NXP script RT1180_connect_M33_wake_M7.scp)
			TargetInterface.stop (1000);
			RunBootLoader_118x ();
			Init_118x_M33 ();	// The M33 TCM is the source of the DMA transfers to the M7 TCMs
			Release_118x_M7 (); // Enable M7 core
		}
		TargetInterface.setDebugInterfaceProperty ("set_adiv5_AHB_ap_num", 2);
	}
}


// Called from Reset ()
function Reset_RT1180 (DeviceName)
{
	if (DeviceName.indexOf ("_cm7") != -1)
	{
		TargetInterface.stop (1000);
		TargetInterface.pokeUint32 (0xE000ED14, 0);	// SCB->CCR - Disable the caches of the M7
		// Disable pending interrupts
		FillMemory (0xE000E180, 0x40,    0xFFFFFFFF); // NVIC_ICER
		FillMemory (0xE000E280, 0x40,    0xFFFFFFFF); // NVIC_ICPR
	}
	else
	{
		// M33 target: a M7 that is not released yet is not touched (not available on every device, may be started
		// by the application). The reset below holds the M7 in reset again, so a M7 that was released before (e.g.
		// by GetPartName () of a M7 debug session) is released again. Otherwise the following M7 accesses time out.
		// This is a reset requested by the debugger, so the state of the target may be changed here
		var m7Released = IsReleased_118x_M7 ();
		TargetInterface.stop (1000);
		RunBootLoader_118x ();	// Defined state: the application in the flash is not executed
		TargetInterface.resetAndStop (1000);
		Init_118x_M33 ();
		if (m7Released != 0 && IsReleased_118x_M7 () == 0)
		{
			TargetInterface.message ("## M7 was held in reset by the M33 reset, releasing it again");
			Release_118x_M7 ();
		}
	}
}


// Returns 1 if the M7 is released (SRC->SCR.BT_RELEASE_M7), otherwise 0
function IsReleased_118x_M7 ()
{
	return TargetInterface.peekUint32 (0x54460010) & 0x1;
}


function Release_118x_M7 ()
{
	var ItcmBase  = 0x00000000 // M7 ITCM
	var ItcmAlias = 0x303c0000 // M7 ITCM Aliased to M33

	TargetInterface.message ("************* Begin Operations to Enable CM7 ***********************");

	// Clock Preparation
	TargetInterface.message ("******** Prepare Clock *********");
	TargetInterface.pokeUint32 (0x54484310, 0x007901F2);	// ANADIG_OSC->OSC_RC24M_CTRL
	TargetInterface.pokeUint32 (0x54484320, 0x40000014);	// ANADIG_OSC->OSC_24M_CTRL
	TargetInterface.pokeUint32 (0x54450080, 0x0);			// CCM->CLOCK_ROOT[1].CONTROL
	TargetInterface.delay (50);
	TargetInterface.pokeUint32 (0x54484680, 0x105);			// PHY_LDO->CTRL0.RW - LinReg 1V
	TargetInterface.delay (50);
	TargetInterface.pokeUint32 (0x54484710, 0x70);			// ANADIG_PMU->PMU_REF_CTRL
	TargetInterface.pokeUint32 (0x54484000, 0x400020a6);	// ANADIG_PLL->ARM_PLL_CTRL - ARM_PLL_GATE, POWER_UP
	TargetInterface.delay (50);
	// check 54484000
	TargetInterface.pokeUint32 (0x54484000, 0x000060a6);	// ANADIG_PLL->ARM_PLL_CTRL - ARM_PLL_GATE, POWER_UP, ENABLE

	// DCDC_SetVDD1P0BuckModeTargetVoltage(DCDC, kDCDC_1P0Target1P1V);
	TargetInterface.pokeUint32 (0x5452000C, 0x980000);		// DCDC->REG3
	TargetInterface.pokeUint32 (0x54520024, 0x100C14);		// DCDC->TRG_SW_0

	// M7 Clk
	TargetInterface.pokeUint32 (0x54450000, 0x201);			// CCM->CLOCK_ROOT0_CONTROL

	TargetInterface.pokeUint32 (0x54458000, 0x1);			// CCM->LPCG[0].DIRECT - ON
	TargetInterface.delay (50);
  
	// VTOR
	TargetInterface.pokeUint32 (0x544F0080, ((ItcmBase >> 7) << 7) | 0x10); // BLK_CTRL_S_AON->M7_CFG = VTOR | WAIT

	// Release CM7
	TargetInterface.pokeUint32 (0x54460010, 0x1);			// SRC_GENERAL_REG->SCR - Release CM7

	// DMA initialization
	TargetInterface.message ("******** DMA operation *********");
	FillMemory_DMA_118x (0x303C0000, 0x20000, 0);	// M7 ITCM
	FillMemory_DMA_118x (0x303E0000, 0x20000, 0);
	FillMemory_DMA_118x (0x30400000, 0x20000, 0);	// M7 DTCM
	FillMemory_DMA_118x (0x30420000, 0x20000, 0);

	// Making Landing Zone
	TargetInterface.pokeUint32 (ItcmAlias + 0x0, ItcmBase + 0x2000);
	TargetInterface.pokeUint32 (ItcmAlias + 0x4, ItcmBase + 0x9);
	TargetInterface.pokeUint32 (ItcmAlias + 0x8, 0xE7FEE7FE);
	TargetInterface.pokeUint32 (ItcmAlias + 0xC, 0xE7FEE7FE);

	TargetInterface.delay (200);

	// Trigger S401 - EdgeLock APC Request
	TargetInterface.message ("******** S401 Trigger *********");
	// Same Message Unit as in the NXP connect script RT1180_connect_M33_wake_M7.scp
	TargetInterface.pokeUint32 (0x57520200, 0x17d20106); // MU_APPS_S3MUA->TR[0]
	TargetInterface.delay (200);
	var resp1 = TargetInterface.peekUint32 (0x57520280); // MU_APPS_S3MUA->RR[0]
	var resp2 = TargetInterface.peekUint32 (0x57520284); // MU_APPS_S3MUA->RR[1]
	TargetInterface.message ("ELE response: " + Hex (resp1) + ", " + Hex (resp2));

	// Deassert CM7 Wait
	TargetInterface.message ("******** Kickoff CM7 *********");
	TargetInterface.pokeUint32 (0x544F0080, (ItcmBase >> 7) << 7); // BLK_CTRL_S_AON->M7_CFG = VTOR
	TargetInterface.delay (200);
}

// Fill a memory area with 64 bit DMA transfers (ECC memories like the TCMs need 64 bit writes)
// The pattern is taken from an 8 byte scratch area in the M33 D-TCM (0x20000000)
// address and size have to be 8 byte aligned
function FillMemory_DMA_118x (address, size, value)
{
	TargetInterface.pokeUint32 (0x20000000, value);			// Scratch area, source of the DMA transfer
	TargetInterface.pokeUint32 (0x20000004, value);
	TargetInterface.pokeUint32 (0x52010020, 0x20200000);	// DMA4->TCD[0].SADDR - M33 D-TCM as seen by the DMA
	TargetInterface.pokeUint32 (0x52010030, address);		// DMA4->TCD[0].DADDR
	TargetInterface.pokeUint32 (0x52010028, size);			// DMA4->TCD[0].NBYTES_MLOFFNO
	TargetInterface.pokeUint16 (0x52010036, 0x1);			// DMA4->TCD[0].ELINKNO
	TargetInterface.pokeUint16 (0x5201003E, 0x1);			// DMA4->TCD[0].BITER_ELINKNO
	TargetInterface.pokeUint16 (0x52010026, 0x0303);		// DMA4->TCD[0].ATTR
	TargetInterface.pokeUint16 (0x52010024, 0x0);			// DMA4->TCD[0].SOFF
	TargetInterface.pokeUint16 (0x52010034, 0x8);			// DMA4->TCD[0].DOFF
	TargetInterface.pokeUint32 (0x52010000, 0x7);			// DMA4->TDC[0].CH_CSR
	TargetInterface.pokeUint16 (0x5201003C, 0x8);			// DMA4->TCD[0].CSR
	TargetInterface.pokeUint16 (0x5201003C, 0x9);			// DMA4->TCD[0].CSR - Start
	TargetInterface.delay (50);
	TargetInterface.pokeUint32 (0x52010000, 0x40000006);	// DMA4->TDC[0].CH_CSR - Clear done flag
	TargetInterface.pokeUint32 (0x52010000, 0x40000000);
}

// Invalidate and disable the caches of the M33 (XCACHE_PC and XCACHE_PS)
function XCache_Disable_118x ()
{
	TargetInterface.pokeUint32 (0x54400000, 0x85000000);	// XCACHE_PC->CCR
	TargetInterface.pokeUint32 (0x54400800, 0x85000000);	// XCACHE_PS->CCR
}

// Initialize the M33: Caches, NVIC and the ECC protected TCMs
function Init_118x_M33 ()
{
	TargetInterface.message ("## Initialize CM33");
	XCache_Disable_118x ();

	// Disable pending interrupts
	FillMemory (0xE000E180, 0x40, 0xFFFFFFFF);	// NVIC_ICER
	FillMemory (0xE000E280, 0x40, 0xFFFFFFFF);	// NVIC_ICPR

	// Initialize the TCMs of the M33 by DMA, the addresses are the ones seen by the DMA
	FillMemory_DMA_118x (0x201E0000, 0x20000, 0);			// M33 ITCM
	FillMemory_DMA_118x (0x20200008, 0x20000 - 0x8, 0);		// M33 DTCM (without the scratch area)
}

// Enter the serial downloader mode of the boot ROM by calling the runBootLoader() function of the ROM API
// The application in the flash is not executed then and the controller is in a defined state
// The controller has to be halted when calling this function
// Like in the NXP script RT1180_connect_M33_wake_M7.scp
function RunBootLoader_118x ()
{
	// Already in the ROM: the serial downloader (or the ROM boot) is already active
	var pc = TargetInterface.getRegister ("pc");
	if ((pc & 0xFFFF0000) == 0x10000000)
	{
		TargetInterface.message ("## runBootLoader skipped - core is already running in the ROM");
		return;
	}

	TargetInterface.message ("## runBootLoader to enter serial downloader mode");

	// The NXP script searches for a writable scratch area as the ELE may lock some memory areas.
	// A failing memory access can't be caught in this script dialect, so only the first location is used
	var scratch = 0x2001FFF0;	// End of the M33 DTCM

	// Get the address of runBootLoader() via the root pointer of the ROM API
	var apiTree = TargetInterface.peekUint32 (0x1000001C);
	var runBootLoader = TargetInterface.peekUint32 (apiTree);

	// runBootLoader() gets a pointer to its argument in R0
	TargetInterface.pokeUint32 (scratch, 0xEB100000);
	TargetInterface.setRegister ("r0", scratch);
	TargetInterface.setRegister ("pc", runBootLoader | 1);	// Thumb bit as for runFromAddress ()
	TargetInterface.go ();
	TargetInterface.delay (100);

	// Reconnect and halt the core in the ROM
	TargetInterface.resetDebugInterface ();
	TargetInterface.stop (1000);
}

function Clock_Init_118x () 
{
	////////// Initialize System PLL1 //////////
	ModifyRegister (0x44484180, 0, 0x10000); // ANATOP_PllBypass ETHERNET_PLL true
	ModifyRegister (0x44484100, 0, 0x2000);  // ANATOP_SysPll1SwEnClk

	// ANATOP_PllConfigure
	TargetInterface.pokeUint32 (0x444841a0, 0x0aaaaaaa);
	TargetInterface.pokeUint32 (0x444841b0, 0x0fffffff);
	ModifyRegister (0x44484180, 0x7F, (0x29 & 0x7F) | 0x400000);
	TargetInterface.delay (100);
	ModifyRegister (0x44484180, 0, 0x4000 | 0x2000);

	// ANATOP_PllToggleHoldRingOff
	ModifyRegister (0x44484180, 0, 0x2000);
	TargetInterface.delay (225);
	ModifyRegister (0x44484180, 0x2000, 0);

	// ANATOP_SysPll1WaitStable
	var val = 0;
	do
	{
		val = TargetInterface.peekUint32 (0x44484100);
	} while ((val & 0x20000000) != 0x20000000);

	ModifyRegister (0x44484180, 0, 0x8000);    // ANATOP_PllEnableClk
	ModifyRegister (0x44484100, 0x4000, 0);    // ANATOP_SysPll1Gate
	ModifyRegister (0x44484100, 0, 0x2000000); // ANATOP_SysPll1Div2En
	ModifyRegister (0x44484100, 0, 0x4000000); // ANATOP_SysPll1Div5En
	ModifyRegister (0x44484180, 0x10000, 0);   // ANATOP_PllBypass ETHERNET_PLL false

	// Set SEMC root clock
	TargetInterface.pokeUint32 (0x44451580, 0x204); // Use sys pll1 divided by 5: 200Mhz
}

function SDRAM_Init_1180 ()
{
	var IOMUXC                = 0x42A10000;
	var IOMUXC_SW_MUX_CTL_PAD = IOMUXC + 0x10;
	var IOMUXC_SW_PAD_CTL_PAD = IOMUXC + 0x258;
	var SEMC                  = 0x42910000;

	SDRAM_Init_11xx (IOMUXC_SW_MUX_CTL_PAD, IOMUXC_SW_PAD_CTL_PAD, SEMC, 16, 32);
}
