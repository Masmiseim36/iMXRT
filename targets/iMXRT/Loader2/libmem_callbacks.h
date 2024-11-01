/** Loader for iMXRT-Family
Copyright (C) 2019-2026 Markus Klein
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

#ifndef _LIBMEM_CALLBACKS_H_
#define _LIBMEM_CALLBACKS_H_

#include "libmem.h"

namespace Xspi
{
	int ProgramPage (libmem_driver_handle_t *h, uint8_t *destination, const uint8_t *source);

	const extern libmem_driver_functions_t     DriverFunctions;
	const extern libmem_ext_driver_functions_t DriverFunctions_Extended;
}

namespace Hyperflash
{
	int ProgramPage (libmem_driver_handle_t *h, uint8_t *destination, const uint8_t *source);

	const extern libmem_driver_functions_t     DriverFunctions;
	const extern libmem_ext_driver_functions_t DriverFunctions_Extended;
} // namesapce Hyperflash

#endif // _LIBMEM_CALLBACKS_H_