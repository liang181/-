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
    SCON=0x50;   //UART方式1:8位UART;   REN=1:允许接收 
    PCON=0x00;   //SMOD=0:波特率不加倍 
    TMOD=0x20;   //T1方式2,用于UART波特率 
    TH1=0xFD; 
    TL1=0xFD;   //UART波特率设置:FDFD，9600;FFFF,57600
    TR1=1;	 //允许T1计数 
    EA=1;
}


void Uart_Send_Byte(unsigned char c)//UART Send a byte
{
	SBUF = c;
	while(!TI);		//发送完为1 
	TI = 0;
}

unsigned char Uart_Receive_Byte()//UART Receive a byteg
{	
	unsigned char dat;
	while(!RI);	 //接收完为1 
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
    //定义按键输入端口
	KEY_DOWN=1;
	KEY_OK=1;
	KEY_CANCEL=1;
}

/*lcd1602*/
char local_date=0,base_date=0;//全局变量，当前箭头位置

unsigned char code *Main_Menu[]=
{
{"  指纹识别系统"},
{"  1.添加指纹"},
{"  2.搜索指纹"},
{"  3.清空指纹库"}
};
/***********************************************
函数名称：Delay_us
功    能：STC 1T单片机1us延时程序
入口参数：us:延时的微秒数
返 回 值：无	
备    注：内部时钟11.0592MHz
************************************************/
void Delayus(unsigned int us)
{
  	while( us--)
   	{
    	_nop_(); 
   	}             
}
/***********************************************
函数名称：Delay_ms
功    能：STC 1T单片机1ms延时程序
入口参数：ms:延时的毫秒数
返 回 值：无	
备    注：示波器实测1.05ms 内部时钟11.0592MHz
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
* 函 数 名: LCD1602_WriteCMD
* 函数功能: 向lcd1602写指令
* 入口参数: cmd:指令
* 返    回: 无
* 备    注：无
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
函数名称：LCD1602_WriteDAT
功    能：向lcd1602写数据
入口参数：dat：数据
返 回 值：无	
备    注：无
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
函数名称：LCD1602_CheckBusy
功    能：检测lcd1602忙信号
入口参数：无
返 回 值：无	
备    注：无
************************************************/
void LCD1602_CheckBusy(void)
{
  	unsigned char temp;
	DataPort=0xff;		  //做输入先置高,12c系列单片机需转换端口模式
  	while(1)
  	{    
    	EN=0;			   
    	RS=0;
    	RW=1;
    	Delayus(10);
    	EN=1;
    	Delayus(10);
    	temp=DataPort;    //读取忙通道数据
    	Delayus(10);
    	EN=0;
    	if((temp&0x80)!=0x80)
    	{    
      		break;
    	}
  	}
}
 /***********************************************
函数名称：LCD1602_Init
功    能：LCD1602初始化
入口参数：无
返 回 值：无	
备    注：详见数据手册定义。
************************************************/
void LCD1602_Init(void)  
{
  	Delayms(15);      		//上电延时15ms
  	LCD1602_WriteCMD(0x38); //写显示指令(不检测忙信号)
  	Delayms(5);
  	LCD1602_WriteCMD(0x38); //写显示指令(不检测忙信号)
  	Delayms(5);
  	LCD1602_WriteCMD(0x38); //写显示指令(不检测忙信号)
  	LCD1602_CheckBusy();
  	LCD1602_WriteCMD(0x38); //写显示指令
  	LCD1602_CheckBusy();
  	LCD1602_WriteCMD(0x08); //显示关闭
  	LCD1602_CheckBusy();
  	LCD1602_WriteCMD(0x01); //清屏
  	LCD1602_CheckBusy();
  	LCD1602_WriteCMD(0x06); //显示光标移动设置
  	LCD1602_CheckBusy();
  	LCD1602_WriteCMD(0x0c); //显示开及光标设置  
}
 /***********************************************
函数名称：LCD1602_Display
功    能：写多字节字符
入口参数：addr起始地址，pointer指针地址，index下标，num个数
返 回 值：无	
备    注：无
************************************************/
void LCD1602_Display(unsigned char addr,unsigned char* pointer,unsigned char index,unsigned char num)
{
  	unsigned char i;
  	LCD1602_CheckBusy();	//判断忙信号
  	LCD1602_WriteCMD(addr);	//写入地址
  	for(i=0;i<num;i++)		//写入数据
  	{
     	LCD1602_CheckBusy();			   //判断忙信号
     	LCD1602_WriteDAT(pointer[index+i]);//写入数据     
  	}
}

