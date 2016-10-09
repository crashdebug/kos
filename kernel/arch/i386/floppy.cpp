#include "dma.h"
#include "floppy.h"
#include "irq.h"
#include <stdio.h>
#include "rtc.h"
#include "util.h"

using namespace kos;

namespace kos::x86
{
	static const char * aFloppyTypes[8] =
	{
	    "None",
	    "360kB 5.25\"",
	    "1.2MB 5.25\"",
	    "720kB 3.5\"",
	    "1.44MB 3.5\"",
	    "2.88MB 3.5\"",
	    "Unknown",
	    "Unknown"
	};

	static const char * aFloppyStatus[] =
	{
		"Normal",
		"Error",
		"Invalid command",
		"Drive not ready"
	};

	// The parameters for the fd0
	FloppyParameters *oFloppyParams = (FloppyParameters*)FLOPPY_PARAMETER_ADDRESS;

	/*
	FloppyDrive::FloppyDrive( )
	{
	}
	*/

	FloppyDrive::FloppyDrive(unsigned char iDrive, unsigned char eDriveType)
		//: Kernel::Drive(iDrive)
	{
		printf("Bytes/Sector: %i\n", oFloppyParams->iBytesPerSector);
		printf("Sectors/Track: %i\n", oFloppyParams->iSectorsPerTrack);
		printf("Gap Len: %i\n", oFloppyParams->iGapLength);
		printf("Data len: %i\n", oFloppyParams->iDataLength);

		this->id = iDrive;
		this->sName[0] = 'f';
		this->sName[1] = 'd';
		this->sName[2] = '0' + iDrive;
		this->sName[3] = 0;

		this->bMotorOn = false;
	    this->eType = eDriveType;

		if (this->eType != FLOPPY_TYPE_NONE)
		{
	    	this->Reset();
		}
	}

	FloppyDrive::~FloppyDrive()
	{
	}

	unsigned char FloppyDrive::GetID() const
	{
		return this->id;
	}

	const char* FloppyDrive::GetName() const
	{
		return this->sName;
	}

	unsigned int FloppyDrive::GetMaxSectors() const
	{
		return 0xffff;
	}

	unsigned char* FloppyDrive::GetDescription() const
	{
		return (unsigned char *)aFloppyTypes[this->eType];
	}

	bool FloppyDrive::IsInstalled() const
	{
		return (this->eType != FLOPPY_TYPE_NONE);
	}

	unsigned int FloppyDrive::GetSectorSize() const
	{
		// Not yet implemented
		return 0;
	}

	unsigned int FloppyDrive::ReadData(unsigned int iPosition, void * aBuffer, unsigned int iOffset, unsigned int iLen)
	{
		// Not yet implemented
		return 0;
	}

	unsigned int FloppyDrive::WriteData(unsigned int iPosition, void * aBuffer, unsigned int iOffset, unsigned int iLen)
	{
		// Not yet implemented
		return 0;
	}

	unsigned int FloppyDrive::ReadSector(unsigned int iSectorID, void * aBuffer)
	{
		if (!this->IsInstalled( ))
		{
			printf("FloppyDrive::ReadSector(%2x): Drive not installed!\n", this->GetID());
			return 0;
		}
		else if (iSectorID == 0)
		{
			printf("FloppyDrive::ReadSector(%2x): Invalid sector [%i]!\n", this->GetID( ), iSectorID);
			return 0;
		}
		else if (aBuffer == 0)
		{
			printf("FloppyDrive::ReadSector(%2x): Invalid buffer [%p]!\n", this->GetID( ), aBuffer);
			return 0;
		}

		unsigned char iCylinder, iHead, iSector;
		TranslateSectorID(iSectorID, &iSector, &iHead, &iCylinder);
		return this->ReadSector(iSector, iHead, iCylinder, aBuffer);
	}

