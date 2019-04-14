#ifndef __DEFINES_H
#define __DEFINES_H

#include <misc.h>

#define false 0
#define true !false

typedef union {
	uint8_t b[2];
	int16_t i;
} twoBytes;

typedef union {
	uint8_t b[2];
	uint16_t i;
} u_twoBytes;

typedef union{
	uint8_t b[4];
	uint32_t i;
} u_fourBytes;

typedef union {
	uint8_t b[4];
    float f;
} u_float;

/*
 * Set version String
 *
 */
static char FW_0 __attribute__ ((section ("._version_start"))) __attribute__ ((__used__)) = 1;
static char FW_1 __attribute__ ((section ("._version_start"))) __attribute__ ((__used__)) = 0;
static char FW_2 __attribute__ ((section ("._version_start"))) __attribute__ ((__used__)) = 0;
static char FW_3 __attribute__ ((section ("._version_start"))) __attribute__ ((__used__)) = 0;

////////////////////////////////////////////////////////////////////////////
/* wrap-safe macros for tick comparison */
#define TIMEOUT(start, duration)         ((micros - start) >= duration)

#ifndef NULL
#define NULL ((void*)0)
#endif

#define ABS(x)         (x < 0) ? (-x) : x
#define MAX(a, b)       (a < b) ? (b) : a
#define MIN(a, b)      (((a) < (b)) ? (a) : (b))


#define _BV(x)			1<<x
#define _WV(x)			1U<<x

#endif	/* __DEFINES_H */