/*as608*/
volatile unsigned char FPM10A_RECEICE_BUFFER[32];
unsigned int finger_id = 0;

//FINGERPRINT通信协议定义

code unsigned char FPM10A_Get_Device[10] ={0x01,0x00,0x07,0x13,0x00,0x00,0x00,0x00,0x00,0x1b};//口令验证
code unsigned char FPM10A_Pack_Head[6] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF};  //协议包头
code unsigned char FPM10A_Get_Img[6] = {0x01,0x00,0x03,0x01,0x00,0x05};    //获得指纹图像
code unsigned char FPM10A_Get_Templete_Count[6] ={0x01,0x00,0x03,0x1D,0x00,0x21 }; //获得模版总数
code unsigned char FPM10A_Search[11]={0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x03,0xE7,0x00,0xF8}; //搜索指纹搜索范围0 - 999,使用BUFFER1中的特征码搜索
code unsigned char FPM10A_Search_0_9[11]={0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x00,0x13,0x00,0x21}; //搜索0-9号指纹
code unsigned char FPM10A_Img_To_Buffer1[7]={0x01,0x00,0x04,0x02,0x01,0x00,0x08}; //将图像放入到BUFFER1
code unsigned char FPM10A_Img_To_Buffer2[7]={0x01,0x00,0x04,0x02,0x02,0x00,0x09}; //将图像放入到BUFFER2
code unsigned char FPM10A_Reg_Model[6]={0x01,0x00,0x03,0x05,0x00,0x09}; //将BUFFER1跟BUFFER2合成特征模版
code unsigned char FPM10A_Delete_All_Model[6]={0x01,0x00,0x03,0x0d,0x00,0x11};//删除指纹模块里所有的模版
volatile unsigned char  FPM10A_Save_Finger[9]={0x01,0x00,0x06,0x06,0x01,0x00,0x0B,0x00,0x19};//将BUFFER1中的特征码存放到指定的位置
//volatile:系统总是重新从它所在的内存读取数据，即使它前面的指令刚刚从该处读取过数据
/*------------------ FINGERPRINT命令字 --------------------------*/
 //发送包头
void FPM10A_Cmd_Send_Pack_Head(void)
{
	int i;	
	for(i=0;i<6;i++) //包头
   {
     Uart_Send_Byte(FPM10A_Pack_Head[i]);   
    }		
}
//发送指令
void FPM10A_Cmd_Check(void)
{
	int i=0;
	FPM10A_Cmd_Send_Pack_Head(); //发送通信协议包头
	for(i=0;i<10;i++)
	 {		
		Uart_Send_Byte(FPM10A_Get_Device[i]);
	  }
}
//接收反馈数据缓冲
void FPM10A_Receive_Data(unsigned char ucLength)
{
  unsigned char i;

  for (i=0;i<ucLength;i++)
     FPM10A_RECEICE_BUFFER[i] = Uart_Receive_Byte();

}

//FINGERPRINT_获得指纹图像命令
void FPM10A_Cmd_Get_Img(void)
{
    unsigned char i;
    FPM10A_Cmd_Send_Pack_Head(); //发送通信协议包头
    for(i=0;i<6;i++) //发送命令 0x1d
	{
       Uart_Send_Byte(FPM10A_Get_Img[i]);
	}
}
//讲图像转换成特征码存放在Buffer1中
void FINGERPRINT_Cmd_Img_To_Buffer1(void)
{
 	unsigned char i;
	FPM10A_Cmd_Send_Pack_Head(); //发送通信协议包头      
   	for(i=0;i<7;i++)   //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1
     {
      Uart_Send_Byte(FPM10A_Img_To_Buffer1[i]);
   	  }
}
//将图像转换成特征码存放在Buffer2中
void FINGERPRINT_Cmd_Img_To_Buffer2(void)
{
     unsigned char i;
     for(i=0;i<6;i++)    //发送包头
	 {
    	Uart_Send_Byte(FPM10A_Pack_Head[i]);   
   	 }
     for(i=0;i<7;i++)   //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1
      {
      	Uart_Send_Byte(FPM10A_Img_To_Buffer2[i]);
   	  }
}
//搜索全部用户999枚
void FPM10A_Cmd_Search_Finger(void)
{
       unsigned char i;	   	    
			 FPM10A_Cmd_Send_Pack_Head(); //发送通信协议包头
       for(i=0;i<11;i++)
           {
    	      Uart_Send_Byte(FPM10A_Search[i]);   
   		   }
}

