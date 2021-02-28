#include <reg52.h>
#define uchar unsigned char
#define uint unsigned int
uchar num;
unsigned char i, j, key;

sbit KEY_IN_1  = P1^4;//第一列
sbit KEY_IN_2  = P1^5;
sbit KEY_IN_3  = P1^6;
sbit KEY_IN_4  = P1^7;
sbit KEY_IN_5  = P3^2;//第五列
sbit KEY_OUT_1 = P1^3;//第一行
sbit KEY_OUT_2 = P1^2;
sbit KEY_OUT_3 = P1^1;
sbit KEY_OUT_4 = P1^0;
sbit rs=P2^6;
sbit wr=P2^5;
sbit lcden=P2^7;

uchar code table1[]="Num";//
uchar code table2[]="Input StudentNum";//
uchar table3[]="StudentNum  Test";
uchar table4[]="Loadind...";
uchar tabley[]="000";//不可使用code，因为这个数组内容是变化的。
uchar KeySta[4][5] = {  //全部矩阵按键的当前状态
    {1, 1, 1, 1, 1},  {1, 1, 1, 1, 1},  {1, 1, 1, 1, 1},  {1, 1, 1, 1, 1}
        };
uchar backup[4][5] = {  //按键值备份，保存前一次的值
        {1, 1, 1, 1, 1},  {1, 1, 1, 1, 1},  {1, 1, 1, 1, 1},  {1, 1, 1, 1, 1}
};
void delay(uint x)
{
        uchar m,n;
        for(m=x;m>0;m--)
        {
        for(n=120;n>0;n--);
        }
}
void Delayms(unsigned int ms)
{
  	unsigned int i;
  	while( (ms--) != 0)
   	{
    	for(i = 0; i < 600; i++); 
   	}             
}
keyscan()//矩阵键盘扫描函数
        {
                for (i=0; i<4; i++)  //循环检测4*5的矩阵按键
        {
            for (j=0; j<5; j++)
            {
                if (backup[i][j] != KeySta[i][j])  //检测按键动作
                {
                    if (backup[i][j] != 0)         //按键按下时执行动作
                    {
                        //P0 = LedChar[i*5+j];       //将编号显示到数码管
                                                  key=i*5+j;
                                        }
                    backup[i][j] = KeySta[i][j];   //更新前一次的备份值
                }
            }
        }
                return key;
                }

void display(uchar x)
{
        switch(x)
        {        case 0: tabley[0]='E';tabley[1]='n';tabley[2]='t'; break;
                case 1: tabley[0]='E';tabley[1]='s';tabley[2]='c'; break;
                  case 2: tabley[0]='D';tabley[1]='o';tabley[2]='w'; break;
                case 3: tabley[0]='U';tabley[1]='p';tabley[2]=' '; break;
                case 4: tabley[0]='*';tabley[1]=' ';tabley[2]=' '; break;
                case 5: tabley[0]='R';tabley[1]='i';tabley[2]='t'; break;
                case 6: tabley[0]='9';tabley[1]=' ';tabley[2]=' '; break;
                  case 7: tabley[0]='6';tabley[1]=' ';tabley[2]=' '; break;
                case 8: tabley[0]='3';tabley[1]=' ';tabley[2]=' '; break;
                case 9: tabley[0]='#';tabley[1]=' ';tabley[2]=' '; break;
                case 10: tabley[0]='0';tabley[1]=' ';tabley[2]=' '; break;
                case 11: tabley[0]='8';tabley[1]=' ';tabley[2]=' '; break;
                case 12: tabley[0]='5';tabley[1]=' ';tabley[2]=' '; break;
                  case 13: tabley[0]='2';tabley[1]=' ';tabley[2]=' '; break;
                case 14: tabley[0]='F';tabley[1]='2';tabley[2]=' '; break;
                case 15: tabley[0]='L';tabley[1]='e';tabley[2]='f'; break;
                case 16: tabley[0]='7';tabley[1]=' ';tabley[2]=' '; break;
                case 17: tabley[0]='4';tabley[1]=' ';tabley[2]=' '; break;
                case 18: tabley[0]='1';tabley[1]=' ';tabley[2]=' '; break;
                case 19: tabley[0]='F';tabley[1]='1';tabley[2]=' '; break;
        default: break;
        }

}
void write_com(uchar com)
{
        rs=0;
        wr=0;
        lcden=0;
        P0=com;
        lcden=1;
        delay(5);
        lcden=0;

}
void write_data(uchar dat)
{
        rs=1;
        wr=0;
        lcden=0;
        P0=dat;
        lcden=1;
        delay(5);
        lcden=0;

}

