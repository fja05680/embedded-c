/***************************************
Here is my fixed point code, you can use it like this:

fixed f;
fixed g;


f = int2fix(10);
g = float2fix(1.234);

f = fixmul(f,g) + f - fixdiv(g,f);
//Addition and subtraction just use the normal operators,
// multiplication and division use macros.

printf("%d.%03d\r\n",intpart(f),fracpart(f));

******************************************/

#ifndef __FIXED_POINT_H
#define __FIXED_POINT_H

#include <stdlib.h>
#include <stdint.h>

typedef int32_t fixed;

#define FP_BINPOINT 8
#define FP_PARTS 256 /* 8 bits behind bin point, 256 parts/whole */
#define FP_FRACMASK 0xFF

extern uint16_t fixedfrac[];

#define fixmul(x1, x2) (((x1) * (x2)) >> FP_BINPOINT)
#define fixdiv(x1, x2) (((x1) << FP_BINPOINT) / (x2))

#define int2fix(i) ((fixed)((int32_t)(i) << FP_BINPOINT))
#define fix2int(f) ((int32_t)((f) >> FP_BINPOINT))

#define float2fix(f) ((fixed)((f) * FP_PARTS) )
#define fix2float(f) (((float)(f)) / FP_PARTS )

#define intpart(f) (((f) > (fixed)0) ? ((f) >> FP_BINPOINT) : (-1 * (abs(f) >> FP_BINPOINT)))
#define fracpart(f) (fixedfrac[abs(f) & FP_FRACMASK])

#define roundfix(f) (((abs(f) & FP_FRACMASK) > FP_PARTS / 2) ? intpart(f) + 1 : intpart(f))

#endif /*__FIXED_POINT_H*/
