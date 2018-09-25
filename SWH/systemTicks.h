#ifndef _SYSTEMTICK_H
#define _SYSTEMTICK_H

#include <misc.h>

#define SYSTICK     1000000

#define _ONE_SEC SYSTICK

#define TIME(x) (const) ((float) x * SYSTICK)

#define _DelayMS(x)        _DelayUS(x * 1000)

extern volatile uint32_t micros;
void InitSystemTicks(void);

extern void _DelayUS(uint32_t aDelay);

#endif
