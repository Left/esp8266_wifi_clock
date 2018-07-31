#include <stdio.h>
#include <unistd.h>
#include <term.h>
#include <string>

#include <sys/time.h>
#include <iomanip>
#include <sstream>
#include <unistd.h>

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2
#define BYTE 0

#define PROGMEM
#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1

#define LSBFIRST 0 
#define MSBFIRST 1

#define CHANGE 1
#define FALLING 2
#define RISING 3

#define INTERNAL 3
#define DEFAULT 1
#define EXTERNAL 0

void delay(unsigned long) {}
void pinMode(int,int) {}
void digitalWrite(int pin, int val) {}
void shiftOut(int dataPin, int clockPin, int bitOrder, int value);

uint32_t micros() {
    timeval tv1 = {0};
    struct timezone tz = {0};
    gettimeofday(&tv1, &tz);
    return (((unsigned long)tv1.tv_sec - tz.tz_minuteswest*60) * 1000000l + tv1.tv_usec);
}

uint32_t millis() {
    return micros() / 1000;
}

#include "../lcd.h"

int main(int argc, char const *argv[]) {   
    if (!cur_term) {
        int result;
        setupterm( NULL, STDOUT_FILENO, &result );
        if (result <= 0) return -1;
    }
  
    putp(tigetstr((char *)"clear"));

    LcdScreen screen;

    for (int ii = 0; ii < 16000; ++ii) {
        //
        putp( tparm( tigetstr((char *)"cup" ), 0, 0, 0, 0, 0, 0, 0, 0, 0 ) );
        
        // screen.showTime(micros());
        screen.clear();
        // screen.printStr(31, 8 - micros() / 1000 / 100 % 16, "четверг");
        screen.printStr((micros() / 1000 / 50) % 300, 0, L"Четверг aaa bbb ююю {} || sdf 0123456789");

        /////////////////////////////////////////////////////////
        // OUT!
       
        for (int y = 0; y < 8; ++y) {
            for (int cnt = 0; cnt < 2; ++cnt) {
                printf("%01d |", y);
                for (int x = 0; x < screen.width(); ++x) {
                    printf("%s|", screen.get(screen.width() - 1 - x, y) ? "***" : "   ");
                }
                printf("\n");
            }
        }

        printf("  |");
        for (int x = 0; x < NUM_MAX*8; ++x) {
            printf("%02d |", screen.width() - 1 - x);
        }

        printf("\n");

        printf("Micros: %u\n", micros());

        usleep(1000); // will sleep for 1 ms
    }

    return 0;
}
