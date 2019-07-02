#include <hal.h>
#include <ch.h>
#include <chprintf.h>
#include <errno.h>
#include <communication.h>
// #include <usbcfg.c>



#define EOK 0

#ifdef __cplusplus
extern "C" {
#endif

extern const USBConfig usbcfg;
extern SerialUSBConfig serusbcfg;
extern SerialUSBDriver SDU1;

#ifdef __cplusplus
}
#endif

static  int8_t speed_value = 13;
static  int8_t angle_value = 0;

static bool flag_debug = 0;
static int survey_period_milliseconds;

static BaseSequentialStream *debug_stream = NULL;

static THD_WORKING_AREA(waConnection_action_n, 256);
static THD_FUNCTION(Connection_action_n, arg)
    {
        arg = arg;
        chThdSleepMilliseconds(500);
        while (true)
        {
            if (survey_period_milliseconds <= 0)
            {
                chThdSleepMilliseconds(10);
            }
            else
            {   
                chThdSleepMilliseconds(survey_period_milliseconds);
                SerialCommGetPkg();
                
            }
        }
    }


void init_USB(int period_milliseconds)
{
    survey_period_milliseconds = period_milliseconds;
    sduObjectInit( &SDU1 );
    sduStart( &SDU1, &serusbcfg );


    usbDisconnectBus( serusbcfg.usbp );
    chThdSleepMilliseconds( 1500 );
    usbStart( serusbcfg.usbp, &usbcfg );
    usbConnectBus( serusbcfg.usbp );

    debug_stream = (BaseSequentialStream *)&SDU1;
    


    chThdCreateStatic(waConnection_action_n, sizeof(waConnection_action_n), NORMALPRIO, Connection_action_n, NULL /* arg is NULL */);
    chThdSleepMilliseconds( 1500 );
  
}



SerialUSBDriver *comm_dr = &SDU1;


void dbgprintf( const char* format, ... )
{   

    if(flag_debug == 0)
        return;
    
    if ( !debug_stream )
        return;

    va_list ap;
    va_start(ap, format);
    chvprintf(debug_stream, format, ap);
    va_end(ap);
    
}


int get_value_speed(void)
{
    return speed_value;
}

int get_value_angle(void)
{
    return angle_value;

}

int SerialCommGetPkg(void)
{


    msg_t msg = chnGetTimeout(comm_dr, MS2ST(10));

    chThdSleepMilliseconds(100);
    

    if (msg < 0)
    {

        return EIO;
    }
    
    char start_byte = msg;
    if (start_byte == '#')
    {
        
        chThdSleepMilliseconds(10);

        uint8_t rcv_buffer[2];
        int32_t rcv_bytes = sizeof(rcv_buffer);

        msg = chnReadTimeout(comm_dr, rcv_buffer, rcv_bytes, MS2ST(10));

        if (msg != rcv_bytes)
        {
            return EIO;
        }

        if(rcv_buffer[0]*2 == rcv_buffer[1])
        {
            speed_value = rcv_buffer[0];

        }

  
    }

    else if(start_byte == '$')
    {   
        chThdSleepMilliseconds(10);
        
        
        uint8_t rcv_buffer[2];
        int32_t rcv_bytes = sizeof(rcv_buffer);

        msg = chnReadTimeout(comm_dr, rcv_buffer, rcv_bytes, MS2ST(10));

        if (msg != rcv_bytes)
        {
            return EIO;
        }

        if(rcv_buffer[0]*3 == rcv_buffer[1])
        {

            angle_value = rcv_buffer[0];
           
        }
        

    }

    else if(start_byte == '&')
    {
        chThdSleepMilliseconds(10);

        uint8_t rcv_buffer[3];
        int32_t rcv_bytes = sizeof(rcv_buffer);

        msg = chnReadTimeout(comm_dr, rcv_buffer, rcv_bytes, MS2ST(10));

        if (msg != rcv_bytes)
        {
            return EIO;
        }

        // Enable_debugging
        if(rcv_buffer[0]==38 && rcv_buffer[1]==79 && rcv_buffer[2] == 123)
        {
            flag_debug = 1;
        }

        // Disable_debugging
        if(rcv_buffer[0]==31 && rcv_buffer[1]==39 && rcv_buffer[2] == 115)
        {
            flag_debug = 0;
        }

        // Deactivate_connection
        if(rcv_buffer[0]==67 && rcv_buffer[1]==89 && rcv_buffer[2] == 23)
        {
            speed_value = 0;
            angle_value = 0;
        }
        // Stop_connection
        if(rcv_buffer[0]==34 && rcv_buffer[1]==63 && rcv_buffer[2] == 129)
        {
            speed_value = 0;
            angle_value = 0;
        }

    }

    

    return EOK;
}








