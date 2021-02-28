#include <reg52.h>
#include <intrins.h>
#include "delay.h"
#include "uart.h"
#include "buzz.h"
#include "key.h"
#include "lcd_1602.h"
#include "as608.h"

/*delay*/
void Delay_Us(int i)    
{
	while(--i);
}

void Delay_Ms(int m)
{
 	while(m--)
	{
	 	Delay_Us(125);
	}
}

/*uart*/
void Uart_Init(void)
{
    SCON=0x50;   //UART��ʽ1:8λUART;   REN=1:������� 
    PCON=0x00;   //SMOD=0:�����ʲ��ӱ� 
    TMOD=0x20;   //T1��ʽ2,����UART������ 
    TH1=0xFD; 
    TL1=0xFD;   //UART����������:FDFD��9600;FFFF,57600
    TR1=1;	 //����T1���� 
    EA=1;
}


void Uart_Send_Byte(unsigned char c)//UART Send a byte
{
	SBUF = c;
	while(!TI);		//������Ϊ1 
	TI = 0;
}

unsigned char Uart_Receive_Byte()//UART Receive a byteg
{	
	unsigned char dat;
	while(!RI);	 //������Ϊ1 
	RI = 0;
	dat = SBUF;
	return (dat);
}

/*buzz*/
void Buzz_Times(unsigned char times)
{
	unsigned char i=0;
	for(i=0;i<times;i++)
	{
		 buzz=0;
		 Delay_Ms(200);
		 buzz=1;
		 Delay_Ms(200);
	}
}

/*key*/
void Key_Init(void)
{
    //���尴������˿�
	KEY_DOWN=1;
	KEY_OK=1;
	KEY_CANCEL=1;
}

/*lcd1602*/
char local_date=0,base_date=0;//ȫ�ֱ�������ǰ��ͷλ��

unsigned char code *Main_Menu[]=
{
{"  ָ��ʶ��ϵͳ"},
{"  1.���ָ��"},
{"  2.����ָ��"},
{"  3.���ָ�ƿ�"}
};
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

/*as608*/
volatile unsigned char FPM10A_RECEICE_BUFFER[32];
unsigned int finger_id = 0;

//FINGERPRINTͨ��Э�鶨��

code unsigned char FPM10A_Get_Device[10] ={0x01,0x00,0x07,0x13,0x00,0x00,0x00,0x00,0x00,0x1b};//������֤
code unsigned char FPM10A_Pack_Head[6] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF};  //Э���ͷ
code unsigned char FPM10A_Get_Img[6] = {0x01,0x00,0x03,0x01,0x00,0x05};    //���ָ��ͼ��
code unsigned char FPM10A_Get_Templete_Count[6] ={0x01,0x00,0x03,0x1D,0x00,0x21 }; //���ģ������
code unsigned char FPM10A_Search[11]={0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x03,0xE7,0x00,0xF8}; //����ָ��������Χ0 - 999,ʹ��BUFFER1�е�����������
code unsigned char FPM10A_Search_0_9[11]={0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x00,0x13,0x00,0x21}; //����0-9��ָ��
code unsigned char FPM10A_Img_To_Buffer1[7]={0x01,0x00,0x04,0x02,0x01,0x00,0x08}; //��ͼ����뵽BUFFER1
code unsigned char FPM10A_Img_To_Buffer2[7]={0x01,0x00,0x04,0x02,0x02,0x00,0x09}; //��ͼ����뵽BUFFER2
code unsigned char FPM10A_Reg_Model[6]={0x01,0x00,0x03,0x05,0x00,0x09}; //��BUFFER1��BUFFER2�ϳ�����ģ��
code unsigned char FPM10A_Delete_All_Model[6]={0x01,0x00,0x03,0x0d,0x00,0x11};//ɾ��ָ��ģ�������е�ģ��
volatile unsigned char  FPM10A_Save_Finger[9]={0x01,0x00,0x06,0x06,0x01,0x00,0x0B,0x00,0x19};//��BUFFER1�е��������ŵ�ָ����λ��
//volatile:ϵͳ�������´������ڵ��ڴ��ȡ���ݣ���ʹ��ǰ���ָ��ոմӸô���ȡ������
/*------------------ FINGERPRINT������ --------------------------*/
 //���Ͱ�ͷ
void FPM10A_Cmd_Send_Pack_Head(void)
{
	int i;	
	for(i=0;i<6;i++) //��ͷ
   {
     Uart_Send_Byte(FPM10A_Pack_Head[i]);   
    }		
}
//����ָ��
void FPM10A_Cmd_Check(void)
{
	int i=0;
	FPM10A_Cmd_Send_Pack_Head(); //����ͨ��Э���ͷ
	for(i=0;i<10;i++)
	 {		
		Uart_Send_Byte(FPM10A_Get_Device[i]);
	  }
}
//���շ������ݻ���
void FPM10A_Receive_Data(unsigned char ucLength)
{
  unsigned char i;

  for (i=0;i<ucLength;i++)
     FPM10A_RECEICE_BUFFER[i] = Uart_Receive_Byte();

}

