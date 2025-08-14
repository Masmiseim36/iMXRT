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

#include "libmem_callbacks.h"
#include "fsl_common.h"
#include "FlexSPI_Helper.h"
#include "FlexSPI_Spansion.h"


namespace Xspi
{
	namespace
	{
		/*! EraseChip:
		\brief Erase the whole-Flash-memory
		\param base The FlexSPI-Interface where the Flash is located which should be erased
		\return static status_t Status of the Operation - kStatus_Success when successfully */
		[[maybe_unused]] status_t EraseChip (FlexSPI_Helper *base)
		{
			DebugPrint ("EraseChip\r\n");

			status_t stat = base->WriteEnable (0);
			if (stat != kStatus_Success)
				return stat;

			stat = base->SendCommand (0, LUT_EraseChip);
			if (stat != kStatus_Success)
				return LIBMEM_STATUS_ERROR;

			return base->WaitBusBusy ();
		}

		/*! EraseSector:
		\brief Erase a sector of the Flash-Memory
		\param h Handle to the Flash-Driver
		\param si Information about the sector which should be erased
		\return static int LibmemStaus_Success when the erase operation was successfully, otherwise LibmemStaus_Error */
		int EraseSector (libmem_driver_handle_t *h, libmem_sector_info_t *si)
		{
			if (IsSectorEmpty (reinterpret_cast<uint32_t *>(si->start)))
			{
				DebugPrintf ("EraseSector at 0x%x, is allready erased\r\n", si->start);
				return LIBMEM_STATUS_SUCCESS;
			}

			auto *base = reinterpret_cast<FlexSPI_Helper *>(h->user_data);
			const uint32_t sectorAddr = libmem_CalculateOffset (h, si->start);
			if (sectorAddr == UINT32_MAX)
				return LibmemStaus_Error;

			DebugPrintf ("EraseSector at 0x%x, size: %d\r\n", sectorAddr, si->size);

			status_t status = base->WriteEnable (sectorAddr);
			if (status != kStatus_Success)
				return LibmemStaus_Error;

			status = base->SendCommand (sectorAddr, LUT_EraseSector);
			if (status != kStatus_Success)
				return LibmemStaus_Error;

			status = base->WaitBusBusy ();
			if (status != kStatus_Success)
				return LibmemStaus_Error;

			return LibmemStaus_Success;
		}
	} // namespace

	/*! ProgramPage:
	\brief Write Data to a Flash-Page
	\param h Handle to the Flash-Driver
	\param destination Address to write the Data to. This Address is in the Address-Range of the Controller
	\param source Address of the Array with the data to write
	\return static int LibmemStaus_Success when the write operation was successfully, otherwise LibmemStaus_Error */
	int ProgramPage (libmem_driver_handle_t *h, uint8_t *destination, const uint8_t *source)
	{
		auto *base = reinterpret_cast<FlexSPI_Helper *>(h->user_data);
		const uint32_t deviceAddress = libmem_CalculateOffset (h, destination);
		if (deviceAddress == UINT32_MAX)
			return LibmemStaus_Error;

		DebugPrintf ("ProgramPage at 0x%08X (Offset:0x%X)\r\n", destination, deviceAddress);

		// Write enable
		status_t status = base->WriteEnable (deviceAddress);
		if (status != kStatus_Success)
			return LibmemStaus_Error;

		// Prepare page program command
		Transfer flashXfer
		{
			deviceAddress,				// deviceAddress	- Operation device address.
			CommandType::Write,			// cmdType			- Execution command type.
			LUT_ProgramPage,			// seqIndex			- Sequence ID for command.
			1,							// SeqNumber		- Sequence number for command.
			(uint32_t *)source,			// data				- Data buffer.
			QSPIFLASH_PAGE_SIZE			// dataSize			- Data size in bytes.
		};
		status = base->TransferBlocking (&flashXfer);
		if (status != kStatus_Success)
			return LibmemStaus_Error;

		status = base->WaitBusBusy ();
		if (status != kStatus_Success)
			return LibmemStaus_Error;

		// Do software reset or clear AHB buffer directly depending on the device capabilities
		#if defined(FSL_FEATURE_SOC_OTFAD_COUNT) && defined(FLEXSPI_AHBCR_CLRAHBRXBUF_MASK) && defined(FLEXSPI_AHBCR_CLRAHBTXBUF_MASK)
			base->AHBCR |= FLEXSPI_AHBCR_CLRAHBRXBUF_MASK | FLEXSPI_AHBCR_CLRAHBTXBUF_MASK;
			base->AHBCR &= ~(FLEXSPI_AHBCR_CLRAHBRXBUF_MASK | FLEXSPI_AHBCR_CLRAHBTXBUF_MASK);
		#else
			base->SoftwareReset ();
		#endif
		return LibmemStaus_Success;
	}

