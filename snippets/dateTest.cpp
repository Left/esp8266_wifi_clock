#include "pseudo_arduino.h"
#include "../dateutil.h"

int main(int argc, char const *argv[]) {   
    for (int i = 0; i <= 17745; i+=1) {
        date::RTC rtc;
        date::epoc2rtc(i, rtc);
        printf("%d ::: %d%s %d %d (%d %d )\n", i, 
            rtc.year, rtc.leapYear ? "(leap)" : "", 
            rtc.month, 
            rtc.day,
            rtc.doy,
            rtc.dow);
    }

}