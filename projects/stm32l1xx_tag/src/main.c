#include "functions.h"

int main(void) {
    deviceEnable();

    while(1) {
        __WFI();
        __NOP();
    }
}