	namespace
	{
		/*! libmem_ProgramPage:
		\brief The LIBMEM driver's write function.
		\param h    A pointer to the handle of the LIBMEM driver.
		\param dest A pointer to the memory address in memory range handled by driver to write data to.
		\param src  pointer to the memory address to read data from.
		\param size The number of bytes to write.
		\return int The LIBMEM status result */
		int libmem_ProgramPage (libmem_driver_handle_t *h, uint8_t *dest, const uint8_t *src, size_t size)
		{
			DebugPrintf ("libmem_ProgramPage at 0x%08X - size: %d\r\n", dest, size);
			auto *driver = static_cast<LibmemDriver *>(h);
			return libmem_driver_paged_write (h, dest, src, size, &driver->PageWriteControlBlock);
		}

		/*! libmem_EraseSector:
		\brief The LIBMEM driver's erase function
		\param h           A pointer to the handle of the LIBMEM driver.
		\param start       A pointer to the initial memory address in memory range handled by driver to erase.
		\param size        The number of bytes to erase.
		\param erase_start A pointer to a location in memory to store a pointer to the start of the memory range that has actually been erased or nullptr if not required.
		\param erase_size  A pointer to a location in memory to store the size in bytes of the memory range that has actually been erased or nullptr if not required.
		\return int        The LIBMEM status result */
		int libmem_EraseSector (libmem_driver_handle_t *h, uint8_t *start, size_t size, uint8_t **erase_start, size_t *erase_size)
		{
			DebugPrintf ("libmem_EraseSector at 0x%08X - size: %d\r\n", start, size);
			const int ret = libmem_foreach_sector_in_range (h, start, size, EraseSector, erase_start, erase_size);

			auto *base = reinterpret_cast<FlexSPI_Helper *>(h->user_data);
			base->SoftwareReset ();

			return ret;
		}

		/*! libmem_Flush:
		\brief The LIBMEM driver's flush function.
		\param h    A pointer to the handle of the LIBMEM driver. 
		\return int The LIBMEM status result */
		int libmem_Flush (libmem_driver_handle_t *h)
		{
			DebugPrint ("libmem_Flush\r\n");
			auto *driver = static_cast<LibmemDriver *>(h);
			return libmem_driver_paged_write_flush (h, &driver->PageWriteControlBlock);
		}

		/*! libmem_Read:
		\brief The LIBMEM driver's read extended function.
		\param h    A pointer to the handle of the LIBMEM driver.
		\param dest A pointer to the initial memory address to write data to.
		\param src  A pointer to the initial memory address in the memory range handled by the driver to read data from.
		\param size The number of bytes to write.
		\return int The LIBMEM status result */
		int libmem_Read (libmem_driver_handle_t *h, uint8_t *dest, const uint8_t *src, size_t size)
		{
           const uint32_t deviceAddress = libmem_CalculateOffset (h, src);
           if (deviceAddress == UINT32_MAX)
               return LibmemStaus_Error;

			DebugPrintf ("Read at 0x%x, size: %d\r\n", src, size);
			if (size == 0)
				return LibmemStaus_InvalidParameter;
//			memcpy (dest, src, size);

			Transfer flashXfer
			{
				deviceAddress,		// Operation device address.
				CommandType::Read,	// Execution command type.
				LUT_ReadArray,		// Sequence ID for command.
				1,					// Sequence number for command.
				(uint32_t *)dest,	// Data buffer.
				size				// Data size in bytes.
			};

			auto *base = reinterpret_cast<FlexSPI_Helper *>(h->user_data);
			const status_t status = base->TransferBlocking (&flashXfer);
			if (status != kStatus_Success)
				return status;

			return LibmemStaus_Success;
		}


