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