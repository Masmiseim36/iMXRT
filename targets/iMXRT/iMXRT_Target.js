/*****************************************************************************
 * Copyright (c) 2016 Rowley Associates Limited.                             *
 *                                                                           *
 * This file may be distributed under the terms of the License Agreement     *
 * provided with this software.                                              *
 *                                                                           *
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE   *
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. *
 *                                                                           *
 *****************************************************************************/

function Connect()
{
}

function Reset()
{
  TargetInterface.resetDebugInterface();
  TargetInterface.resetAndStop(1000);
  if (TargetInterface.implementation() == "crossworks_simulator")
    return;

  ocotp_base = 0x401F4000;
  ocotp_fuse_bank0_base = ocotp_base + 0x400;
  dcdc_base = 0x40080000;
  
  dcdc_trim_loaded = 0;

  reg = TargetInterface.peekWord(ocotp_fuse_bank0_base + 0x90);
  if (reg & (1<<10))
    {
      // DCDC: REG0->VBG_TRM
      trim_value = (reg & (0x1F << 11)) >> 11; 
      reg = (TargetInterface.peekWord (dcdc_base + 0x4) & ~(0x1F << 24)) | (trim_value << 24);
      TargetInterface.pokeWord(dcdc_base + 0x4, reg);
      dcdc_trim_loaded = 1;
    }

  reg = TargetInterface.peekWord(ocotp_fuse_bank0_base + 0x80);
  if (reg & (1<<30))
    {
      index = (reg & (3 << 28)) >> 28;
      if (index < 4)
        {
          // DCDC: REG3->TRG 
          reg = (TargetInterface.peekWord (dcdc_base + 0xC) & ~(0x1F)) | ((0xF + index));
          TargetInterface.pokeWord(dcdc_base + 0xC, reg);
          dcdc_trim_loaded = 1;
        }
    }

  if (dcdc_trim_loaded)
    {
      // delay about 400us till dcdc is stable.
      TargetInterface.delay (1);
    }
}

function GetPartName()
{    
  var PART = "";
  return PART;
}

function MatchPartName(name)
{
  var partName = GetPartName();

  if (partName == "")
    return false;

  return partName.substring(0, 6) == name.substring(0, 6);

}

function EnableTrace(traceInterfaceType)
{
	// TODO: Enable trace
}

