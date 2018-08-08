#include <functional>

#include "common.h"

#include "fonts.h"
#include "dateutil.h"

#define NUM_MAX 4

typedef const wchar_t* WSTR;
typedef wchar_t* WSTR_MUTABLE;

const WSTR weekdays[] = {
    L"Понедельник", 
    L"Вторник",
    L"Среда",
    L"Четверг",
    L"Пятница",
    L"Суббота",
    L"Воскресенье"
};

const WSTR monthes[] = {
    L"января",
    L"февраля",
    L"марта",
    L"апреля",
    L"мая",
    L"июня",
    L"июля",
    L"августа",
    L"сентября",
    L"октября",
    L"ноября",
    L"декабря"
};

const unsigned char midNumbers[][8] = {
  { 0x38, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x38 }, // 0
  { 0x10, 0x30, 0x50, 0x10, 0x10, 0x10, 0x10, 0x7c }, // 1
  { 0x38, 0x44, 0x04, 0x08, 0x10, 0x20, 0x40, 0x7c }, // 2
  { 0x38, 0x44, 0x04, 0x18, 0x04, 0x04, 0x44, 0x38 }, // 3
  { 0x0c, 0x14, 0x24, 0x44, 0x7c, 0x04, 0x04, 0x04 }, // 4
  { 0x7c, 0x40, 0x40, 0x78, 0x04, 0x04, 0x44, 0x38 }, // 5
  { 0x3c, 0x40, 0x40, 0x78, 0x44, 0x44, 0x44, 0x38 }, // 6
  { 0x7c, 0x44, 0x04, 0x08, 0x10, 0x20, 0x20, 0x20 }, // 7
  { 0x38, 0x44, 0x44, 0x38, 0x44, 0x44, 0x44, 0x38 }, // 8
  { 0x38, 0x44, 0x44, 0x44, 0x3c, 0x04, 0x04, 0x38 }, // 9
};

const unsigned char smallNumbers[][8] = {
  { 0x00, 0x00, 0x00, 0xe0, 0xa0, 0xa0, 0xa0, 0xe0 }, // 0
  { 0x00, 0x00, 0x00, 0x40, 0xc0, 0x40, 0x40, 0xe0 }, // 1
  { 0x00, 0x00, 0x00, 0xe0, 0x20, 0xe0, 0x80, 0xe0 }, // 2
  { 0x00, 0x00, 0x00, 0xe0, 0x20, 0xe0, 0x20, 0xe0 }, // 3
  { 0x00, 0x00, 0x00, 0xa0, 0xa0, 0xe0, 0x20, 0x20 }, // 4
  { 0x00, 0x00, 0x00, 0xe0, 0x80, 0xe0, 0x20, 0xe0 }, // 5
  { 0x00, 0x00, 0x00, 0xe0, 0x80, 0xe0, 0xa0, 0xe0 }, // 6
  { 0x00, 0x00, 0x00, 0xe0, 0x20, 0x20, 0x40, 0x40 }, // 7
  { 0x00, 0x00, 0x00, 0xe0, 0xa0, 0xe0, 0xa0, 0xe0 }, // 8
  { 0x00, 0x00, 0x00, 0xe0, 0xa0, 0xe0, 0x20, 0xe0 }, // 9
};

class Figure {
public:
    virtual void pixels(std::function<void(int, int)> acceptor) const = 0;
};

class Rectangle : public Figure {
public:
    const int x;
    const int y;
    const int w;
    const int h;

    Rectangle() : x(0), y(0), w(0), h(0) {}
    Rectangle(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h) {}

    virtual void pixels(std::function<void(int, int)> acceptor) const {
        for (int xx = x; xx < x + w; ++xx) {
            for (int yy = y; yy < y + h; ++yy) {
                acceptor(xx, yy);
            }
        }
    }
};

class Bitmask : public Figure {
    typedef const uint8_t BitsContent[8];
public:
    const BitsContent& bits;
    Bitmask(const BitsContent& _bits) : bits(_bits) {
    }

    virtual void pixels(std::function<void(int, int)> acceptor) const {
        for (int yy = 0; yy < sizeof(bits); ++yy) {
            for (int i = 0; i < 8; ++i) {
                if ((bits[yy] >> i) & 1) {
                    acceptor(i, yy);
                }
            }
        }
    }
};


