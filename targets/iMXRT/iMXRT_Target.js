/*****************************************************************************
 * Copyright (c) 2016-2022 Rowley Associates Limited.                        *
 *                                                                           *
 * This file may be distributed under the terms of the License Agreement     *
 * provided with this software.                                              *
 *                                                                           *
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE   *
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. *
 *                                                                           *
 *****************************************************************************/

function IsArmdInterface()
{
  var implementation = TargetInterface.implementation();
  return implementation == "cmsis-dap" || implementation == "crossworks_hardware";
}

var CCM = 0x40CC0000;
var CCM_CLOCK_ROOT_M7_CONTROL = CCM + (0 * 0x80) + 0;
var CCM_CLOCK_ROOT_M4_CONTROL = CCM + (1 * 0x80) + 0;

var IOMUXC_LPSR_GPR = 0x40C0C000;
var IOMUXC_LPSR_GPR0 = IOMUXC_LPSR_GPR + 0x0;
var IOMUXC_LPSR_GPR1 = IOMUXC_LPSR_GPR + 0x4;

var SRC              = 0x40C04000;
var SRC_SCR          = SRC;
var SRC_SRMR         = SRC + 0x4;
var SRC_CTRL_M7CORE  = SRC + 0x2A4;
var SRC_STAT_M7CORE  = SRC + 0x2B0;
var SRC_CTRL_M4CORE  = SRC + 0x284;
var SRC_STAT_M4CORE  = SRC + 0x290;

function EnableCM4()
{
  if (!IsArmdInterface())
    return;
  if ((TargetInterface.peekUint32 (SRC_SCR) & 0x1)) // M4 already running
    {
      TargetInterface.setDebugInterfaceProperty("set_adiv5_AHB_ap_num", 1);
      return;
    }

  // Start CM4 here
  // 0x2021FF00: b .
  var start = 0x2021FF00;
  TargetInterface.pokeUint32 (start, 0xE7FEE7FE);
  TargetInterface.pokeUint32 (start+4, 0x2021FF01);  
  TargetInterface.pokeUint32 (IOMUXC_LPSR_GPR0, start & 0xFFFF);
  TargetInterface.pokeUint32 (IOMUXC_LPSR_GPR1, (start >> 16) & 0xFFFF);

  // Set m4_clk_root to OSC_RC_400M / 2: CLOCK_ROOT1 = mux(2), div(1)
  //TargetInterface.pokeUint32 (CCM_CLOCK_ROOT_M4_CONTROL, 0x201);

  // Save current reset SRMR and prevent M4 SW reset affecting the system
  var srmr = TargetInterface.peekUint32 (SRC_SRMR); 
  TargetInterface.pokeUint32 (SRC_SRMR, 0x00000C00); // don't system reset from M4 reset
  TargetInterface.pokeUint32 (SRC_CTRL_M4CORE, 0x1); // reset M4
  TargetInterface.pokeUint32 (SRC_SRMR, srmr);
 
  if ((TargetInterface.peekUint32 (SRC_SCR) & 0x1) == 0) 
    TargetInterface.pokeUint32 (SRC_SCR, scr | 1);
  TargetInterface.setDebugInterfaceProperty("set_adiv5_AHB_ap_num", 1);
}

function Connect()
{
}

function GetPartName()
{
  if (TargetInterface.getProjectProperty("arm_core_type")=="Cortex-M4")
    {
      EnableCM4();
    }
}

function MatchPartName(name)
{
  return true;
}

function Reset()
{
  if (!IsArmdInterface())
    {
      TargetInterface.resetAndStop(1000);
      return;
    }  
  if (TargetInterface.getProjectProperty("Target").indexOf("MIMXRT11")==0)
    {      
      if (TargetInterface.getProjectProperty("arm_core_type")=="Cortex-M4")
        {
          TargetInterface.stop();
          TargetInterface.pokeUint32 (SRC_CTRL_M4CORE, 0x1);
          while (TargetInterface.peekUint32 (SRC_STAT_M4CORE) & 1);
          TargetInterface.stop();
        }
      else
        {
          TargetInterface.resetAndStop(1000);
          TargetInterface.pokeUint32(SRC_CTRL_M7CORE, 1);
          TargetInterface.delay(1);
          while (TargetInterface.peekUint32 (SRC_STAT_M7CORE) & 1);
          TargetInterface.resetDebugInterface();
          TargetInterface.stop();
          EnableCM4();
          TargetInterface.setDebugInterfaceProperty("set_adiv5_AHB_ap_num", 0);
          TargetInterface.stop();
        }
    }
  else
    {
      TargetInterface.resetAndStop(100);
      InitDCDC();
    }
}

