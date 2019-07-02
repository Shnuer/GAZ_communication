#ifdef _COMMUNICATION_


#include <chprintf.h>
void init_USB(int period_milliseconds);
// int SerialCommGetPkg(void);
void dbgprintf( const char* format, ... );
int get_value_speed(void);
int get_value_angle(void);

#endif //_COMMUNICATION_
