#ifndef _BUZZ_H_
#define _BUZZ_H_
#include<reg52.h>

sbit buzz=P1^4;

void Buzz_Times(unsigned char times);

#endif