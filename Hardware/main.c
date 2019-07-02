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
    int8_t value_speed_dbg = 0;
    int8_t value_angle_dbg = 0;
    chThdSleepMilliseconds(200);

    while (true)
    {   
        // SerialCommGetPkg();
        value_speed_dbg = get_value_speed();
        // value_speed_dbg = 10;
        value_angle_dbg = get_value_angle();
        // palToggleLine(LINE_LED2);
        chThdSleepMilliseconds(200);
        dbgprintf("value speed dbg %d , value angle dbg %d \n", value_speed_dbg, value_angle_dbg);
        // palToggleLine(LINE_LED3);
        // chThdSleepMilliseconds(100);

    }
}
