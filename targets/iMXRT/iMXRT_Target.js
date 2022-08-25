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

function EnableCM4()
{
  if (!IsArmdInterface())
    return;
  var start = 0x20200000;
  TargetInterface.pokeUint32 (start, 0xE7FEE7FE);
  TargetInterface.pokeUint32 (start+0x4, start+1);
  TargetInterface.pokeUint32 (0x40C0C000, start & 0xffff); // IOMUXC_LPSR_GPR0
  TargetInterface.pokeUint32 (0x40C0C004, (start & 0xffff0000) >> 16); // IOMUXC_LPSR_GPR1
  TargetInterface.pokeUint32 (0x40C04000, 0x1); // SCR
  TargetInterface.setDebugInterfaceProperty("set_adiv5_AHB_ap_num", 1, 0, 0);
}

function Connect()
{
}

function GetPartName()
{
  if (TargetInterface.getProjectProperty("Target").indexOf("_CM4")!=-1)
    EnableCM4();
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
      TargetInterface.setNSRST(0);
      TargetInterface.delay(100);
      TargetInterface.setNSRST(1);
      TargetInterface.delay(100);
      TargetInterface.resetDebugInterface();
      TargetInterface.stop(1000);     
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
  EnableCM4();
  TargetInterface.stop(1000);
  TargetInterface.setDebugInterfaceProperty("set_adiv5_AHB_ap_num", 0, 0, 0); 
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