	unsigned int FloppyDrive::WriteSector(unsigned int iSectorID, void* aBuffer)
	{
		if (!this->IsInstalled())
		{
			printf("FloppyDrive::WriteSector(%2x): Drive not installed!\n", this->GetID());
			return 0;
		}
		else if (iSectorID == 0)
		{
			printf("FloppyDrive::WriteSector(%2x): Invalid sector [%i]!\n", this->GetID(), iSectorID);
			return 0;
		}
		else if (aBuffer == 0)
		{
			printf("FloppyDrive::WriteSector(%2x): Invalid buffer [%p]!\n", this->GetID(), aBuffer);
			return 0;
		}

		printf("FloppyDrive::WriteSector(%2x): Sector [%x]\n", this->GetID(), iSectorID);

		unsigned char iCylinder, iHead, iSector;
		TranslateSectorID(iSectorID, &iSector, &iHead, &iCylinder);
		return this->WriteSector(iSector, iHead, iCylinder, aBuffer);
	}

	unsigned int FloppyDrive::Seek(unsigned int iSectorID)
	{
		if (!this->IsInstalled())
		{
			printf("FloppyDrive::Seek(%2x): Drive not installed!\n", this->GetID());
			return 0;
		}
		else if (iSectorID == 0)
		{
			printf("FloppyDrive::Seek(%2x): Invalid sector [%i]!\n", this->GetID(), iSectorID);
			return 0;
		}

		unsigned char iCylinder, iHead, iSector;
		TranslateSectorID(iSectorID, &iSector, &iHead, &iCylinder);
		if (this->Seek(iHead, iCylinder))
		{
			return iSectorID;
		}
		else
		{
			return 0;
		}
	}

	void FloppyDrive::ControlMotor(bool bTurnOn)
	{
		if (!this->IsInstalled())
		{
			printf("FloppyDrive::ControlMotor(%2x): Drive not installed!\n", this->GetID());
			return;
		}

		if (bTurnOn && !bMotorOn)
		{
			// Start motor and enable interrupts
			outb(FLOPPY_BASE + FLOPPY_DIGITAL_OUTPUT_REG, 0x1c);
			bMotorOn = true;
		}
		else if (bTurnOn && bMotorOn)
		{
			// Stop the motor
			outb(FLOPPY_BASE + FLOPPY_DIGITAL_OUTPUT_REG, 0x0c);
			bMotorOn = false;
		}
		sleep(50);
	}

	void FloppyDrive::CheckInterrupt(unsigned char *iStatus, unsigned char *iCylinder)
	{
		WriteFloppyCommand(FLOPPY_CMD_SENSE_INTERRUPT);
		*iStatus = ReadFloppyData();
		*iCylinder = ReadFloppyData();
		//Console.WriteLine( "FloppyDrive::CheckInterrupt( ) : Status [%x] Cyl [%d]", *iStatus, *iCylinder );
	}

	bool FloppyDrive::Calibrate()
	{
		if (!this->IsInstalled())
		{
			printf("FloppyDrive::Calibrate(%2x): Drive not installed!\n", this->GetID());
			return false;
		}

		unsigned char i = 0, iStatus, iCyl;
		for ( ; i < FLOPPY_RETRY_COUNT; i++)
		{
			// Send the command to recalibrate
			WriteFloppyCommand(FLOPPY_CMD_RECALIBRATE);
			// Send which drive
			WriteFloppyCommand(this->GetID());
			// Wait for the drive to signal it's ready
			wait_for_irq(FLOPPY_IRQ);
			// Check the status
			this->CheckInterrupt(&iStatus, &iCyl);
			// Something went wrong?
			if (iStatus & 0xc0)
			{
	            printf("FloppyDrive::CalibrateFloppy(%2x): [%x] %s\n", this->GetID(), iStatus, aFloppyStatus[iStatus >> 6]);
	            continue;
			}
			// Found cylinder 0?
			if (iCyl == 0)
			{
				// Turn the motor off
				this->ControlMotor(false);
				return true;
			}
		}
		printf("FloppyDrive::CalibrateFloppy(%2x): Calibration failed after %i attempts!\n", this->GetID(), i);
		return false;
	}