class LcdScreen {

private:
    uint8_t screen[NUM_MAX * 8];

    int strStartAt = 0;
    WSTR_MUTABLE _str = NULL;

public:
    static const int secInUs = 1000000;

    LcdScreen() {
        memset(screen, 0, sizeof(screen));
    }

    bool fits(int x, int y) {
        return (x >= 0) && (y >= 0) && (x < width()) && (y < height());
    }

    int idx(int x, int y) {
        int res = x / 8 * 8 + y;
        return res;
    }

    bool get(int x, int y) {
        if (!fits(x, y)) {
            return false; // Outside world is black
        }
        return (screen[idx(x, y)] >> (x % 8)) & 1;
    }

    void set(int x, int y, bool clr) {
        if (fits(x, y)) {
            screen[idx(x, y)] &= ~(1 << (x % 8));
            screen[idx(x, y)] |= (clr ? 1 : 0) << (x % 8);
        }
    }

    void invert(int x, int y) {
        set(x, y, !get(x, y));
    }

    void invert(int _x, int _y, const Figure& fg) {
        fg.pixels([=](int x, int y) {
            this->invert(_x + x, _y + y);
        });
    }

    void set(int _x, int _y, const Figure& fg, bool clr) {
        fg.pixels([=](int x, int y) {
            this->set(_x + x, _y + y, clr);
        });
    }

    void clear() {
        memset(screen, 0, sizeof(screen));
    }

    uint8_t line8(int l) const {
        return screen[l];
    }

    int height() const {
        return 8;
    }

    int width() const {
        return NUM_MAX*8;
    }

    /**
     * This could be hour, min or sec
     */
    class TimeComponent {
        int val = 0;
        char asStr[4];
    public:
        TimeComponent(int _val) : val(_val) {
            memset(asStr, 0, sizeof(asStr));
            snprintf(asStr, sizeof(asStr), "%02d", val);
        }

        uint8_t charAt(int ind) {
            return asStr[ind];
        }

        const char* c_str() {
            return asStr;
        }
    };

    const uint8_t* symbolPtrOrNull(wchar_t symbol) {
        int fontItem = fontUA[0];
        int sym1251 = symbol & 0xffff;
        if (sym1251 >= 0x410 && sym1251 < 0x450) {
            sym1251 = sym1251 - 0x410 + 0xa0;
        } else {
            sym1251 = sym1251 - 0x20;
        }
        if (sym1251 >= 0 && sym1251 < (sizeof(fontUA) - 1)/fontItem) {
            return fontUA + 1 + sym1251*fontItem;
        }
        return NULL;
    }

    int getStrWidth(const WSTR* strs, int strsCnt) {
        int w = 0;
        for (;strsCnt > 0; --strsCnt, ++strs) {
            w += getStrWidth(*(strs)) + 1;
        }
        return w;
    }

    int getStrWidth(WSTR str) {
        int res = 0;
        for (int i=0; str[i] != 0; ++i) {
            const uint8_t* symbol = symbolPtrOrNull(str[i]);
            if (symbol != NULL) {
                if (res > 0) {
                    res += 1;
                }
                res += symbol[0];
            }
        }
        return res;
    }

    /**
     * Prints several strings at once
     */
    int printStr(int _x, int _y, const WSTR* strs, int strsCnt) {
        int ww = 0;
        for (;strsCnt > 0; --strsCnt, ++strs) {
            int w = printStr(_x, _y, *(strs));
            ww += w + 1;
            _x -= w + 1;
        }
        return ww;
    }

    /**
     * Prints string and returns printed string width
     */
    int printStr(int _x, int _y, WSTR str) {
        int w = 0;
        for (int i=0; str[i] != 0; ++i) {
            const uint8_t* symbol = symbolPtrOrNull(str[i]);
            if (symbol > fontUA && symbol < (fontUA + sizeof(fontUA))) {
                int symbolW = *symbol & 0xff;
                
                for (int x = 0; x < symbolW; ++x) {
                    uint8_t data = symbol[1 + x];
                    for (int y = 0; y < 8; ++y) {
                        set(_x - w - x, y + _y, (data >> y) & 1);
                    }
                }
                w += symbolW + 1;
            }
        }
        return w;        
    }