		/*! libmem_CRC32:
		\brief The LIBMEM driver's crc32 extended function.
		\param h     A pointer to the handle of the LIBMEM driver.
		\param start A pointer to the start of the address range.
		\param size  The size of the address range in bytes.
		\param crc   The initial CRC-32 value.
		\return uint32_t The computed CRC-32 value. */
		uint32_t libmem_CRC32 ([[maybe_unused]]libmem_driver_handle_t *h, const uint8_t *start, size_t size, uint32_t crc)
		{
			DebugPrintf ("Calculate CRC from 0x%X, size 0x%X, calculated CRC: 0x%X\r\n", start, size, crc);
			static std::array <uint8_t, 4096> pageBuffer;

			while (size >= pageBuffer.size())
			{
				libmem_Read (h, pageBuffer.data(), const_cast<uint8_t *>(start), pageBuffer.size());
				crc = libmem_crc32_direct (pageBuffer.data(),  pageBuffer.size(), crc);
				start += pageBuffer.size();
				size  -= pageBuffer.size();
			}
			if (size > 0)
			{
				libmem_Read (h, pageBuffer.data(), const_cast<uint8_t *>(start), size);
				crc = libmem_crc32_direct (pageBuffer.data(), size, crc);
			}
			return crc;
		}
	} // namespace

	const libmem_driver_functions_t DriverFunctions
	{
		libmem_ProgramPage,
		nullptr,
		libmem_EraseSector,
		nullptr,
		nullptr,
		libmem_Flush
	};

	const libmem_ext_driver_functions_t DriverFunctions_Extended
	{
		nullptr,
		libmem_Read,
		libmem_CRC32
	};
} // namespace Xspi

namespace Hyperflash
{
	namespace
	{
		/*! WriteEnable:
		\brief Send write-enable command
		\param base The Flex-SPI-base to use
		\param baseAddr The base-address of the command
		\return status_t kStatus_Success if the operation was successfully */
		status_t WriteEnable (FlexSPI_Helper *base, uint32_t baseAddr)
		{
			return base->SendCommand (baseAddr, static_cast<LUT_CommandOffsets>(Spansion::Command::WriteEnable), 2);
		}

		/*! WaitBusBusy:
		\brief Wait until the Write/erase operation is finished and the Flash is not busy anymore
		\param base The Flex-SPI-base to use
		\return status_t kStatus_Success if the operation was successfully */
		status_t WaitBusBusy (FlexSPI_Helper *base)
		{
			// Wait status ready.
			bool isBusy{false};
			uint32_t readValue{};
			status_t status{};
			Transfer flashXfer
			{
				0,														// deviceAddress	- Operation device address.
				CommandType::Read,										// cmdType			- Execution command type.
				static_cast<uint8_t>(Spansion::Command::ReadStatus),	// seqIndex			- Sequence ID for command.
				2,														// SeqNumber		- Sequence number for command.
				&readValue,												// data				- Data buffer.
				2														// dataSize			- Data size in bytes.
			};

			do
			{
				status = base->TransferBlocking (&flashXfer);
				if (status != kStatus_Success)
					return status;

				isBusy = !(readValue & 0x8000U);

				if (readValue & 0x3200U)
				{
					status = kStatus_Fail;
					break;
				}
			}
			while (isBusy);

			return status;
		}

		/*! EraseChip:
		\brief Erase the whole-Flash-memory
		\param base The FlexSPI-Interface where the Flash is located which should be erased
		\return static status_t Status of the Operation - kStatus_Success when successfully */
		[[maybe_unused]] status_t EraseChip (FlexSPI_Helper *base)
		{
			DebugPrintf ("EraseChip\r\n");

			status_t stat = ::Hyperflash::WriteEnable (base, 0);
			if (stat != kStatus_Success)
				return stat;

			stat = base->SendCommand (0, static_cast<LUT_CommandOffsets>(Spansion::Command::EraseChip), 4);
			if (stat != kStatus_Success)
				return LIBMEM_STATUS_ERROR;

			return ::Hyperflash::WaitBusBusy (base);
		}

