#ifndef FLASH_H
#define FLASH_H
enum
{
	CMD_DATA_AREA_READ 	= 1,	
	CMD_DATA_AREA_WIRTE = 2,	
	CMD_DATA_AREA_ERASE_SECTOR = 3,
	CMD_CODE_AREA_READ = 5,	
	CMD_CODE_AREA_WRITE = 6,
	CMD_CODE_AREA_ERASE_SECTOR = 7,
};
enum
{
	CMD_CODE_AREA_UNLOCK = 0x29,
	CMD_DATA_AREA_UNLOCK = 0x2A,
	CMD_FLASH_LOCK = 0xAA,
};

 //¼Ä´æÆ÷FSCMDÎ»¶¨Òå
#define IFEN		(1<<7)


void Data_Area_Sector_Erase(unsigned char SectorNumber);
void Data_Area_Write_Byte(unsigned int Address,unsigned char Data);
void Data_Area_Mass_Write(unsigned int Address,unsigned char *pData,unsigned int Length);
unsigned char Data_Area_Read_Byte(unsigned int Address);
void Data_Area_Mass_Read(unsigned int Address,unsigned char *pData,unsigned int Length);
void Code_Area_Sector_Erase(unsigned char SectorNumber);
unsigned long int GetChipID(void);
#endif