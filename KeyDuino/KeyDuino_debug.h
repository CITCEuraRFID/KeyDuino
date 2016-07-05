#ifndef __DEBUG_H__
#define __DEBUG_H__

//#define DEBUG

#include "Arduino.h"

#ifdef DEBUG
    #define DMSG(args...)       Serial.print(args)
    #define DMSG_STR(str)       Serial.println(str)
    #define DMSG_HEX(num)       Serial.print(' '); Serial.print(num, HEX)
    #define DMSG_INT(num)       Serial.print(' '); Serial.print(num)

#else //Add delay to let time for the process to complete ...
    #define DELAY_DURATION      10
    #define DMSG(args...)       delay(DELAY_DURATION)
    #define DMSG_STR(str)       delay(DELAY_DURATION)
    #define DMSG_HEX(num)       delay(DELAY_DURATION)
    #define DMSG_INT(num)       delay(DELAY_DURATION)
#endif

#endif