    void showMessage(const char* utf8str) {
        if (_str != NULL) {
            free(_str);
        }
        int maxSize = strlen(utf8str)*sizeof(wchar_t);
        _str = (WSTR_MUTABLE)malloc(maxSize);
        memset(_str, 0, maxSize-1);
        int srcLen = strlen(utf8str);
        for (int i = 0, outIndex = 0; i < srcLen;) {
            uint16_t sym = utf8str[i];
            if ((sym & 0b10000000) == 0) {
                // If an UCS fits 7 bits, its coded as 0xxxxxxx. This makes ASCII character represented by themselves
                _str[outIndex++] = sym;
                i++;
            } else if (
                (sym & 0b11100000) == 0b11000000 && 
                ((i + 1) < srcLen) && (utf8str[i+1] & 0b11000000) == 0b10000000
            ) {
                // If an UCS fits 11 bits, it is coded as 110xxxxx 10xxxxxx
                _str[outIndex++] = (((sym & 0b11111) << 6) & 0b11111000000) |
                    ((uint16_t)utf8str[i+1] & 0b111111);
                i+=2;
            } else if (
                (sym & 0b11110000) == 0x11100000 && 
                ((i + 1) < srcLen) && (utf8str[i+1] & 0b11000000) == 0b10000000 &&
                ((i + 2) < srcLen) && (utf8str[i+2] & 0b11000000) == 0b10000000
            ) {
                // If an UCS fits 16 bits, it is coded as 1110xxxx 10xxxxxx 10xxxxxx
                _str[outIndex++] = 
                    (((sym & 0b1111) << 12) & 0b1111000000000000) |
                    ((((uint16_t)utf8str[i+1] & 0b111111) << 6) & 0b111111000000) |
                    ((uint16_t)utf8str[i+2] & 0b111111);

                i+=3;
            } else {
                i++;
            }
        }
        strStartAt = millis();
    }

    /**
     * micros is current time in microseconds
     */
    void showTime(uint32_t daysSince1970, uint32_t millisSince1200) {
        if (_str != NULL) {
            uint32_t showedTime = millis() - strStartAt;
            printStr((showedTime / 50) % getStrWidth(_str), 0, _str);
            if ((showedTime / 50) > getStrWidth(_str)) {
                free(_str);
                _str = NULL;
            }
            return;
        } else if (millisSince1200 / 1000 % 30 < 10) {
            date::RTC rtc = {0};
            date::epoc2rtc(daysSince1970, rtc);
            wchar_t yearStr[10] = { 0 };
            swprintf(yearStr, __countof(yearStr), L"%d", rtc.year);
            wchar_t dayStr[10] = { 0 };
            swprintf(dayStr, __countof(dayStr), L"%d", rtc.day+1);
            const WSTR ss[] = {
                L"  ", 
                weekdays[rtc.dow],
                L", ",
                dayStr,
                L" ",
                monthes[rtc.month],
                L"   "
            };
            printStr((millis() / 50) % getStrWidth(ss, __countof(ss)), 0, ss, __countof(ss));
            return;
        }
        
        int seconds = millisSince1200 / 1000;
        TimeComponent hours((seconds % 86400L) / 3600);
        TimeComponent mins((seconds % 3600) / 60);

        TimeComponent secs(seconds % 60);
        TimeComponent nextSecs((seconds + 1) % 60);
        TimeComponent nextNextSecs((seconds + 2) % 60);

        clear();
        // Hours
        for(int n = 0; n < 2; n++) {
            set(width() - 1 - (n+1)*6, 0, 
                Bitmask(midNumbers[hours.charAt(n) - '0']), true);
        }
        // Mins
        for(int n = 0; n < 2; n++) {
            set(18 - (n+1)*6, 0, 
                Bitmask(midNumbers[mins.charAt(n) - '0']), true);
        }

        int movingTimeUs = 1000 / 4; // Period of time to do seconds moving transition
        int smallFontHeight = 6;
        for(int n = 1; n >= 0; n--) {
            int y;
            int ms  = (millisSince1200 % 1000);
            if (ms >= (1000 - movingTimeUs)) {
                y = (smallFontHeight - (1000 - ms) * smallFontHeight / movingTimeUs);
            } else {
                y = 0;
            }

            invert(n*4 - 5, y, Bitmask(smallNumbers[secs.charAt(1-n) - '0']));
            invert(n*4 - 5, y-smallFontHeight, Bitmask(smallNumbers[nextSecs.charAt(1-n) - '0']));
            invert(n*4 - 5, y-smallFontHeight*2, Bitmask(smallNumbers[nextNextSecs.charAt(1-n) - '0']));

            // printf("%s %s %s -> %d %ul\n", nextNextSecs.c_str(), secs.c_str(), nextSecs.c_str(), y, us); 
        }
    }
};

