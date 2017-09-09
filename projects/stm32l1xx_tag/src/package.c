#include "package.h"

void createFullPackage() {

}

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

void createSimplePackage(uint8_t *pkg, uint8_t *broadcastMac) {
    pkg[0] = 0xF0; // PRE and SFD
    pkg[0] |= (broadcastMac[0] >> 6) & 0x03;

    pkg[1] = (broadcastMac[0] << 2) & 0xFC;
    pkg[1] |= (broadcastMac[1] >> 6) & 0x03;

    pkg[2] = (broadcastMac[1] << 2) & 0xFC;
    pkg[2] |= (broadcastMac[2] >> 6) & 0x03;

    pkg[3] = (broadcastMac[2] << 2) & 0xFC;
    pkg[3] |= 0x02;
}
