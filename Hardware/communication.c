#include <hal.h>
#include <ch.h>
#include <chprintf.h>
#include <errno.h>
#include <communication.h>

#define EOK 0

extern const USBConfig usbcfg;
extern SerialUSBConfig serusbcfg;
extern SerialUSBDriver SDU1;

static comm_speed_t speed_value = 0;
static comm_steer_t angle_value = 0;

static bool flag_debug = 0;

static BaseSequentialStream *debug_stream = NULL;
static BaseChannel *comm_chn = NULL;

static int retrieve_input_data(void);

#define INPUT_SYMB_CMD  '&'
typedef struct {
    uint8_t speed;
    uint8_t steer;
    uint8_t ck;
} input_cmd_t;

#define INPUT_SYMB_CTL  '#'

static THD_WORKING_AREA(waConnection_action_n, 256);
static THD_FUNCTION(Connection_action_n, arg)
{
    arg = arg;
    
    while (true)
    {
#if (COMM_MODE == COMM_MODE_SERIAL_USB)
        /* Each time we need to check as reading from non active leads to freeze */
        if ( SDU1.config->usbp->state != USB_ACTIVE )
        {
            chThdSleepMilliseconds( 20 );
            continue;
        }
#endif

        if ( retrieve_input_data() == EOK )
        {
        }
    }
}

static THD_WORKING_AREA(waBlinker, 256);
static THD_FUNCTION(Blinker, arg)
{
    arg = arg;

    while (true)
    {
        chThdSleepMilliseconds(500);
        palToggleLine(LINE_LED3);     
    }
}

static int retrieve_input_data(void)
{
    msg_t msg = chnGetTimeout(comm_chn, TIME_INFINITE);
    if (msg < 0)
    {   
#if (COMM_MODE == COMM_MODE_SERIAL_USB)
        /* Q_RESET is returned in case of USB, when it is suddenly plugged out */
        if ( msg == Q_RESET )
            palToggleLine(LINE_LED2);
#endif

        return ENODATA;
    }

    char start_byte = msg;
    if (start_byte == INPUT_SYMB_CTL)
    {
        input_cmd_t inp;

        msg = chnRead(comm_chn, (uint8_t *)&inp, sizeof(inp));

        if (msg != sizeof(inp))
        {
            return EIO;
        }

        uint8_t calc_ck = inp.speed + inp.steer * 2;
        if (calc_ck == inp.ck)
        {
            speed_value = inp.speed;
            angle_value = inp.steer;

            return EOK;
        }
    }
    else if (start_byte == INPUT_SYMB_CMD)
    {
        uint8_t rcv_buffer[3];

        msg = chnRead(comm_chn, rcv_buffer, sizeof(rcv_buffer));

        if (msg != sizeof(rcv_buffer))
        {
            return EIO;
        }

        // Enable_debugging
        if (rcv_buffer[0] == 38 && rcv_buffer[1] == 79 && rcv_buffer[2] == 123)
        {
            flag_debug = 1;

            return EOK;
        }

        // Disable_debugging
        if (rcv_buffer[0] == 31 && rcv_buffer[1] == 39 && rcv_buffer[2] == 115)
        {
            flag_debug = 0;

            return EOK;
        }

        // Deactivate_connection
        if (rcv_buffer[0] == 67 && rcv_buffer[1] == 89 && rcv_buffer[2] == 23)
        {
            speed_value = 0;
            angle_value = 0;

            return EOK;
        }
        // Stop_connection
        if (rcv_buffer[0] == 34 && rcv_buffer[1] == 63 && rcv_buffer[2] == 129)
        {
            speed_value = 0;
            angle_value = 0;

            return EOK;
        }
    }

    return ENODATA;
}

void comm_init()
{
    chThdCreateStatic(waBlinker, sizeof(waBlinker), NORMALPRIO, Blinker, NULL /* arg is NULL */);

#if (COMM_MODE == COMM_MODE_SERIAL_USB)
    sduObjectInit(&SDU1);
    sduStart(&SDU1, &serusbcfg);

    usbDisconnectBus(serusbcfg.usbp);
    // chThdSleepMilliseconds(1500);
    usbStart(serusbcfg.usbp, &usbcfg);
    usbConnectBus(serusbcfg.usbp);

    debug_stream = (BaseSequentialStream *)&SDU1;
    comm_chn = (BaseChannel *)&SDU1;
#elif (COMM_MODE == COMM_MODE_SERIAL_PROG)
    static const SerialConfig sd_st_cfg = {
        .speed = 115200,
        .cr1 = 0,
        .cr2 = 0,
        .cr3 = 0};

    sdStart(&SD3, &sd_st_cfg);
    palSetPadMode(GPIOD, 8, PAL_MODE_ALTERNATE(7));
    palSetPadMode(GPIOD, 9, PAL_MODE_ALTERNATE(7));

    debug_stream = (BaseSequentialStream *)&SD3;
    comm_chn = (BaseChannel *)&SD3;
#else
    chSysHalt("Communication module: mode not set");
#endif

    chThdCreateStatic(waConnection_action_n, sizeof(waConnection_action_n), NORMALPRIO, Connection_action_n, NULL /* arg is NULL */);
}

void comm_dbgprintf(const char *format, ...)
{
    if (!debug_stream)
        return;

    if (!flag_debug)
        return;

    va_list ap;
    va_start(ap, format);
    chvprintf(debug_stream, format, ap);
    va_end(ap);
}

comm_speed_t comm_get_speed(void)
{
    return speed_value;
}

comm_steer_t comm_get_steer(void)
{
    return angle_value;
}