function GetProjectPartName ()
{
	var TargetFullName = TargetInterface.getProjectProperty ("Target");
	var TargetShort = TargetFullName.substring (0, 10);

	switch (TargetFullName.slice(-4))
	{
		case "_cm7":
			TargetShort += '_cm7';
			break;
		case "_cm4":
			TargetShort += '_cm4';
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
		case "MIMXRT1015":
		case "MIMXRT1021":
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

function ClockGate_EnableAll ()
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


function Clock_Init_1021 () 
{
	TargetInterface.message ("Clock_Init_1021");
	ClockGate_EnableAll ();	// Enable all clocks

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
	reg = TargetInterface.peekUint32 (0xE000ED94);
	reg &= ~0x1;
	TargetInterface.pokeUint32 (0xE000ED94, reg);
	TargetInterface.message ("Clock_Init_1021 - Done");
}

function Clock_Init_105x () 
{
	TargetInterface.message ("Clock_Init_105x");
	ClockGate_EnableAll ();	// Enable all clocks

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


function SDRAM_WaitIpCmdDone () 
{
	var reg;
	do
	{
		reg = TargetInterface.peekUint32 (0x402F003C);
	}
	while ((reg & 0x3) == 0);

	TargetInterface.pokeUint32 (0x402F003C,0x00000003); // clear IPCMDERR and IPCMDDONE bits
	TargetInterface.message ("SDRAM_WaitIpCmdDone");
}

function SDRAM_Init ()
{
	var DeviceName = GetProjectPartName ();
	TargetInterface.message ("SDRAM interface initialize for " + DeviceName);

	// Config IOMUX for SDRAM
	TargetInterface.pokeUint32 (0x401F8014, 0x00000000); // EMC_00
	TargetInterface.pokeUint32 (0x401F8018, 0x00000000); // EMC_01
	TargetInterface.pokeUint32 (0x401F801C, 0x00000000); // EMC_02
	TargetInterface.pokeUint32 (0x401F8020, 0x00000000); // EMC_03
	TargetInterface.pokeUint32 (0x401F8024, 0x00000000); // EMC_04
	TargetInterface.pokeUint32 (0x401F8028, 0x00000000); // EMC_05
	TargetInterface.pokeUint32 (0x401F802C, 0x00000000); // EMC_06
	TargetInterface.pokeUint32 (0x401F8030, 0x00000000); // EMC_07
	TargetInterface.pokeUint32 (0x401F8034, 0x00000000); // EMC_08
	TargetInterface.pokeUint32 (0x401F8038, 0x00000000); // EMC_09
	TargetInterface.pokeUint32 (0x401F803C, 0x00000000); // EMC_10
	TargetInterface.pokeUint32 (0x401F8040, 0x00000000); // EMC_11
	TargetInterface.pokeUint32 (0x401F8044, 0x00000000); // EMC_12
	TargetInterface.pokeUint32 (0x401F8048, 0x00000000); // EMC_13
	TargetInterface.pokeUint32 (0x401F804C, 0x00000000); // EMC_14
	TargetInterface.pokeUint32 (0x401F8050, 0x00000000); // EMC_15
	TargetInterface.pokeUint32 (0x401F8054, 0x00000000); // EMC_16
	TargetInterface.pokeUint32 (0x401F8058, 0x00000000); // EMC_17
	TargetInterface.pokeUint32 (0x401F805C, 0x00000000); // EMC_18
	TargetInterface.pokeUint32 (0x401F8060, 0x00000000); // EMC_19
	TargetInterface.pokeUint32 (0x401F8064, 0x00000000); // EMC_20
	TargetInterface.pokeUint32 (0x401F8068, 0x00000000); // EMC_21
	TargetInterface.pokeUint32 (0x401F806C, 0x00000000); // EMC_22
	TargetInterface.pokeUint32 (0x401F8070, 0x00000000); // EMC_23
	TargetInterface.pokeUint32 (0x401F8074, 0x00000000); // EMC_24
	TargetInterface.pokeUint32 (0x401F8078, 0x00000000); // EMC_25
	TargetInterface.pokeUint32 (0x401F807C, 0x00000000); // EMC_26
	TargetInterface.pokeUint32 (0x401F8080, 0x00000000); // EMC_27
	if (DeviceName == "MIMXRT1021")
		TargetInterface.pokeUint32 (0x401F8084, 0x00000010); // EMC_28, DQS PIN, enable SION
	else
		TargetInterface.pokeUint32 (0x401F8084, 0x00000000); // EMC_28
	TargetInterface.pokeUint32 (0x401F8088, 0x00000000); // EMC_29
	TargetInterface.pokeUint32 (0x401F808C, 0x00000000); // EMC_30
	TargetInterface.pokeUint32 (0x401F8090, 0x00000000); // EMC_31
	TargetInterface.pokeUint32 (0x401F8094, 0x00000000); // EMC_32
	TargetInterface.pokeUint32 (0x401F8098, 0x00000000); // EMC_33
	TargetInterface.pokeUint32 (0x401F809C, 0x00000000); // EMC_34
	TargetInterface.pokeUint32 (0x401F80A0, 0x00000000); // EMC_35
	TargetInterface.pokeUint32 (0x401F80A4, 0x00000000); // EMC_36
	TargetInterface.pokeUint32 (0x401F80A8, 0x00000000); // EMC_37
	TargetInterface.pokeUint32 (0x401F80AC, 0x00000000); // EMC_38
	if (DeviceName == "MIMXRT1021")
		TargetInterface.pokeUint32 (0x401F80B0, 0x00000000); // EMC_39, DQS PIN, enable SION
	else
		TargetInterface.pokeUint32 (0x401F80B0, 0x00000010); // EMC_39
	TargetInterface.pokeUint32 (0x401F80B4, 0x00000000); // EMC_40
	TargetInterface.pokeUint32 (0x401F80B8, 0x00000000); // EMC_41

	// PAD ctrl
	// drive strength = 0x7 to increase drive strength
	// otherwise the data7 bit may fail.
	TargetInterface.pokeUint32 (0x401F8204, 0x000110F9); // EMC_00
	TargetInterface.pokeUint32 (0x401F8208, 0x000110F9); // EMC_01
	TargetInterface.pokeUint32 (0x401F820C, 0x000110F9); // EMC_02
	TargetInterface.pokeUint32 (0x401F8210, 0x000110F9); // EMC_03
	TargetInterface.pokeUint32 (0x401F8214, 0x000110F9); // EMC_04
	TargetInterface.pokeUint32 (0x401F8218, 0x000110F9); // EMC_05
	TargetInterface.pokeUint32 (0x401F821C, 0x000110F9); // EMC_06
	TargetInterface.pokeUint32 (0x401F8220, 0x000110F9); // EMC_07
	TargetInterface.pokeUint32 (0x401F8224, 0x000110F9); // EMC_08
	TargetInterface.pokeUint32 (0x401F8228, 0x000110F9); // EMC_09
	TargetInterface.pokeUint32 (0x401F822C, 0x000110F9); // EMC_10
	TargetInterface.pokeUint32 (0x401F8230, 0x000110F9); // EMC_11
	TargetInterface.pokeUint32 (0x401F8234, 0x000110F9); // EMC_12
	TargetInterface.pokeUint32 (0x401F8238, 0x000110F9); // EMC_13
	TargetInterface.pokeUint32 (0x401F823C, 0x000110F9); // EMC_14
	TargetInterface.pokeUint32 (0x401F8240, 0x000110F9); // EMC_15
	TargetInterface.pokeUint32 (0x401F8244, 0x000110F9); // EMC_16
	TargetInterface.pokeUint32 (0x401F8248, 0x000110F9); // EMC_17
	TargetInterface.pokeUint32 (0x401F824C, 0x000110F9); // EMC_18
	TargetInterface.pokeUint32 (0x401F8250, 0x000110F9); // EMC_19
	TargetInterface.pokeUint32 (0x401F8254, 0x000110F9); // EMC_20
	TargetInterface.pokeUint32 (0x401F8258, 0x000110F9); // EMC_21
	TargetInterface.pokeUint32 (0x401F825C, 0x000110F9); // EMC_22
	TargetInterface.pokeUint32 (0x401F8260, 0x000110F9); // EMC_23
	TargetInterface.pokeUint32 (0x401F8264, 0x000110F9); // EMC_24
	TargetInterface.pokeUint32 (0x401F8268, 0x000110F9); // EMC_25
	TargetInterface.pokeUint32 (0x401F826C, 0x000110F9); // EMC_26
	TargetInterface.pokeUint32 (0x401F8270, 0x000110F9); // EMC_27
	TargetInterface.pokeUint32 (0x401F8274, 0x000110F9); // EMC_28
	TargetInterface.pokeUint32 (0x401F8278, 0x000110F9); // EMC_29
	TargetInterface.pokeUint32 (0x401F827C, 0x000110F9); // EMC_30
	TargetInterface.pokeUint32 (0x401F8280, 0x000110F9); // EMC_31
	TargetInterface.pokeUint32 (0x401F8284, 0x000110F9); // EMC_32
	TargetInterface.pokeUint32 (0x401F8288, 0x000110F9); // EMC_33
	TargetInterface.pokeUint32 (0x401F828C, 0x000110F9); // EMC_34
	TargetInterface.pokeUint32 (0x401F8290, 0x000110F9); // EMC_35
	TargetInterface.pokeUint32 (0x401F8294, 0x000110F9); // EMC_36
	TargetInterface.pokeUint32 (0x401F8298, 0x000110F9); // EMC_37
	TargetInterface.pokeUint32 (0x401F829C, 0x000110F9); // EMC_38
	TargetInterface.pokeUint32 (0x401F82A0, 0x000110F9); // EMC_39
	TargetInterface.pokeUint32 (0x401F82A4, 0x000110F9); // EMC_40
	TargetInterface.pokeUint32 (0x401F82A8, 0x000110F9); // EMC_41

	// Config SDR Controller Registers
	TargetInterface.message ("SDR Controller Registers");
	TargetInterface.pokeUint32 (0x402F0000, 0x10000004); // MCR
	TargetInterface.pokeUint32 (0x402F0008, 0x00030524); // BMCR0
	TargetInterface.pokeUint32 (0x402F000C, 0x06030524); // BMCR1
	TargetInterface.pokeUint32 (0x402F0010, 0x8000001B); // BR0, 32MB
	TargetInterface.pokeUint32 (0x402F0014, 0x8200001B); // BR1, 32MB
	TargetInterface.pokeUint32 (0x402F0018, 0x8400001B); // BR2, 32MB
	TargetInterface.pokeUint32 (0x402F001C, 0x8600001B); // BR3, 32MB
	TargetInterface.pokeUint32 (0x402F0020, 0x90000021); // BR4,
	TargetInterface.pokeUint32 (0x402F0024, 0xA0000019); // BR5,
	TargetInterface.pokeUint32 (0x402F0028, 0xA8000017); // BR6,
	TargetInterface.pokeUint32 (0x402F002C, 0xA900001B); // BR7,
	TargetInterface.pokeUint32 (0x402F0030, 0x00000021); // BR8,
	TargetInterface.pokeUint32 (0x402F0004, 0x000079A8); //IOCR,SEMC_CCSX0 as NOR CE, SEMC_CSX1 as PSRAM CE, SEMC_CSX2 as NAND CE, SEMC_CSX3 as DBI CE.

	TargetInterface.pokeUint32 (0x402F0040, 0x00000F31); // SDRAMCR0
	TargetInterface.pokeUint32 (0x402F0044, 0x00652922); // SDRAMCR1
	TargetInterface.pokeUint32 (0x402F0048, 0x00010920); // SDRAMCR2
	TargetInterface.pokeUint32 (0x402F004C, 0x50210A09); // SDRAMCR3

	TargetInterface.pokeUint32 (0x402F0040, 0x00000F31); // SDRAMCR0
	TargetInterface.pokeUint32 (0x402F0044, 0x00652922); // SDRAMCR1
	TargetInterface.pokeUint32 (0x402F0048, 0x00010920); // SDRAMCR2
	TargetInterface.pokeUint32 (0x402F004C, 0x50210A09); // SDRAMCR3

	TargetInterface.pokeUint32 (0x402F0080, 0x00000021); // DBICR0
	TargetInterface.pokeUint32 (0x402F0084, 0x00888888); // DBICR1
	TargetInterface.pokeUint32 (0x402F0094, 0x00000002); // IPCR1
	TargetInterface.pokeUint32 (0x402F0098, 0x00000000); // IPCR2

	TargetInterface.pokeUint32 (0x402F0090, 0x80000000); // IPCR0
	TargetInterface.pokeUint32 (0x402F009C, 0xA55A000F); // IPCMD, SD_CC_IPREA
	SDRAM_WaitIpCmdDone();
	TargetInterface.pokeUint32 (0x402F0090, 0x80000000); // IPCR0
	TargetInterface.pokeUint32 (0x402F009C, 0xA55A000C); // SD_CC_IAF
	SDRAM_WaitIpCmdDone();
	TargetInterface.pokeUint32 (0x402F0090, 0x80000000); // IPCR0
	TargetInterface.pokeUint32 (0x402F009C, 0xA55A000C); // SD_CC_IAF
	SDRAM_WaitIpCmdDone();
	TargetInterface.pokeUint32 (0x402F00A0, 0x00000033); // IPTXDAT
	TargetInterface.pokeUint32 (0x402F0090, 0x80000000); // IPCR0
	TargetInterface.pokeUint32 (0x402F009C, 0xA55A000A); // SD_CC_IMS
	SDRAM_WaitIpCmdDone();
	TargetInterface.pokeUint32 (0x402F004C, 0x50210A09); // enable sdram self refresh again after initialization done.
	TargetInterface.message ("SDRAM_Init - Done");
}

function AlterRegister (Addr, Clear, Set)
{
	var temp = TargetInterface.peekWord (Addr);
	temp &= ~Clear;
	temp |= Set;
	TargetInterface.pokeUint32 (Addr, temp);
}

function FLEXSPI_Init (FlexSPI)
{
	var FlexSPI1 = 0x402A8000;
	var FlexSPI2 = 0x402A4000;
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
	var CCM_CBCMR = CCM + 0x18;
	var CCM_CSCMR1 = CCM + 0x1C;
	var CCM_CSCMR2 = CCM + 0x20;

	var CCM_ANALOG = 0x400D8000;
	var CCM_ANALOG_PFD480 = CCM_ANALOG + 0xF0;

	// Set flexspi root clock to 166MHZ.
	AlterRegister (CCM_ANALOG_PFD480, 0xBF, 0x80);	// Disable the clock output first.
	AlterRegister (CCM_ANALOG_PFD480, 0xBF, 26);	// Set the new value and enable output.

	// Choose PLL3 PFD0 clock as flexspi source clock and set internal clock 83 Megaherz
	AlterRegister (CCM_CSCMR1, 0x60000000 | 0x3800000, (3 << 29) | (3 << 23));

	var FlexSPI_MCR0        = base + 0x00;
	var FlexSPI_MCR1        = base + 0x04;
	var FlexSPI_MCR2        = base + 0x08;
	var FlexSPI_AHBCR       = base + 0x0C;
	var FlexSPI_AHBRXBUFCR0 = base + 0x20;
	var FlexSPI_IPRXFCR     = base + 0xD8;
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