		/*! EraseSector:
		\brief Erase a sector of the Flash-Memory
		\param h Handle to the Flash-Driver
		\param si Information about the sector which should be erased
		\return static int LIBMEM_STATUS_SUCCESS when the erase operation was successfully, otherwise LIBMEM_STATUS_ERROR */
		status_t EraseSector (libmem_driver_handle_t *h, libmem_sector_info_t *si)
		{
			static constexpr uint32_t SectorSize = 256 * 1024;
			if (IsSectorEmpty (reinterpret_cast<uint32_t *>(si->start), SectorSize))
			{
				DebugPrintf ("EraseSector at 0x%08X, is allready erased\r\n", si->start);
				return LIBMEM_STATUS_SUCCESS;
			}

			auto *base = reinterpret_cast<FlexSPI_Helper *>(h->user_data);
			const uint32_t sectorAddr = libmem_CalculateOffset (h, si->start);
			if (sectorAddr == UINT32_MAX)
				return LIBMEM_STATUS_INVALID_RANGE;

			DebugPrintf ("EraseSector at 0x%08X, size: %d\r\n", sectorAddr, si->size);

			// Write enable
			status_t status = ::Hyperflash::WriteEnable (base, 0);
			if (status != kStatus_Success)
				return LIBMEM_STATUS_ERROR;

			status = base->SendCommand (sectorAddr, static_cast<LUT_CommandOffsets>(Spansion::Command::EraseSector), 4);
			if (status != kStatus_Success)
				return LibmemStaus_Error;

			status = ::Hyperflash::WaitBusBusy (base);
			if (status != kStatus_Success)
				return LIBMEM_STATUS_ERROR;

			return LIBMEM_STATUS_SUCCESS;
		}
	} // namespace 

	/*! ProgramPage:
	\brief Write Data to a Flash-Page
	\param h Handle to the Flash-Driver
	\param destination Address to write the Data to. This Address is in the Address-Range of the Controller
	\param source Address of the Array with the Data to write
	\return static int LIBMEM_STATUS_SUCCESS when the write operation was successfully, otherwise LIBMEM_STATUS_ERROR */
	int ProgramPage (libmem_driver_handle_t *h, uint8_t *destination, const uint8_t *source)
	{
		auto *base = reinterpret_cast<FlexSPI_Helper *>(h->user_data);
		const uint32_t deviceAddress = libmem_CalculateOffset (h, destination);
		if (deviceAddress == UINT32_MAX)
			return LIBMEM_STATUS_INVALID_RANGE;

		DebugPrintf ("ProgramPage at 0x%X\r\n", deviceAddress);

		// Write enable
		status_t status = ::Hyperflash::WriteEnable (base, 0);
		if (status != kStatus_Success)
			return LIBMEM_STATUS_ERROR;

		// Prepare page program command
		Transfer flashXfer
		{
			deviceAddress,											// deviceAddress	- Operation device address.
			CommandType::Write,										// cmdType			- Execution command type.
			static_cast<uint8_t>(Spansion::Command::PageProgram),	// seqIndex			- Sequence ID for command.
			2,														// SeqNumber		- Sequence number for command.
			const_cast<uint32_t *>(reinterpret_cast<const uint32_t *>(source)),// data				- Data buffer.
			HYPERFLASH_PAGE_SIZE									// dataSize			- Data size in bytes.
		};
		status = base->TransferBlocking (&flashXfer);
		if (status != kStatus_Success)
			return LIBMEM_STATUS_ERROR;

		status = ::Hyperflash::WaitBusBusy (base);
		if (status != kStatus_Success)
			return LIBMEM_STATUS_ERROR;

//		// Do software reset or clear AHB buffer directly depending on the device capabilities
//		#if defined(FSL_FEATURE_SOC_OTFAD_COUNT) && defined(FLEXSPI_AHBCR_CLRAHBRXBUF_MASK) && defined(FLEXSPI_AHBCR_CLRAHBTXBUF_MASK)
//			base->AHBCR |= FLEXSPI_AHBCR_CLRAHBRXBUF_MASK | FLEXSPI_AHBCR_CLRAHBTXBUF_MASK;
//			base->AHBCR &= ~(FLEXSPI_AHBCR_CLRAHBRXBUF_MASK | FLEXSPI_AHBCR_CLRAHBTXBUF_MASK);
//		#else
//			FLEXSPI_SoftwareReset (base);
//		#endif
		return LIBMEM_STATUS_SUCCESS;
	}

