#include <ch.h>
#include <hal.h>
#include <communication.h>
#include <chprintf.h>



int main(void)
{
    chSysInit();
    halInit();

    init_USB();


    while (true)
    {
        SerialCommGetPkg();
        dbgprintf('C');
        
        // palToggleLine(LINE_LED1);
        // chThdSleepMilliseconds(200);
    }
}
