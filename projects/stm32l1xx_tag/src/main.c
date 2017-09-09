#include "functions.h"

int main(void) {
    deviceEnable();

    while(1) {
        if(!buttonPressedCheck()) {
            //Stop mode
            SCB->SCR |= SCB_SCR_SLEEPDEEP;
            __WFI();
        }

        __NOP();
    }
}