//FINGERPRINT_���ָ��ͼ������
void FPM10A_Cmd_Get_Img(void)
{
    unsigned char i;
    FPM10A_Cmd_Send_Pack_Head(); //����ͨ��Э���ͷ
    for(i=0;i<6;i++) //�������� 0x1d
	{
       Uart_Send_Byte(FPM10A_Get_Img[i]);
	}
}
//��ͼ��ת��������������Buffer1��
void FINGERPRINT_Cmd_Img_To_Buffer1(void)
{
 	unsigned char i;
	FPM10A_Cmd_Send_Pack_Head(); //����ͨ��Э���ͷ      
   	for(i=0;i<7;i++)   //�������� ��ͼ��ת���� ������ ����� CHAR_buffer1
     {
      Uart_Send_Byte(FPM10A_Img_To_Buffer1[i]);
   	  }
}
//��ͼ��ת��������������Buffer2��
void FINGERPRINT_Cmd_Img_To_Buffer2(void)
{
     unsigned char i;
     for(i=0;i<6;i++)    //���Ͱ�ͷ
	 {
    	Uart_Send_Byte(FPM10A_Pack_Head[i]);   
   	 }
     for(i=0;i<7;i++)   //�������� ��ͼ��ת���� ������ ����� CHAR_buffer1
      {
      	Uart_Send_Byte(FPM10A_Img_To_Buffer2[i]);
   	  }
}
//����ȫ���û�999ö
void FPM10A_Cmd_Search_Finger(void)
{
       unsigned char i;	   	    
			 FPM10A_Cmd_Send_Pack_Head(); //����ͨ��Э���ͷ
       for(i=0;i<11;i++)
           {
    	      Uart_Send_Byte(FPM10A_Search[i]);   
   		   }
}