void init()
{
        write_com(0x38);
//        write_com(0x08);
        write_com(0x01);
        write_com(0x06);
        write_com(0x0f);
	    write_com(0x0c);
               

}



void main()
{
    EA = 1;       //使能总中断
    TMOD = 0x01;  //设置T0为模式1
    TH0  = 0xFC;  //为T0赋初值0xFC67，定时1ms
    TL0  = 0x67;
    ET0  = 1;     //使能T0中断
    TR0  = 1;     //启动T0
    P0 =0;   //默认显示0
        init();
	 for(num=0;num<16 ;num++)
        {
                write_data(table3[num]);
        //        delay(10);
               
        }
	write_com(0xc2);
	for(num=0;num<10 ;num++)
       {
                write_data(table4[num]);
        //        delay(10);
               
       }
	Delayms(200);
	write_com(0x01);		
	
    while (1)
    {
        keyscan();
        display(key);
//        tabley[0]=(key/10)+0x30;
//        tabley[1]=(key%10)+0x30;
        write_com(0x80+0x01);
        for(num=0;num<3 ;num++)
        {
                write_data(table1[num]);
        //        delay(10);
               
        }
        write_com(0x80+0x05);
/*                write_data(key/10+48);//第一种方法
                write_data(key%10+48);
                delay(10);*/
        for(num=0;num<3 ;num++)//第二种方法
        {
                write_data(tabley[num]);
        //        delay(10);       
        }
        write_com(0x80+0x40);
        for(num=0;num<16 ;num++)
        {
                write_data(table2[num]);
        //        delay(10);
        }



    }
}




/* T0中断服务函数，扫描矩阵按键状态并消抖 */
void InterruptTimer0() interrupt 1
{
    unsigned char i;
    static unsigned char keyout = 0;  //矩阵按键扫描输出索引
    static unsigned char keybuf[4][5] = {  //矩阵按键扫描缓冲区
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF},  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF},  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
};

    TH0 = 0xFC;  //重新加载初值
    TL0 = 0x67;
    //将一行的5个按键值移入缓冲区
    keybuf[keyout][0] = (keybuf[keyout][0] << 1) | KEY_IN_1;	//第一行第一列的缓冲值=第一行第一列的缓冲值左移一位和第一列按位或
    keybuf[keyout][1] = (keybuf[keyout][1] << 1) | KEY_IN_2;	
    keybuf[keyout][2] = (keybuf[keyout][2] << 1) | KEY_IN_3;	
    keybuf[keyout][3] = (keybuf[keyout][3] << 1) | KEY_IN_4;	
    keybuf[keyout][4] = (keybuf[keyout][4] << 1) | KEY_IN_5;	
    //消抖后更新按键状态
    for (i=0; i<5; i++)  //每行5个按键，所以循环5次
    {
        if ((keybuf[keyout][i] & 0x0F) == 0x00)
        {   //连续5次扫描值为0，即4*4ms内都是按下状态时，可认为按键已稳定的按下
            KeySta[keyout][i] = 0;
        }
        else if ((keybuf[keyout][i] & 0x0F) == 0x0F)
        {   //连续5次扫描值为1，即4*4ms内都是弹起状态时，可认为按键已稳定的弹起
            KeySta[keyout][i] = 1;
        }
    }
    //执行下一次的扫描输出
    keyout++;                //输出索引递增
    keyout = keyout & 0x03;  //索引值加到4即归零
    switch (keyout)          //根据索引，释放当前输出引脚，拉低下次的输出引脚
    {
        case 0: KEY_OUT_4 = 1; KEY_OUT_1 = 0; break;
        case 1: KEY_OUT_1 = 1; KEY_OUT_2 = 0; break;
        case 2: KEY_OUT_2 = 1; KEY_OUT_3 = 0; break;
        case 3: KEY_OUT_3 = 1; KEY_OUT_4 = 0; break;
        default: break;
    }
}