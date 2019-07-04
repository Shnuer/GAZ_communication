#include <ch.h>
#include <hal.h>
#include <communication.h>

/* For module test.
Obtaining speed and angle values and sending them is processed.
*/
int main(void)
{
    chSysInit();
    halInit();

    comm_init();

    comm_speed_t value_speed_dbg = 0;
    comm_steer_t value_angle_dbg = 0;

    while ( true )
    {   
        value_speed_dbg = comm_get_speed();
        value_angle_dbg = comm_get_steer();

        comm_dbgprintf("value speed dbg %d, value angle dbg %d\n", value_speed_dbg, value_angle_dbg);
        
        chThdSleepMilliseconds(200);
    }
}