function Reset2()
{
  Reset();
}

function ResetCM4()
{  
  Reset(); 
}

function InitDCDC()
{
  // i.MXRT1010, i.MXRT1015, i.MXRT1020, i.MXRT1024, i.MXRT1050, i.MXRT1060, i.MXRT1064
  ocotp_base = 0x401F4000;
  ocotp_fuse_bank0_base = ocotp_base + 0x400;
  dcdc_base = 0x40080000;
  
  dcdc_trim_loaded = 0;

  reg = TargetInterface.peekWord(ocotp_fuse_bank0_base + 0x90);
  if (reg & (1<<10))
    {
      // DCDC: REG0->VBG_TRM
      trim_value = (reg & (0x1F << 11)) >> 11; 
      reg = (TargetInterface.peekWord(dcdc_base + 0x4) & ~(0x1F << 24)) | (trim_value << 24);
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
          reg = (TargetInterface.peekWord(dcdc_base + 0xC) & ~(0x1F)) | ((0xF + index));
          TargetInterface.pokeWord(dcdc_base + 0xC, reg);
          dcdc_trim_loaded = 1;
        }
    }

  if (dcdc_trim_loaded)
    {
      // delay about 400us till dcdc is stable.
      TargetInterface.delay(1);
    }
}

function EnableSWO()
{
/*
  CCM->LPCG[49].DIRECT = 1; // IOMUXC
  CCM->LPCG[50].DIRECT = 1; // IOMUXLPSR
  CCM->LPCG[41].DIRECT = 1; // CSTRACE
  CCM->LPCG[42].DIRECT = 1; // CSSYS

  IOMUXC_LPSR->SW_MUX_CTL_PAD[kIOMUXC_LPSR_SW_MUX_CTL_PAD_GPIO_LPSR_11] = 7;
  IOMUXC_LPSR->SW_PAD_CTL_PAD[kIOMUXC_LPSR_SW_PAD_CTL_PAD_GPIO_LPSR_11] = 1<<3|1<<2|1<<1|1;

  // TRACE_SWO_SETUP?
  IOMUXC->SW_MUX_CTL_PAD[kIOMUXC_SW_MUX_CTL_PAD_GPIO_DISP_B2_07] = 0x03;
  IOMUXC->SW_PAD_CTL_PAD[kIOMUXC_SW_PAD_CTL_PAD_GPIO_DISP_B2_07] = 0x06;

  // TRACE_CLK_SETUP?
  //IOMUXC->SW_MUX_CTL_PAD[kIOMUXC_SW_MUX_CTL_PAD_GPIO_DISP_B2_06] = 0x03;
  //IOMUXC->SW_PAD_CTL_PAD[kIOMUXC_SW_PAD_CTL_PAD_GPIO_DISP_B2_06] = 0x06;

  // CSTRACE_CLK_ROOT
  CCM->CLOCK_ROOT[6].CONTROL = 4<<8;
  CCM->CLOCK_ROOT[6].CONTROL_SET = 4;

  // CS-SWO
  *(volatile unsigned *)(0xE0048FB0) = 0xC5ACCE55;
  *(volatile unsigned *)(0xE0048010) = 0x8;
  *(volatile unsigned *)(0xE00480F0) = 0x2;

  // CSFT
  *(volatile unsigned *)(0xE0045FB0) = 0xC5ACCE55;
  *(volatile unsigned *)(0xE0045000) = 0xF;

  // M7
  *(volatile unsigned *)(0xE0043FB0) = 0xC5ACCE55;
  *(volatile unsigned *)(0xE0043000) = 0xF;
*/
}

