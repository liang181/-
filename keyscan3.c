#include<reg52.h>
#include"lcd_1602.h"
#include"delay.h"
#include<intrins.h>
#define KeyPort P3

char local_date=0,base_date=0;//ȫ�ֱ�������ǰ��ͷλ��

/*unsigned char code *Main_Menu[]=
{
{"  ָ��ʶ��ϵͳ"},
{"  1.���ָ��"},
{"  2.����ָ��"},
{"  3.���ָ�ƿ�"}
};
*/


/***********************************************
�������ƣ�Delay_us
��    �ܣ�STC 1T��Ƭ��1us��ʱ����
��ڲ�����us:��ʱ��΢����
�� �� ֵ����	
��    ע���ڲ�ʱ��11.0592MHz
************************************************/
void Delayus(unsigned int us)
{
  	while( us--)
   	{
    	_nop_(); 
   	}             
}
/***********************************************
�������ƣ�Delay_ms
��    �ܣ�STC 1T��Ƭ��1ms��ʱ����
��ڲ�����ms:��ʱ�ĺ�����
�� �� ֵ����	
��    ע��ʾ����ʵ��1.05ms �ڲ�ʱ��11.0592MHz
************************************************/
void Delayms(unsigned int ms)
{
  	unsigned int i;
  	while( (ms--) != 0)
   	{
    	for(i = 0; i < 600; i++); 
   	}             
}
/*************************************
* �� �� ��: LCD1602_WriteCMD
* ��������: ��lcd1602дָ��
* ��ڲ���: cmd:ָ��
* ��    ��: ��
* ��    ע����
**************************************/
void LCD1602_WriteCMD(unsigned char cmd)
{
  	EN=0;
  	RS=0;
  	RW=0;
  	Delayus(10);
  	EN=1; 
  	Delayus(10);
  	DataPort=cmd;
  	Delayus(10);
  	EN=0;
}
/***********************************************
�������ƣ�LCD1602_WriteDAT
��    �ܣ���lcd1602д����
��ڲ�����dat������
�� �� ֵ����	
��    ע����
************************************************/
void LCD1602_WriteDAT(unsigned char dat)
{
  	EN=0;
  	RS=1;
  	RW=0;
  	Delayus(10);
  	EN=1; 
  	Delayus(10);
  	DataPort=dat;
  	Delayus(10);
  	EN=0;
}
/***********************************************
�������ƣ�LCD1602_CheckBusy
��    �ܣ����lcd1602æ�ź�
��ڲ�������
�� �� ֵ����	
��    ע����
************************************************/
void LCD1602_CheckBusy(void)
{
  	unsigned char temp;
	DataPort=0xff;		  //���������ø�,12cϵ�е�Ƭ����ת���˿�ģʽ
  	while(1)
  	{    
    	EN=0;			   
    	RS=0;
    	RW=1;
    	Delayus(10);
    	EN=1;
    	Delayus(10);
    	temp=DataPort;    //��ȡæͨ������
    	Delayus(10);
    	EN=0;
    	if((temp&0x80)!=0x80)
    	{    
      		break;
    	}
  	}
}
 /***********************************************
�������ƣ�LCD1602_Init
��    �ܣ�LCD1602��ʼ��
��ڲ�������
�� �� ֵ����	
��    ע����������ֲᶨ�塣
************************************************/
void LCD1602_Init(void)  
{
  	Delayms(15);      		//�ϵ���ʱ15ms
  	LCD1602_WriteCMD(0x38); //д��ʾָ��(�����æ�ź�)
  	Delayms(5);
  	LCD1602_WriteCMD(0x38); //д��ʾָ��(�����æ�ź�)
  	Delayms(5);
  	LCD1602_WriteCMD(0x38); //д��ʾָ��(�����æ�ź�)
  	LCD1602_CheckBusy();
  	LCD1602_WriteCMD(0x38); //д��ʾָ��
  	LCD1602_CheckBusy();
  	LCD1602_WriteCMD(0x08); //��ʾ�ر�
  	LCD1602_CheckBusy();
  	LCD1602_WriteCMD(0x01); //����
  	LCD1602_CheckBusy();
  	LCD1602_WriteCMD(0x06); //��ʾ����ƶ�����
  	LCD1602_CheckBusy();
  	LCD1602_WriteCMD(0x0c); //��ʾ�����������  
}
 /***********************************************
�������ƣ�LCD1602_Display
��    �ܣ�д���ֽ��ַ�
��ڲ�����addr��ʼ��ַ��pointerָ���ַ��index�±꣬num����
�� �� ֵ����	
��    ע����
************************************************/
void LCD1602_Display(unsigned char addr,unsigned char* pointer,unsigned char index,unsigned char num)
{
  	unsigned char i;
  	LCD1602_CheckBusy();	//�ж�æ�ź�
  	LCD1602_WriteCMD(addr);	//д���ַ
  	for(i=0;i<num;i++)		//д������
  	{
     	LCD1602_CheckBusy();			   //�ж�æ�ź�
     	LCD1602_WriteDAT(pointer[index+i]);//д������     
  	}
}
unsigned char keyscan()
{
	unsigned char ve;
	KeyPort=0x0f;
	if(KeyPort!=0x0f)
	{
		switch(KeyPort)
		{
			case 0x0e: ve=0; break;
			case 0x0d: ve=1; break;
			case 0x0b: ve=2; break;
			case 0x07: ve=3; break;
		}
		KeyPort=0xf0;
		switch(KeyPort)
		{
			case 0xe0: ve=ve; break;
			case 0xd0: ve+=4; break;
			case 0xb0: ve+=8; break;
			case 0x70: ve+=12; break;
		}		
	}
	return ve;
}



unsigned char table1[]="Input StudentNum";
unsigned char table[];

void main()
{
	unsigned char i;
	LCD1602_Init();
	LCD1602_Display(0x80,"StudentNum  Test",0,16);
	LCD1602_Display(0xc0,"   Loading...   ",0,16);
	Delayms(200);
	LCD1602_WriteCMD(0x01);
	while(1)
	{
		
	
		for(i=0;i<16;i++)
		{
			LCD1602_WriteCMD(0x80+i);
			LCD1602_WriteDAT(table1[i]);
		}
		LCD1602_Display(0xc2,"num",0,3);
		LCD1602_WriteCMD(0xc0+0x06);
		LCD1602_WriteDAT(keyscan()+'0');

	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
}
	