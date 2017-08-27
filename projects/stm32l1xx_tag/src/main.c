#include "functions.h"

int main(void) {
    deviceEnable();

    while(1) {
        if(!buttonPressedCheck()) {
            //TODO Stop mode
            __WFI();
        }

        __NOP();
    }
}