	namespace
	{
		/*! libmem_ProgramPage:
		\brief The LIBMEM driver's write function.
		\param h    A pointer to the handle of the LIBMEM driver.
		\param dest A pointer to the memory address in memory range handled by driver to write data to.
		\param src  pointer to the memory address to read data from.
		\param size The number of bytes to write.
		\return int The LIBMEM status result */
		int libmem_ProgramPage (libmem_driver_handle_t *h, uint8_t *dest, const uint8_t *src, size_t size)
		{
			DebugPrintf ("libmem_ProgramPage at 0x%x - size: %d\r\n", dest, size);
			auto *driver = static_cast<LibmemDriver *>(h);
			return libmem_driver_paged_write (h, dest, src, size, &driver->PageWriteControlBlock);
		}

		/*! libmem_EraseSector:
		\brief The LIBMEM driver's erase function
		\param h           A pointer to the handle of the LIBMEM driver.
		\param start       A pointer to the initial memory address in memory range handled by driver to erase.
		\param size        The number of bytes to erase.
		\param erase_start A pointer to a location in memory to store a pointer to the start of the memory range that has actually been erased or nullptr if not required.
		\param erase_size  A pointer to a location in memory to store the size in bytes of the memory range that has actually been erased or nullptr if not required.
		\return int        The LIBMEM status result */
		int libmem_EraseSector (libmem_driver_handle_t *h, uint8_t *start, size_t size, uint8_t **erase_start, size_t *erase_size)
		{
			return libmem_foreach_sector_in_range (h, start, size, EraseSector, erase_start, erase_size);
		}

		/*! libmem_Flush:
		\brief The LIBMEM driver's flush function.
		\param h    A pointer to the handle of the LIBMEM driver. 
		\return int The LIBMEM status result */
		int libmem_Flush (libmem_driver_handle_t *h)
		{
			DebugPrint ("libmem_Flush\r\n");
			auto *driver = static_cast<LibmemDriver *>(h);
			return libmem_driver_paged_write_flush (h, &driver->PageWriteControlBlock);
		}

		/*! libmem_Read:
		\brief The LIBMEM driver's read extended function.
		\param h    A pointer to the handle of the LIBMEM driver.
		\param dest A pointer to the initial memory address to write data to.
		\param src  A pointer to the initial memory address in the memory range handled by the driver to read data from.
		\param size The number of bytes to write.
		\return int The LIBMEM status result */
		int libmem_Read ([[maybe_unused]] libmem_driver_handle_t *h, uint8_t *dest, const uint8_t *src, size_t size)
		{
			if (size != 0)
				memcpy (dest, src, size);
			return LIBMEM_STATUS_SUCCESS;
		}

		/*! libmem_CRC32:
		\brief The LIBMEM driver's crc32 extended function.
		\param h     A pointer to the handle of the LIBMEM driver.
		\param start A pointer to the start of the address range.
		\param size  The size of the address range in bytes.
		\param crc   The initial CRC-32 value.
		\return uint32_t The computed CRC-32 value. */
		uint32_t libmem_CRC32 ([[maybe_unused]] libmem_driver_handle_t *h, const uint8_t *start, size_t size, uint32_t crc)
		{
			crc = libmem_crc32_direct (start, size, crc);
			return crc;
		}
	} // namespace 

	const libmem_driver_functions_t DriverFunctions
	{
		libmem_ProgramPage,
		nullptr,
		libmem_EraseSector,
		nullptr,
		nullptr,
		libmem_Flush
	};

	const libmem_ext_driver_functions_t DriverFunctions_Extended
	{
		nullptr,
		libmem_Read,
		libmem_CRC32
	};
} // namesapce Hyperflash


