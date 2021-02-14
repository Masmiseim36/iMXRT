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

function Connect ()
{
	var DeviceName = GetProjectPartName ();
	TargetInterface.message ("## Connect to " + DeviceName);
	switch (DeviceName)
	{
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

function Reset ()
{
	if (TargetInterface.implementation() == "crossworks_simulator")
		return;

	var DeviceName = GetProjectPartName ();
	TargetInterface.message ("## Reset " + DeviceName);
	switch (DeviceName)
	{
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