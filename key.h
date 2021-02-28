#ifndef _KEY_H_
#define _KEY_H_
#include<reg52.h>

//定义按键输入端口
sbit KEY_DOWN=P3^7;
sbit KEY_OK=P3^6;
sbit KEY_CANCEL=P3^5;
sbit GND=P3^3;

void Key_Init(void);

#endif