function EnableTrace(TraceInterfaceType)
{
  var CPUID = TargetInterface.peekWord(0xE000ED00);
  if (((CPUID>>4)&0xf)==4)
    return; // not supported for Cortex-M4
  if (TraceInterfaceType == "TracePort")
    {        
    }
  else if (TraceInterfaceType == "SWO")
    {
      var target = TargetInterface.getProjectProperty("Target");
      if (target.indexOf("MIMXRT1011")==0)
        {
          TargetInterface.pokeWord(0x401F8024, 3); // IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_09.MUX_MODE=ALT3(ARM_TRACE_SWO)
          TargetInterface.pokeWord(0x401F80D4, 0x00F9); // IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_09
          TargetInterface.pokeWord(0x400FC018, TargetInterface.peekWord(0x400FC018) | (3<<14)); // CCM_CBCMR.TRACE_CLK_SEL
          TargetInterface.pokeWord(0x400FC024, TargetInterface.peekWord(0x400FC024) & ~(3<<25)); // CCM_CSCDR1.TRACE_PODF
        }
      else if (target.indexOf("MIMXRT1015")==0 || target.indexOf("MIMXRT102")==0)
        {
          TargetInterface.pokeWord(0x401F80E8, 6); // IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_11.MUX_MODE=ALT6(ARM_TRACE_SWO)
          TargetInterface.pokeWord(0x401F825C, 0x00F9); // IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B0_11
          TargetInterface.pokeWord(0x400FC018, TargetInterface.peekWord(0x400FC018) | (3<<14)); // CCM_CBCMR.TRACE_CLK_SEL
          TargetInterface.pokeWord(0x400FC024, TargetInterface.peekWord(0x400FC024) & ~(3<<25)); // CCM_CSCDR1.TRACE_PODF
        }
      else if (target.indexOf("MIMXRT105")==0)
        {         
          TargetInterface.pokeWord(0x401F8170, 2); // IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_13.MUX_MODE=ALT2(ARM_TRACE_SWO)
          TargetInterface.pokeWord(0x401F82E0, 0x00F9); // IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B0_13
          TargetInterface.pokeWord(0x400FC018, TargetInterface.peekWord(0x400FC018) | (3<<14)); // CCM_CBCMR.TRACE_CLK_SEL
          TargetInterface.pokeWord(0x400FC024, TargetInterface.peekWord(0x400FC024) & ~(3<<25)); // CCM_CSCDR1.TRACE_PODF
        }
      else if (target.indexOf("MIMXRT104")==0 || target.indexOf("MIMXRT106")==0)
        {         
          TargetInterface.pokeWord(0x401F80E4, 9); // IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_10.MUX_MODE=ALT9(ARM_TRACE_SWO)
          TargetInterface.pokeWord(0x401F82D4, 0x00F9); // IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B0_10
          TargetInterface.pokeWord(0x400FC018, TargetInterface.peekWord(0x400FC018) | (3<<14)); // CCM_CBCMR.TRACE_CLK_SEL
          TargetInterface.pokeWord(0x400FC024, TargetInterface.peekWord(0x400FC024) & ~(3<<25)); // CCM_CSCDR1.TRACE_PODF
        }
      else if(target.indexOf("MIMXRT11")==0)
        {
          TargetInterface.pokeWord(0x40C08000+0x2C, 7); // IOMUXC_LPSR_SW_MUX_CTL_PAD_GPIO_LPSR_11.MUX_MODE=ALT7(ARM_TRACE_SWO)
          TargetInterface.pokeWord(0x40C08000+0x6C, 2); // IOMUXC_LPSR_SW_PAD_CTL_PAD_GPIO_LPSR_11
          TargetInterface.pokeWord(0x40CC0300, 0x703);  // CLOCK_ROOT6_CONTROL - SYS_PLL2_CLK/4
          // CSFT
          TargetInterface.pokeWord(0xE0045FB0, 0xC5ACCE55);
          TargetInterface.pokeWord(0xE0045000, 0xF);
          // M7
          TargetInterface.pokeWord(0xE0043FB0, 0xC5ACCE55);
          TargetInterface.pokeWord(0xE0043000, 0xF);
        }
    }
}
