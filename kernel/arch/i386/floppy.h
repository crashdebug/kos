#ifndef FLOPPY_H
#define FLOPPY_H

#include <idriver.h>

#define FLOPPY_PARAMETER_ADDRESS 0x000fefc7

// standard base address of the primary floppy controller
#define FLOPPY_BASE 0x03f0
// standard IRQ number for floppy controllers
#define FLOPPY_IRQ 6

// The registers of interest. There are more, but we only use these here.
#define FLOPPY_DIGITAL_OUTPUT_REG		2	// Digital output register
#define FLOPPY_MAIN_STATUS_REG			4	// Master status register, read only
#define FLOPPY_DATA_REG					5	// Data FIFO, in DMA operation for commands
#define FLOPPY_CONFIG_CONTROL_REG		7	// Configuration control register, write only

// The commands of interest. There are more, but we only use these here.
#define FLOPPY_CMD_SPECIFY				0x03
#define FLOPPY_CMD_CHECK_STATUS			0x04
#define FLOPPY_CMD_WRITE_DATA			0x05
#define FLOPPY_CMD_READ_DATA			0x06
#define FLOPPY_CMD_RECALIBRATE			0x07
#define FLOPPY_CMD_SENSE_INTERRUPT		0x08
#define FLOPPY_CMD_SEEK					0x0f
#define FLOPPY_CMD_READ_TRACK			0x42
#define FLOPPY_CMD_FORMAT_TRACK			0x4d
#define FLOPPY_CMD_READ_SECTOR_ID		0x4a
#define FLOPPY_CMD_READ_SECTOR			0xc6
#define FLOPPY_CMD_WRITE_SECTOR			0xc5
#define FLOPPY_CMD_WRITE_DELETE_SECTOR	0xc9
#define FLOPPY_CMD_READ_DELETE_SECTOR	0xcc

// The address where BIOS stores floppy drive information
#define FLOPPY_PARAMETER_ADDRESS		0x000fefc7

#define FLOPPY_RETRY_COUNT				5

// "none"
#define FLOPPY_TYPE_NONE	0
// "360kB 5.25\""
#define FLOPPY_TYPE_SS_SD	1
// "1.2MB 5.25\""
#define FLOPPY_TYPE_DS_SD	2
// "720kB 3.5\""
#define FLOPPY_TYPE_SS_DD	3
// "1.44MB 3.5\""
#define FLOPPY_TYPE_DS_DD	4
// "2.88MB 3.5\""
#define FLOPPY_TYPE_DS_QD	5
// "unknown type"
#define FLOPPY_TYPE_UNKN0	6
// "unknown type"
#define FLOPPY_TYPE_UNKN1	7

namespace kos::x86
{
	struct FloppyParameters
	{
	  unsigned char iStepRateHeadUnload;
	  unsigned char iHeadLoadNDMA;
	  // Specified in clock ticks
	  unsigned char iMotorDelayOff;
	  unsigned char iBytesPerSector;
	  unsigned char iSectorsPerTrack;
	  unsigned char iGapLength;
	  // Used only when bytes per sector == 0
	  unsigned char iDataLength;
	  unsigned char iFormatGapLength;
	  unsigned char x;
	  // Specified in milliseconds
	  unsigned char iHeadSettleTime;
	  // Specified in 1/8 seconds
	  unsigned char iMotorStartTime;
	}__attribute__((packed));

	class FloppyDrive : public IDriver
	{
		public:
			FloppyDrive(unsigned char iDrive, unsigned char eType);
			~FloppyDrive();

			void install();

			// Drive
			unsigned char GetID() const;
			unsigned char* GetDescription() const;
			const char* GetName() const;
			unsigned int GetMaxSectors() const;
			bool IsInstalled() const;
			unsigned int ReadSector(unsigned int iSectorID, void* aBuffer);
			unsigned int Seek(unsigned int iSectorID);
			unsigned int WriteSector(unsigned int iSectorID, void* aBuffer);
			bool Reset();
			unsigned int GetSectorSize() const;
			unsigned int ReadData(unsigned int iPosition, void* aBuffer, unsigned int iOffset, unsigned int iLen);
			unsigned int WriteData(unsigned int iPosition, void* aBuffer, unsigned int iOffset, unsigned int iLen);

			//static void InstallFloppy( );
			//static void DetectFloppyDrives( Kernel::Drive ** aDrives, unsigned char iMaxDrives );

			// FilesystemDevice
//			Device ** GetDevices( );
//			Path ** GetChildren( );
//			bool HasDevices( );
//			bool HasChildren( );

		private:
			bool Calibrate();
			void CheckInterrupt(unsigned char* iStatus, unsigned char* iCylinder);
			void ControlMotor(bool bTurnOn);
			int ReadSector(unsigned char iSector, unsigned char iHead, unsigned char iCylinder, void* aBuffer);
			bool Seek(unsigned char iHead, unsigned char iCylinder);
			int Transfer(unsigned char iCommand, unsigned char iDMAMode, unsigned char iSector, unsigned char iHead, unsigned char iCylinder, void* aBuffer);
			int WriteSector(unsigned char iSector, unsigned char iHead, unsigned char iCylinder, void* aBuffer);
			unsigned char eType;
			// True if motor is on, false if it's off
			bool bMotorOn;
			char sName[4];
			unsigned char id;
			//static Kernel::Drive ** aDrives;
	};

	static void WriteFloppyCommand(char iCommand);
	static void WaitFloppyReady();
	static unsigned char ReadFloppyData();
	static void TranslateSectorID(unsigned int iSectorID, unsigned char* iSector, unsigned char* iHead, unsigned char* iCylinder);
}

#endif
