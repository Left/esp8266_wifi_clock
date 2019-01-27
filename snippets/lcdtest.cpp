#include "pseudo_arduino.h"

#include "../lcd.h"

int main(int argc, char const *argv[]) {   
    if (!cur_term) {
        int result;
        setupterm( NULL, STDOUT_FILENO, &result );
        if (result <= 0) return -1;
    }
  
    putp(tigetstr((char *)"clear"));

    LcdScreen screen;
    // screen.showMessage("ЁЁЁёёёЁЁЁёёё!");

    for (int ii = 0; ii < 16000; ++ii) {
        // Move cursor to top left
        putp( tparm( tigetstr((char *)"cup" ), 0, 0, 0, 0, 0, 0, 0, 0, 0 ) );
        
        screen.clear();

        const uint64_t dayInSecs = (24*60*60);
        screen.showTime(micros64() / dayInSecs / 1000000ull, (micros64() % (dayInSecs*1000000ull)/1000ull));

        // screen.clear();
        // screen.printStr(31, 8 - micros() / 1000 / 100 % 16, "четверг");
        // screen.printStr((micros() / 1000 / 50) % 300, 0, L"Четверг aaa bbb ююю {} || sdf 0123456789");

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
