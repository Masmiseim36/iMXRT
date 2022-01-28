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

function EnableCM4()
{
  TargetInterface.pokeUint32 (0x40C04000, 0x3);
  TargetInterface.setDebugInterfaceProperty ("set_adiv5_AHB_ap_num", 1);
}

function ResetCM4()
{
  TargetInterface.stop(1000);
}

function Reset()
{
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