	bool FloppyDrive::Reset()
	{
		if (!this->IsInstalled())
		{
			printf("FloppyDrive::Reset(%2x): Drive not installed!\n", this->GetID());
			return false;
		}

		// Disable controller
		outb(FLOPPY_BASE + FLOPPY_DIGITAL_OUTPUT_REG, 0x00);
		// Enable controller
		outb(FLOPPY_BASE + FLOPPY_DIGITAL_OUTPUT_REG, 0x0c);
		// Turn on the motor
		this->ControlMotor(true);
		// Recalibrate the drive. This brings the drive to a known state.
		WriteFloppyCommand(FLOPPY_CMD_RECALIBRATE);
		// Wait for the drive to do it. It will send an IRQ back when it's done.
		wait_for_irq(FLOPPY_IRQ);

		unsigned char iStatus, iCyl;
		this->CheckInterrupt(&iStatus, &iCyl);

		// Set transfer speed to 500kb/s
		outb(FLOPPY_BASE + FLOPPY_CONFIG_CONTROL_REG, 0x00);

		//	1st byte is: bits[7:4] = steprate, bits[3:0] = head unload time
		//	2nd byte is: bits[7:1] = head load time, bit[0] = no-DMA
		//
		//	steprate	= ( 8.0ms - entry * 0.5ms ) * ( 1MB/s / xfer_rate )
		//	head_unload	= 8ms * entry * ( 1MB/s / xfer_rate ), where entry 0 -> 16
		//	head_load	= 1ms * entry * ( 1MB/s / xfer_rate ), where entry 0 -> 128

	    WriteFloppyCommand(FLOPPY_CMD_SPECIFY);
	    WriteFloppyCommand(0xdf);		// steprate = 3ms, unload time = 240ms
	    WriteFloppyCommand(0x02);		// load time = 16ms, no-DMA = 0

		// Done
		if (this->Calibrate())
		{
			return true;
		}
		else
		{
			printf("FloppyDrive::Reset(%2x): Floppy reset failed!\n", this-GetID());
			return false;
		}
	}

	bool FloppyDrive::Seek(unsigned char iHead, unsigned char iCylinder)
	{
		if (!this->IsInstalled())
		{
			printf("FloppyDrive::Seek(%2x): Drive not installed!\n", this->GetID());
			return false;
		}
		else if (iHead > 1)
		{
			printf("FloppyDrive::Seek(%2x): Invalid head [%i]!\n", this->GetID(), iHead);
			return false;
		}

		this->ControlMotor(true);

		int i = 0;
		for ( ; i < FLOPPY_RETRY_COUNT; i++)
		{
			// Attempt to position to given cylinder
			// 1st byte bit[1:0] = drive, bit[2] = head
			// 2nd byte is cylinder number
			WriteFloppyCommand(FLOPPY_CMD_SEEK);
			WriteFloppyCommand(iHead << 2 | this->GetID());
			WriteFloppyCommand(iCylinder);

			wait_for_irq(FLOPPY_IRQ);

			unsigned char iStatus, iCyl;
			this->CheckInterrupt(&iStatus, &iCyl);
			if (iStatus & 0xc0)
			{
				printf("Floppy::Seek(%2x): Status = %s\n", this->GetID(), aFloppyStatus[iStatus >> 6]);
				continue;
			}
			if (iCyl == iCylinder)
			{
				this->ControlMotor(false);
				return true;
			}
		}
	   	this->ControlMotor(false);
		printf("Floppy::Seek(%2x): %i attempts exhausted. Giving up...\n", this->GetID(), i);
		return false;
	}

	int FloppyDrive::ReadSector(unsigned char iSector, unsigned char iHead, unsigned char iCylinder, void* aBuffer)
	{
		if (!this->IsInstalled())
		{
			return 0;
		}
		if (this->Transfer(FLOPPY_CMD_READ_SECTOR, 0x44, iSector, iHead, iCylinder, aBuffer) == 0)
		{
			return 512;
		}
		else
		{
			return 0;
		}
	}

	int FloppyDrive::WriteSector(unsigned char iSector, unsigned char iHead, unsigned char iCylinder, void* aBuffer)
	{
		if (!this->IsInstalled())
		{
			return 0;
		}
		if (this->Transfer(FLOPPY_CMD_WRITE_SECTOR, 0x48, iSector, iHead, iCylinder, aBuffer) == 0)
		{
			return 512;
		}
		else
		{
			return 0;
		}
	}

