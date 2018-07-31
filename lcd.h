#include <functional>

#include "fonts.h"

#define NUM_MAX 4

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

    uint8_t line8(int l) {
        return screen[l];
    }

    int height() {
        return 8;
    }

    int width() {
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

    int getStrWidth(const wchar_t* str) {
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

    void printStr(int _x, int _y, const wchar_t* str) {
        int xx = _x;
        for (int i=0; str[i] != 0; ++i) {
            const uint8_t* symbol = symbolPtrOrNull(str[i]);
            if (symbol > fontUA && symbol < (fontUA + sizeof(fontUA))) {
                int symbolW = *symbol & 0xff;
                
                for (int x = 0; x < symbolW; ++x) {
                    uint8_t data = symbol[1 + x];
                    for (int y = 0; y < 8; ++y) {
                        set(xx - x, y + _y, (data >> y) & 1);
                    }
                }
                xx -= symbolW + 1;
            }
        }
        
    }

    /**
     * micros is current time in microseconds
     */
    void showTime(uint64_t micros) {
        int seconds = micros / secInUs;
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

        int movingTimeUs = secInUs / 4;
        int smallFontHeight = 6;
        for(int n = 1; n >= 0; n--) {
            int y;
            int us  = (micros % secInUs);
            if (us >= (secInUs - movingTimeUs)) {
                y = (smallFontHeight - (secInUs - us) * smallFontHeight / movingTimeUs);
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
