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
        chThdSleepMilliseconds(200);
    }
}