	int FloppyDrive::Transfer(unsigned char iCommand, unsigned char iDMAMode, unsigned char iSector, unsigned char iHead, unsigned char iCylinder, void* aBuffer)
	{
		if (!this->IsInstalled())
		{
			return 0;
		}

		if (!this->Seek(iHead, iCylinder))
		{
			return -1;
		}

		DMABlock oBlock;
		oBlock.Load(aBuffer, 512);
		StartDMA(0x02, &oBlock, iDMAMode);

		unsigned char iRetries = 0;
		char iError;
		for ( ; iRetries < FLOPPY_RETRY_COUNT; iRetries++)
		{
			this->ControlMotor(true);

			sleep(oFloppyParams[this->GetID( )].iHeadSettleTime);

			WriteFloppyCommand(iCommand);
			WriteFloppyCommand(iHead << 2 | this->GetID());
			WriteFloppyCommand(iCylinder);
			WriteFloppyCommand(iHead);
			WriteFloppyCommand(iSector);
			WriteFloppyCommand(oFloppyParams[this->GetID()].iBytesPerSector);	// Sector size = 128 * 2 ^ size (2 = 512b)
			WriteFloppyCommand(oFloppyParams[this->GetID()].iSectorsPerTrack);	// last sector
			WriteFloppyCommand(oFloppyParams[this->GetID()].iGapLength);		// 27 default gap3 value
			WriteFloppyCommand(oFloppyParams[this->GetID()].iDataLength);		// default value for data length

			wait_for_irq(FLOPPY_IRQ);

			//WaitFloppyReady( );
			unsigned char iStatus0, iStatus1, iStatus2, iResultCylinder, iResultHead, iResultSector, iBytesPerSector;
			iStatus0 = ReadFloppyData();
			iStatus1 = ReadFloppyData();
			iStatus2 = ReadFloppyData();

			// These are cylinder/head/sector values, updated with some
			// rather bizarre logic, that I would like to understand.
			iResultCylinder = ReadFloppyData();
			iResultHead = ReadFloppyData();
			iResultSector = ReadFloppyData();
			// Bytes per sector, should be what we programmed in
			iBytesPerSector = ReadFloppyData();

			iError = 0;
			if (iStatus0 & 0xc0)
			{
				printf("Floppy::Transfer(%2x): %s\n", this->GetID(), aFloppyStatus[iStatus0 >> 6]);
				iError = 1;
			}
			else if (iStatus1 & 0x80)
			{
				printf("Floppy::Transfer(%2x): End of Cylinder!\n", this->GetID());
				iError = 1;
			}
			else if (iStatus0 & 0x08)
			{
				printf("Floppy::Transfer(%2x): Drive not ready!\n", this->GetID());
				iError = 1;
			}
			else if (iStatus1 & 0x20)
			{
				printf("Floppy::Transfer(%2x): CRC error!\n", this->GetID());
				iError = 1;
			}
			else if (iStatus1 & 0x10)
			{
				printf("Floppy::Transfer(%2x): Controller timeout!\n", this->GetID());
				iError = 1;
			}
			else if (iStatus1 & 0x04)
			{
				printf("Floppy::Transfer(%2x): No data found!\n", this->GetID());
				iError = 1;
			}
			else if ((iStatus1 | iStatus2) & 0x01)
			{
				printf("Floppy::Transfer(%2x): No address mark found!\n", this->GetID());
				iError = 1;
			}
			else if ( iStatus2 & 0x40 )
			{
				printf("Floppy::Transfer(%2x): Deleted address mark!\n", this->GetID());
				iError = 1;
			}
			else if (iStatus2 & 0x20)
			{
				printf("Floppy::Transfer(%2x): CRC error in data!\n", this->GetID());
				iError = 1;
			}
			else if (iStatus2 & 0x10)
			{
				printf("Floppy::Transfer(%2x): Wrong cylinder!\n", this->GetID());
				iError = 1;
			}
			else if (iStatus2 & 0x04)
			{
				printf("Floppy::Transfer(%2x): uPD765 sector not found!\n", this->GetID());
				iError = 1;
			}
			else if (iStatus2 & 0x02)
			{
				printf("Floppy::Transfer(%2x): Bad cylinder!\n", this->GetID());
				iError = 1;
			}
			else if (iBytesPerSector != 0x2)
			{
				printf("Floppy::Transfer(%2x): Wanted 512B/sector, got %i\n", this->GetID(), (1 << (iBytesPerSector + 7)));
				iError = 1;
			}
			// Unrecoverable errors
			if (iStatus1 & 0x02)
			{
				printf("Floppy::Transfer(%2x): Not writable!\n", this->GetID());
				iError = 2;
			}

			// No errors?
			if (!iError)
			{
				break;
			}
			// Unrecoverable error?
			else if (iError > 1)
			{
				printf("Floppy::Transfer(%2x): [%i] Not retrying...\n", this->GetID(), iError);
				break;
			}
			// Retry
			printf("Floppy::Transfer(%2x): Retry #%i...\n", this->GetID(), iRetries);
		}

		this->ControlMotor(false);
		StopDMA(0x02);
		return -iError;
	}

