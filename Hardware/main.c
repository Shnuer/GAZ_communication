#include <ch.h>
#include <hal.h>
#include <communication.h>
#include <chprintf.h>



int main(void)
{
    chSysInit();
    halInit();

    init_USB(20);

    chThdSleepMilliseconds(100);

    palToggleLine(LINE_LED3);
    while (true)
    {   
        // SerialCommGetPkg();
        int value_speed_dbg = get_value_speed();
        int value_angle_dbg = get_value_angle();
        // palToggleLine(LINE_LED2);
        dbgprintf("value speed dbg %d , value angle dbg %d \n", value_speed_dbg, value_angle_dbg);
        // palToggleLine(LINE_LED3);
        chThdSleepMilliseconds(100);

    }
}
