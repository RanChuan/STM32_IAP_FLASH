#include "sys.h"
#include "malloc.h"
#include "flash.h"
#include "stmflash.h"

#define SHCSR    (*(u32 *)0xE000ED24)



const char IAP_Version[]="IAP BY FLASH V1.0";
const char IAP_CompileTime[]=__DATE__ " -- " __TIME__;




//flash�洢ӳ����Ϣ		
typedef struct 
{
	u32 appFlashAddr;
	u32 appStm32Addr;
	u32 appSize;
	u32 fatAddr;
	u32 fatStm32Addr;
	u32 fatSize;
	u32 dataAddr;
	u32 dataStm32Addr;
	u32 dataSize;
} IapInfoDef;
		
		


//ִ���û�����
void jump_app(void);







int main()
{
	SHCSR|=7<<16;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2

	mem_init();
	SPI_Flash_Init();
	if (IAP_CMD==IAP_CMD_FLASH)
	{
		IapInfoDef iapinfo={0};
		SPI_Flash_Read ((u8*)&iapinfo,0,sizeof (iapinfo));
		if (iapinfo.appFlashAddr!=0xffffffff)
		{
			u8 *databuff=mymalloc (2048);
			u32 stm32addr=iapinfo.appStm32Addr;
			u32 flashaddr=iapinfo.appFlashAddr;
			u32 datapacksize=2048;
			u32 laftdatasize=iapinfo.appSize;
			while (1)
			{
				SPI_Flash_Read ((u8*)databuff,flashaddr,datapacksize);
				flashaddr+=datapacksize;
				STMFLASH_Write(stm32addr,(u16 *)databuff,datapacksize/2);
				stm32addr+=datapacksize;
				if (laftdatasize>2048)
				{
					datapacksize=2048;
					laftdatasize-=2048;
				}
				else if (laftdatasize>0)
				{
					datapacksize=laftdatasize;
					laftdatasize=0;
				}
				else
				{				
					break;
				}
			}
		}
		IAP_CMD=0;
		jump_app();
	}
	else
	{
		IAP_CMD=0;
		jump_app();
	}
}



//�����û��������ʼ��ַ
void (*user_main)(void);


void jump_app(void)
{
	if(((*(vu32*)0x8002800)&0x2FFE0000)==0x20000000)	//���ջ����ַ�Ƿ�Ϸ�.
	{
		//NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x2800);		
		user_main=(void (*)(void))*(vu32*)(0x8002800+4);		//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
		__set_MSP(*(vu32*)0x8002800);					//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
		user_main();									//��ת��APP.
	}
	
}