void FPM10A_Cmd_Reg_Model(void)
{
       unsigned char i;	   
	    
			 FPM10A_Cmd_Send_Pack_Head(); //����ͨ��Э���ͷ

       for(i=0;i<6;i++)
           {
    	      Uart_Send_Byte(FPM10A_Reg_Model[i]);   
   		   }


}
//ɾ��ָ��ģ���������ָ��ģ��
void FINGERPRINT_Cmd_Delete_All_Model(void)
{
     unsigned char i;    
    for(i=0;i<6;i++) //��ͷ
      Uart_Send_Byte(FPM10A_Pack_Head[i]);   
    for(i=0;i<6;i++) //����ϲ�ָ��ģ��
	   {
      Uart_Send_Byte(FPM10A_Delete_All_Model[i]);   
		 }	
}
//����ָ��
void FPM10A_Cmd_Save_Finger( unsigned int storeID )
{
       unsigned long temp = 0;
		   unsigned char i;
       FPM10A_Save_Finger[5] =(storeID&0xFF00)>>8;
       FPM10A_Save_Finger[6] = (storeID&0x00FF);
		   for(i=0;i<7;i++)   //����У���
		   	   temp = temp + FPM10A_Save_Finger[i]; 
		   FPM10A_Save_Finger[7]=(temp & 0x00FF00) >> 8; //���У������
		   FPM10A_Save_Finger[8]= temp & 0x0000FF;		   
       FPM10A_Cmd_Send_Pack_Head(); //����ͨ��Э���ͷ	
       for(i=0;i<9;i++)  
      		Uart_Send_Byte(FPM10A_Save_Finger[i]);      //�������� ��ͼ��ת���� ������ ����� CHAR_buffer1
}
//���ָ��
void FPM10A_Add_Fingerprint()
{
	unsigned char id_show[]={0,0,0};
	LCD1602_WriteCMD(0x01); //����
	finger_id=0;  
	while(1)
	{
	LCD1602_Display(0x80,"   Add  finger  ",0,16);
	LCD1602_Display(0xc0,"    ID is       ",0,16);
	//�����ؼ�ֱ�ӻص����˵�
	if(KEY_CANCEL == 0) 
	{
		 while(KEY_CANCEL==0);
		 break;
	}

	//���л���ָ��iDֵ��1
	if(KEY_DOWN == 0)
	{
		while(KEY_DOWN==0);
		if(finger_id == 1000)
		{
			finger_id = 0;
		}
		else
		finger_id = finger_id + 1;
	}

	 //ָ��iDֵ��ʾ���� 
	 LCD1602_WriteCMD(0xc0+10);
	 LCD1602_WriteDAT(finger_id/100+48);
	 LCD1602_WriteDAT(finger_id%100/10+48);
	 LCD1602_WriteDAT(finger_id%100%10+48);

	 //��ȷ�ϼ���ʼ¼��ָ����Ϣ 		 			
	 if(KEY_OK == 0)
	  {	
			  while(KEY_OK==0);
			  LCD1602_Display(0x80,"Fingerprint plz!",0,16);
			  LCD1602_Display(0xc0,"                ",0,16);
			while(KEY_CANCEL == 1)
		   {
			  //���·��ؼ��˳�¼�뷵��fingerID����״̬   
				if(KEY_CANCEL == 0) 
				 {
				  while(KEY_CANCEL==0);
				  break;
				  }
				FPM10A_Cmd_Get_Img(); //���ָ��ͼ��
				FPM10A_Receive_Data(12);
				//�жϽ��յ���ȷ����,����0ָ�ƻ�ȡ�ɹ�
				if(FPM10A_RECEICE_BUFFER[9]==0)
				 {
					Delay_Ms(100);
					FINGERPRINT_Cmd_Img_To_Buffer1();
				    FPM10A_Receive_Data(12);
					LCD1602_Display(0x80,"Successful entry",0,16);
					Buzz_Times(1);
					Delay_Ms(1000);
					LCD1602_Display(0x80,"Fingerprint plz!",0,16);
			  		LCD1602_Display(0xc0,"                ",0,16);
					while(1)
					{
						if(KEY_CANCEL == 0) 
						{
							while(KEY_CANCEL==0);
							break;
						}
					 FPM10A_Cmd_Get_Img(); //���ָ��ͼ��
					 FPM10A_Receive_Data(12);
					//�жϽ��յ���ȷ����,����0ָ�ƻ�ȡ�ɹ�
					if(FPM10A_RECEICE_BUFFER[9]==0)
					{
						Delay_Ms(200);
						LCD1602_Display(0x80,"Successful entry",0,16);
						LCD1602_Display(0xc0,"    ID is       ",0,16);
						 //ָ��iDֵ��ʾ���� 
						 LCD1602_WriteCMD(0xc0+10);
						 LCD1602_WriteDAT(finger_id/100+48);
						 LCD1602_WriteDAT(finger_id%100/10+48);
						 LCD1602_WriteDAT(finger_id%100%10+48);
						FINGERPRINT_Cmd_Img_To_Buffer2();
				  		FPM10A_Receive_Data(12);
						FPM10A_Cmd_Reg_Model();//ת����������
	         			FPM10A_Receive_Data(12); 
					  	FPM10A_Cmd_Save_Finger(finger_id);                		         
	          			FPM10A_Receive_Data(12);
						Buzz_Times(1);
						Delay_Ms(1000);
						finger_id=finger_id+1;
				    	break;
				  	}
				   }
				   
	        		break;
					}
				}
//			ClrScreen(); //�����ʾ��
			}
		//	Delay_Ms(500);
		}
}

//����ָ��
void FPM10A_Find_Fingerprint()
{
	unsigned int find_fingerid = 0;
	unsigned char id_show[]={0,0,0};
	do
	{
		LCD1602_Display(0x80,"Fingerprint plz!",0,16);
		LCD1602_Display(0xc0,"                ",0,16);
		FPM10A_Cmd_Get_Img(); //���ָ��ͼ��
		FPM10A_Receive_Data(12);		
		//�жϽ��յ���ȷ����,����0ָ�ƻ�ȡ�ɹ�
		if(FPM10A_RECEICE_BUFFER[9]==0)
		{			
			Delay_Ms(100);
			FINGERPRINT_Cmd_Img_To_Buffer1();
			FPM10A_Receive_Data(12);		
			FPM10A_Cmd_Search_Finger();
			FPM10A_Receive_Data(16);			
			if(FPM10A_RECEICE_BUFFER[9] == 0) //������  
			{
				LCD1602_Display(0x80," Search success ",0,16);
				LCD1602_Display(0xc0,"    ID is       ",0,16);
				Buzz_Times(1);					
				//ƴ��ָ��ID��
				find_fingerid = FPM10A_RECEICE_BUFFER[10]*256 + FPM10A_RECEICE_BUFFER[11];					
				 //ָ��iDֵ��ʾ���� 
				 LCD1602_WriteCMD(0xc0+10);
				 LCD1602_WriteDAT(find_fingerid/100+48);
				 LCD1602_WriteDAT(find_fingerid%100/10+48);
				 LCD1602_WriteDAT(find_fingerid%100%10+48);						
				Delay_Ms(2000);				
			   }
				else //û���ҵ�
				{
					LCD1602_Display(0x80," Search  failed ",0,16);
					LCD1602_Display(0xc0,"                ",0,16);
				 	Buzz_Times(3);
				}
			}		
		}while(KEY_CANCEL == 1);
}
//ɾ�����д�����ָ�ƿ�
void FPM10A_Delete_All_Fingerprint()
{
		unsigned char i=0;
				LCD1602_Display(0x80,"   empty all    ",0,16);
				LCD1602_Display(0xc0,"   Yes or no ?  ",0,16); 
		do
		 {
			if(KEY_OK==0)
			{
				while(KEY_OK==0);
				LCD1602_Display(0x80,"   emptying     ",0,16);
				LCD1602_Display(0xc0,"                ",0,16); 
				Delay_Ms(300);
				LCD1602_WriteCMD(0xc0);
				for(i=0;i<16;i++)
				 {
					LCD1602_WriteDAT(42);
					Delay_Ms(100);
				 }
				FINGERPRINT_Cmd_Delete_All_Model();
			  FPM10A_Receive_Data(12); 
				LCD1602_Display(0x80,"   All empty    ",0,16);
				LCD1602_Display(0xc0,"                ",0,16);
				Buzz_Times(3);
				break;
			}
		 }while(KEY_CANCEL==1);
}

