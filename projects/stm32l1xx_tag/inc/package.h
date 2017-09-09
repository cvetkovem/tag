#ifndef __PACKAGE_H_INCLUDED__
#define __PACKAGE_H_INCLUDED__

#include <stdlib.h>

/**
 FULL PACKAGE
 |PRE|SFD|MAC|DATA|CRC|END|
**/

void createFullPackage();

/**
 SIMPLE PACKAGE
 |PRE|SFD|MAC|END|

 1 - / up chirp
 0 - \ down chirp

*****
 PRE: 1 1 1 1

 SFD: 0 0

 MAC EXAMPLE E4:B3:66
 E4: 1 1 1 0 0 1 0 0
 B3: 1 0 1 1 0 0 1 1
 66: 0 1 1 0 0 1 1 0

 END: 1 0
*****

 TOTAL LEN: 32 bit
**/

void createSimplePackage(uint8_t *pkg, uint8_t *broadcastMac);

#endif /* __PACKAGE_H_INCLUDED__ */