	/****************************************************************************/
	/* Kernel methods 															*/
	/****************************************************************************/

	void FloppyHandler(void*)
	{
		//Console.WriteLine( "FloppyHandler( ): [IRQ%d]Floppy", oRegs->int_no );
	}

	void FloppyDrive::install()
	{
		printf("InstallFloppy( ): Installing floppy...\n");
	    install_irq_handler(FLOPPY_IRQ, &FloppyHandler);
	}

	/****************************************************************************/
	/* Static methods 															*/
	/****************************************************************************/

	static void WaitFloppyReady()
	{
		unsigned int iTimeout = 32;
		while (iTimeout-- > 0)
		{
		     if ((inb(FLOPPY_BASE + FLOPPY_MAIN_STATUS_REG) & 0xd0) != 0xd0)
			 {
		        break;
			 }
		     inb(FLOPPY_BASE + FLOPPY_DATA_REG);
		}
	}

	static void WriteFloppyCommand(char iCommand)
	{
		int i = 0;
		for ( ; i < 600; i++)
		{
			sleep(10);
			if (0x80 & inb(FLOPPY_BASE + FLOPPY_MAIN_STATUS_REG))
			{
				outb(FLOPPY_BASE + FLOPPY_DATA_REG, iCommand);
				return;
			}
		}
		printf("FloppyDrive::WriteFloppyCommand(): Write timeout!\n");
	}

	static unsigned char ReadFloppyData( )
	{
		int i = 0;
		for ( ; i < 600; i++)
		{
			sleep(10);
			if (0x80 & inb(FLOPPY_BASE + FLOPPY_MAIN_STATUS_REG))
			{
				return inb(FLOPPY_BASE + FLOPPY_DATA_REG);
			}
		}
		printf("FloppyDrive::ReadFloppyData(): Read timeout!\n");
		return 0;
	}

	static void TranslateSectorID(unsigned int iSectorID, unsigned char* iSector, unsigned char* iHead, unsigned char* iCylinder)
	{
		*iCylinder = (unsigned char)( iSectorID / 36 );
		iSectorID = iSectorID % 36;
		if (iSectorID > 18)
		{
			*iHead = 1;
			iSectorID -= 18;
		}
		else
		{
			*iHead = 0;
		}
		*iSector = (unsigned char)iSectorID;
		//printf( DEBUG_LEVEL_TRACE, "FloppyDrive::TranslateSectorID( )", "%d -> %d, %d, %d", iSectorID, *iCylinder, *iHead, *iSector );
	}
}

extern "C" void DetectFloppyDrives(unsigned char iMaxDrives )
{
	outb(0x70, 0x10);
	unsigned char iDriveTypes = inb(0x71);
	unsigned char iDriveNum = 0;

	if (iDriveNum < iMaxDrives)
	{
		IDriver* driver = new x86::FloppyDrive(iDriveNum, (iDriveTypes >> 4));
		iDriveNum++;
	}
	if ( iDriveNum < iMaxDrives )
	{
		IDriver* driver = new x86::FloppyDrive(iDriveNum, (iDriveTypes & 0xf));
		iDriveNum++;
	}
}