void Device_Check(void)
{
		unsigned char i=0;
		FPM10A_RECEICE_BUFFER[9]=1;				           //��������ھ�λ���ж��Ƿ�ͨ������
		LCD1602_Display(0xc0,"Loading",0,7);	           //�豸�����н���							   
		for(i=0;i<8;i++)						           //������ʽ���£�����������
		{
			LCD1602_WriteDAT(42);	                       //42��ӦASIC��� *
			Delay_Ms(200);						           //���ƽ������ٶ�
		}									
		LCD1602_Display(0xc0,"Docking  failure",0,16);      //Һ������ʾ�Խ�ʧ�ܣ����ָ��ģ���ԵĻ��Ὣ�串��	
		FPM10A_Cmd_Check();									//��Ƭ����ָ��ģ�鷢��У������
		FPM10A_Receive_Data(12);							//�����ڽ��յ�������ת��
 		if(FPM10A_RECEICE_BUFFER[9] == 0)					//�ж����ݵ͵�9λ�Ƿ���յ�0
		{
			LCD1602_Display(0xc0,"Docking  success",0,16);	//���ϳɹ���������ʾ�Խӳɹ�
		}
}

/*main*/
void main()
{	
	LCD1602_Init();			//��ʼ��Һ��
	LCD1602_Display(0x80,"  Fingerprint   ",0,16);	 //Һ��������ʾ����
  	Uart_Init();			//��ʼ������
	Key_Init();				//��ʼ������
 	Delay_Ms(200);          //��ʱ500MS���ȴ�ָ��ģ�鸴λ
	Device_Check();		   	//У��ָ��ģ���Ƿ������ȷ��Һ��������Ӧ����ʾ
	Delay_Ms(1000);			//�Խӳɹ�����ͣ��һ��ʱ��
	while(1)
	{
	    /**************���������ܽ���****************/
		LCD1602_Display(0x80,"  search finger ",0,16);	 //��һ����ʾ����ָ��
		LCD1602_Display(0xc0,"  Add     Del   ",0,16);	 //��Ӻ�ɾ��ָ��
		if(local_date==0)
		{
			LCD1602_Display(0x80,  " *",0,2);
			LCD1602_Display(0xc0,  "  ",0,2);
			LCD1602_Display(0xc0+8,"  ",0,2);	
		}
		else if(local_date==1)
		{
			LCD1602_Display(0x80,  "  ",0,2);
			LCD1602_Display(0xc0,  " *",0,2);
			LCD1602_Display(0xc0+8,"  ",0,2);	
		}
		else if(local_date==2)
		{
			LCD1602_Display(0x80,  "  ",0,2);
			LCD1602_Display(0xc0,  "  ",0,2);
			LCD1602_Display(0xc0+8," *",0,2);	
		}			
		//ȷ�ϼ�
		if(KEY_OK == 0)
		{	 
		 	while(KEY_OK == 0);//�ȴ��ɿ�����								
			switch(local_date)
			{
					case 0:  //����ָ��						
					FPM10A_Find_Fingerprint();																								
					break;	
					
					case 1:	 //���ָ��
					FPM10A_Add_Fingerprint();
					break; 					
					
					case 2:	//���ָ��
					FPM10A_Delete_All_Fingerprint();
		  			break;
			}
		}
		    //�л���
			if(KEY_DOWN == 0)
			{
			 	while(KEY_DOWN == 0); //�ȴ��ɿ�����				
	  	 		if(local_date<=2)
				{
					local_date++;
					if(local_date==3)
						local_date=0;						
				}		
			}						
			Delay_Ms(100); //��ʱ�ж�100MS���һ��		
	}
}




