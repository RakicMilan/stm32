#ifndef __DEFINES_H
#define __DEFINES_H

#include <misc.h>

#define false 0
#define true !false

typedef union {
    unsigned char b[2];
    unsigned int i;
} u_twoBytes;

typedef union{
	uint32_t i;
	unsigned char b[4];
} u_fourBytes;

typedef union {
    unsigned char b[4];
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