//======================================================================================================

class MAX72xx {
    // Opcodes for the MAX7221 and MAX7219
    // All OP_DIGITn are offsets from OP_DIGIT0
    #define	OP_NOOP         0 ///< MAX72xx opcode for NO OP
    #define OP_DIGIT0       1 ///< MAX72xx opcode for DIGIT0
    #define OP_DIGIT1       2 ///< MAX72xx opcode for DIGIT1
    #define OP_DIGIT2       3 ///< MAX72xx opcode for DIGIT2
    #define OP_DIGIT3       4 ///< MAX72xx opcode for DIGIT3
    #define OP_DIGIT4       5 ///< MAX72xx opcode for DIGIT4
    #define OP_DIGIT5       6 ///< MAX72xx opcode for DIGIT5
    #define OP_DIGIT6       7 ///< MAX72xx opcode for DIGIT6
    #define OP_DIGIT7       8 ///< MAX72xx opcode for DIGIT7
    #define OP_DECODEMODE   9 ///< MAX72xx opcode for DECODE MODE
    #define OP_INTENSITY   10 ///< MAX72xx opcode for SET INTENSITY
    #define OP_SCANLIMIT   11 ///< MAX72xx opcode for SCAN LIMIT
    #define OP_SHUTDOWN    12 ///< MAX72xx opcode for SHUT DOWN
    #define OP_DISPLAYTEST 15 ///< MAX72xx opcode for DISPLAY TEST

public:
    MAX72xx(LcdScreen& _screen, 
            const int _CLK_PIN,
            const int _DATA_PIN,
            const int _CS_PIN) : 
            screen(_screen), 
            CLK_PIN(_CLK_PIN),
            DATA_PIN(_DATA_PIN),
            CS_PIN(_CS_PIN) {
    }

    void sendCmd(int addr, uint8_t cmd, uint8_t data) {
        digitalWrite(CS_PIN, LOW);
        for (int i = NUM_MAX - 1; i >= 0; i--) {
            shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, i == addr ? cmd : 0);
            shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, i == addr ? data : 0);
        }
        digitalWrite(CS_PIN, HIGH);
    }

    void sendCmdAll(uint8_t cmd, uint8_t data) {
        digitalWrite(CS_PIN, LOW);
        for (int i = NUM_MAX - 1; i >= 0; i--) {
            shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, cmd);
            shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, data);
        }
        digitalWrite(CS_PIN, HIGH);
    }

    void setup() {
        pinMode(CS_PIN, OUTPUT);
        pinMode(DATA_PIN, OUTPUT);
        pinMode(CLK_PIN, OUTPUT);

        digitalWrite(CS_PIN, HIGH);
        sendCmdAll(OP_DISPLAYTEST, 0);
        sendCmdAll(OP_SCANLIMIT, 7);
        sendCmdAll(OP_DECODEMODE, 0);
        sendCmdAll(OP_SHUTDOWN, 1);
        sendCmdAll(OP_INTENSITY, 0); // minimum brightness
    }

    void refreshAll() {
        for (int line = 0; line < 8; line++) {
            digitalWrite(CS_PIN, LOW);
            for (int chip = NUM_MAX - 1; chip >= 0; chip--) {
                shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, OP_DIGIT0 + line);
                shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, screen.line8(chip * 8 + line));
            }
            digitalWrite(CS_PIN, HIGH);
        }
        digitalWrite(CS_PIN, LOW);
    }

    void showMessage(const char* str) {
        screen.showMessage(str);
    }

  private:
    const int CLK_PIN;
    const int DATA_PIN;
    const int CS_PIN;

    LcdScreen& screen;
};

//======================================================================================================
