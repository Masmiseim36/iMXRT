/** Loader for iMXRT-Family
Copyright (C) 2019-2024 Markus Klein
https://github.com/Masmiseim36/iMXRT

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE. */

#include "libmem_Tools.h"
#include "libmem.h"


uint32_t LibmemDriver::NextFree = 0;
LibmemDriver LibmemDriver::Drivers[4];

/*! libmem_CalculateOffset:
\brief Recalculate the Address a of a memory-mapped Flash-memory from absolute addressing to memory-Local-Addressing
\param base the FlexSPI-Interface which is be used
\param Addr The Address to be converted
\return uint32_t */
uint32_t libmem_CalculateOffset (libmem_driver_handle_t *handle, uint8_t *Addr)
{
	return Addr - handle->start;
}

/*! Libmem_GetErrorString:
\brief Return an Error String for printing to the user
\param Error Error-ID to be converted to a string
\return const char* const */
const char * Libmem_GetErrorString (int Error)
{
	switch (Error)
	{
		// error codes
		case LIBMEM_STATUS_ERROR:
			return "Generic Error";
		case LIBMEM_STATUS_TIMEOUT:
			return "Timeout Error";
		case LIBMEM_STATUS_LOCKED:
			return "Lock Error";
		case LIBMEM_STATUS_NOT_IMPLEMENTED:
			return "Not Implemented Error";
		case LIBMEM_STATUS_GEOMETRY_REGION_OVERFLOW:
			return "Geometry Region Overflow Error";
		case LIBMEM_STATUS_NO_DRIVER:
			return "No Driver Error";
		case LIBMEM_STATUS_CFI_ERROR:
			return "CFI Error";
		case LIBMEM_STATUS_INVALID_RANGE:
			return "Invalid Range Error";
		case LIBMEM_STATUS_INVALID_PARAMETER:
			return "Invalid parameter Error";
		case LIBMEM_STATUS_INVALID_WIDTH:
			return "Invalid Widdth Error";
		case LIBMEM_STATUS_INVALID_DEVICE:
			return "Invalid Device Error";
//		case LIBMEM_CFI_CMDSET_NONE:
//			return "Invalid CFI command set number";
		
		// status Codes
//		case LIBMEM_STATUS_SUCCESS:
//			return "Success";
//		case LIBMEM_CFI_CMDSET_INTEL_EXTENDED:
//			return "Intel extended command set";
//		case LIBMEM_CFI_CMDSET_AMD_STANDARD:
//			return "AMD standard command set";
//		case LIBMEM_CFI_CMDSET_INTEL_STANDARD:
//			return "Intel standard command set";
//		case LIBMEM_CFI_CMDSET_AMD_EXTENDED:
//			return "AMD extended command set";
//		case LIBMEM_CFI_CMDSET_WINBOND_STANDARD:
//			return "Winbond standard command set";
//		case LIBMEM_CFI_CMDSET_MITSUBISHI_STANDARD:
//			return "Mitsubishi standard command set";
//		case LIBMEM_CFI_CMDSET_MITSUBISHI_EXTENDED:
//			return "Mitsubishi extended command set";
//		case LIBMEM_CFI_CMDSET_SST_PAGE_WRITE:
//			return "SST page write command set";
		default:
			return "Unknown Error";
	}
}


/*! CalculateCapacity_KBytes:
\brief Calculates the capacity in Byes
\param C The Capacity in 2^n number of bytes
\return uint_least32_t The capacity in bytes */
uint_least32_t CalculateCapacity_KBytes (Capacity c)
{
	uint_least32_t size = 1U << (static_cast<uint_least32_t>(c));
	return size / 1024;
}