void FPM10A_Cmd_Reg_Model(void)
{
       unsigned char i;	   
	    
			 FPM10A_Cmd_Send_Pack_Head(); //发送通信协议包头

       for(i=0;i<6;i++)
           {
    	      Uart_Send_Byte(FPM10A_Reg_Model[i]);   
   		   }


}
//删除指纹模块里的所有指纹模版
void FINGERPRINT_Cmd_Delete_All_Model(void)
{
     unsigned char i;    
    for(i=0;i<6;i++) //包头
      Uart_Send_Byte(FPM10A_Pack_Head[i]);   
    for(i=0;i<6;i++) //命令合并指纹模版
	   {
      Uart_Send_Byte(FPM10A_Delete_All_Model[i]);   
		 }	
}
//保存指纹
void FPM10A_Cmd_Save_Finger( unsigned int storeID )
{
       unsigned long temp = 0;
		   unsigned char i;
       FPM10A_Save_Finger[5] =(storeID&0xFF00)>>8;
       FPM10A_Save_Finger[6] = (storeID&0x00FF);
		   for(i=0;i<7;i++)   //计算校验和
		   	   temp = temp + FPM10A_Save_Finger[i]; 
		   FPM10A_Save_Finger[7]=(temp & 0x00FF00) >> 8; //存放校验数据
		   FPM10A_Save_Finger[8]= temp & 0x0000FF;		   
       FPM10A_Cmd_Send_Pack_Head(); //发送通信协议包头	
       for(i=0;i<9;i++)  
      		Uart_Send_Byte(FPM10A_Save_Finger[i]);      //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1
}
//添加指纹
void FPM10A_Add_Fingerprint()
{
	unsigned char id_show[]={0,0,0};
	LCD1602_WriteCMD(0x01); //清屏
	finger_id=0;  
	while(1)
	{
	LCD1602_Display(0x80,"   Add  finger  ",0,16);
	LCD1602_Display(0xc0,"    ID is       ",0,16);
	//按返回键直接回到主菜单
	if(KEY_CANCEL == 0) 
	{
		 while(KEY_CANCEL==0);
		 break;
	}

	//按切换键指纹iD值加1
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

	 //指纹iD值显示处理 
	 LCD1602_WriteCMD(0xc0+10);
	 LCD1602_WriteDAT(finger_id/100+48);
	 LCD1602_WriteDAT(finger_id%100/10+48);
	 LCD1602_WriteDAT(finger_id%100%10+48);

	 //按确认键开始录入指纹信息 		 			
	 if(KEY_OK == 0)
	  {	
			  while(KEY_OK==0);
			  LCD1602_Display(0x80,"Fingerprint plz!",0,16);
			  LCD1602_Display(0xc0,"                ",0,16);
			while(KEY_CANCEL == 1)
		   {
			  //按下返回键退出录入返回fingerID调整状态   
				if(KEY_CANCEL == 0) 
				 {
				  while(KEY_CANCEL==0);
				  break;
				  }
				FPM10A_Cmd_Get_Img(); //获得指纹图像
				FPM10A_Receive_Data(12);
				//判断接收到的确认码,等于0指纹获取成功
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
					 FPM10A_Cmd_Get_Img(); //获得指纹图像
					 FPM10A_Receive_Data(12);
					//判断接收到的确认码,等于0指纹获取成功
					if(FPM10A_RECEICE_BUFFER[9]==0)
					{
						Delay_Ms(200);
						LCD1602_Display(0x80,"Successful entry",0,16);
						LCD1602_Display(0xc0,"    ID is       ",0,16);
						 //指纹iD值显示处理 
						 LCD1602_WriteCMD(0xc0+10);
						 LCD1602_WriteDAT(finger_id/100+48);
						 LCD1602_WriteDAT(finger_id%100/10+48);
						 LCD1602_WriteDAT(finger_id%100%10+48);
						FINGERPRINT_Cmd_Img_To_Buffer2();
				  		FPM10A_Receive_Data(12);
						FPM10A_Cmd_Reg_Model();//转换成特征码
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
//			ClrScreen(); //清空显示屏
			}
		//	Delay_Ms(500);
		}
}

//搜索指纹
void FPM10A_Find_Fingerprint()
{
	unsigned int find_fingerid = 0;
	unsigned char id_show[]={0,0,0};
	do
	{
		LCD1602_Display(0x80,"Fingerprint plz!",0,16);
		LCD1602_Display(0xc0,"                ",0,16);
		FPM10A_Cmd_Get_Img(); //获得指纹图像
		FPM10A_Receive_Data(12);		
		//判断接收到的确认码,等于0指纹获取成功
		if(FPM10A_RECEICE_BUFFER[9]==0)
		{			
			Delay_Ms(100);
			FINGERPRINT_Cmd_Img_To_Buffer1();
			FPM10A_Receive_Data(12);		
			FPM10A_Cmd_Search_Finger();
			FPM10A_Receive_Data(16);			
			if(FPM10A_RECEICE_BUFFER[9] == 0) //搜索到  
			{
				LCD1602_Display(0x80," Search success ",0,16);
				LCD1602_Display(0xc0,"    ID is       ",0,16);
				Buzz_Times(1);					
				//拼接指纹ID数
				find_fingerid = FPM10A_RECEICE_BUFFER[10]*256 + FPM10A_RECEICE_BUFFER[11];					
				 //指纹iD值显示处理 
				 LCD1602_WriteCMD(0xc0+10);
				 LCD1602_WriteDAT(find_fingerid/100+48);
				 LCD1602_WriteDAT(find_fingerid%100/10+48);
				 LCD1602_WriteDAT(find_fingerid%100%10+48);						
				Delay_Ms(2000);				
			   }
				else //没有找到
				{
					LCD1602_Display(0x80," Search  failed ",0,16);
					LCD1602_Display(0xc0,"                ",0,16);
				 	Buzz_Times(3);
				}
			}		
		}while(KEY_CANCEL == 1);
}
//删除所有存贮的指纹库
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
		FPM10A_RECEICE_BUFFER[9]=1;				           //串口数组第九位可判断是否通信正常
		LCD1602_Display(0xc0,"Loading",0,7);	           //设备加载中界面							   
		for(i=0;i<8;i++)						           //进度条式更新，看起来美观
		{
			LCD1602_WriteDAT(42);	                       //42对应ASIC码的 *
			Delay_Ms(200);						           //控制进度条速度
		}									
		LCD1602_Display(0xc0,"Docking  failure",0,16);      //液晶先显示对接失败，如果指纹模块插对的话会将其覆盖	
		FPM10A_Cmd_Check();									//单片机向指纹模块发送校对命令
		FPM10A_Receive_Data(12);							//将串口接收到的数据转存
 		if(FPM10A_RECEICE_BUFFER[9] == 0)					//判断数据低第9位是否接收到0
		{
			LCD1602_Display(0xc0,"Docking  success",0,16);	//符合成功条件则显示对接成功
		}
}

/*main*/
void main()
{	
	LCD1602_Init();			//初始化液晶
	LCD1602_Display(0x80,"  Fingerprint   ",0,16);	 //液晶开机显示界面
  	Uart_Init();			//初始化串口
	Key_Init();				//初始化按键
 	Delay_Ms(200);          //延时500MS，等待指纹模块复位
	Device_Check();		   	//校对指纹模块是否接入正确，液晶做出相应的提示
	Delay_Ms(1000);			//对接成功界面停留一定时间
	while(1)
	{
	    /**************进入主功能界面****************/
		LCD1602_Display(0x80,"  search finger ",0,16);	 //第一排显示搜索指纹
		LCD1602_Display(0xc0,"  Add     Del   ",0,16);	 //添加和删除指纹
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
		//确认键
		if(KEY_OK == 0)
		{	 
		 	while(KEY_OK == 0);//等待松开按键								
			switch(local_date)
			{
					case 0:  //搜索指纹						
					FPM10A_Find_Fingerprint();																								
					break;	
					
					case 1:	 //添加指纹
					FPM10A_Add_Fingerprint();
					break; 					
					
					case 2:	//清空指纹
					FPM10A_Delete_All_Fingerprint();
		  			break;
			}
		}
		    //切换键
			if(KEY_DOWN == 0)
			{
			 	while(KEY_DOWN == 0); //等待松开按键				
	  	 		if(local_date<=2)
				{
					local_date++;
					if(local_date==3)
						local_date=0;						
				}		
			}						
			Delay_Ms(100); //延时判断100MS检测一次		
	}
}




