
const unsigned char midNumbers[][8] = {
  { 0x38, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x38 }, // 0
  { 0x10, 0x30, 0x50, 0x10, 0x10, 0x10, 0x10, 0x7c }, // 1
  { 0x38, 0x44, 0x04, 0x08, 0x10, 0x20, 0x40, 0x7c }, // 2
  { 0x38, 0x44, 0x04, 0x18, 0x04, 0x04, 0x44, 0x38 }, // 3
  { 0x0c, 0x14, 0x24, 0x44, 0x7c, 0x04, 0x04, 0x04 }, // 4
  { 0x7c, 0x40, 0x40, 0x78, 0x04, 0x04, 0x44, 0x38 }, // 5
  { 0x38, 0x44, 0x40, 0x78, 0x44, 0x44, 0x44, 0x38 }, // 6
  { 0x7c, 0x44, 0x04, 0x08, 0x10, 0x20, 0x20, 0x20 }, // 7
  { 0x38, 0x44, 0x44, 0x38, 0x44, 0x44, 0x44, 0x38 }, // 8
  { 0x38, 0x44, 0x44, 0x44, 0x3c, 0x04, 0x44, 0x38 }, // 9
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

const uint8_t fontUA[] = {8,
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // 32 - 'Space'
  0x01, 0x5f, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00,  // 33 - '!'
  0x03, 0x07, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00,  // 34 - '"'
  0x05, 0x14, 0x7f, 0x14, 0x7f, 0x14, 0x00, 0x00,  // 35 - '#'
  0x05, 0x24, 0x2a, 0x7f, 0x2a, 0x12, 0x00, 0x00,  // 36 - '$'
  0x05, 0x23, 0x13, 0x08, 0x64, 0x62, 0x00, 0x00,  // 37 - '%'
  0x05, 0x36, 0x49, 0x56, 0x20, 0x50, 0x00, 0x00,  // 38 - '&'
  0x03, 0x08, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00,  // 39 - '''
  0x03, 0x1c, 0x22, 0x41, 0x00, 0x00, 0x00, 0x00,  // 40 - '('
  0x03, 0x41, 0x22, 0x1c, 0x00, 0x00, 0x00, 0x00,  // 41 - ')'
  0x05, 0x2a, 0x1c, 0x7f, 0x1c, 0x2a, 0x00, 0x00,  // 42 - '*'
  0x05, 0x08, 0x08, 0x3e, 0x08, 0x08, 0x00, 0x00,  // 43 - '+'
  0x03, 0x80, 0x70, 0x30, 0x00, 0x00, 0x00, 0x00,  // 44 - ','
  0x05, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00,  // 45 - '-'
  0x01, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // 46 - '.'
  0x05, 0x20, 0x10, 0x08, 0x04, 0x02, 0x00, 0x00,  // 47 - '/'
  0x05, 0x3e, 0x51, 0x49, 0x45, 0x3e, 0x00, 0x00,  // 48 - '0'
  0x05, 0x00, 0x42, 0x7f, 0x40, 0x00, 0x00, 0x00,  // 49 - '1'
  0x05, 0x72, 0x49, 0x49, 0x49, 0x46, 0x00, 0x00,  // 50 - '2'
  0x05, 0x21, 0x41, 0x49, 0x4d, 0x33, 0x00, 0x00,  // 51 - '3'
  0x05, 0x18, 0x14, 0x12, 0x7f, 0x10, 0x00, 0x00,  // 52 - '4'
  0x05, 0x27, 0x45, 0x45, 0x45, 0x39, 0x00, 0x00,  // 53 - '5'
  0x05, 0x3c, 0x4a, 0x49, 0x49, 0x31, 0x00, 0x00,  // 54 - '6'
  0x05, 0x41, 0x21, 0x11, 0x09, 0x07, 0x00, 0x00,  // 55 - '7'
  0x05, 0x36, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00,  // 56 - '8'
  0x05, 0x46, 0x49, 0x49, 0x29, 0x1e, 0x00, 0x00,  // 57 - '9'
  0x02, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00,  // 58 - ':'
  0x02, 0x80, 0x68, 0x00, 0x00, 0x00, 0x00, 0x00,  // 59 - ';'
  0x04, 0x08, 0x14, 0x22, 0x41, 0x00, 0x00, 0x00,  // 60 - '<'
  0x05, 0x14, 0x14, 0x14, 0x14, 0x14, 0x00, 0x00,  // 61 - '='
  0x04, 0x41, 0x22, 0x14, 0x08, 0x00, 0x00, 0x00,  // 62 - '>'
  0x05, 0x02, 0x01, 0x59, 0x09, 0x06, 0x00, 0x00,  // 63 - '?'
  0x05, 0x3e, 0x41, 0x5d, 0x59, 0x4e, 0x00, 0x00,  // 64 - '@'
  0x05, 0x7c, 0x12, 0x11, 0x12, 0x7c, 0x00, 0x00,  // 65 - 'A'
  0x05, 0x7f, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00,  // 66 - 'B'
  0x05, 0x3e, 0x41, 0x41, 0x41, 0x22, 0x00, 0x00,  // 67 - 'C'
  0x05, 0x7f, 0x41, 0x41, 0x41, 0x3e, 0x00, 0x00,  // 68 - 'D'
  0x05, 0x7f, 0x49, 0x49, 0x49, 0x41, 0x00, 0x00,  // 69 - 'E'
  0x05, 0x7f, 0x09, 0x09, 0x09, 0x01, 0x00, 0x00,  // 70 - 'F'
  0x05, 0x3e, 0x41, 0x41, 0x51, 0x73, 0x00, 0x00,  // 71 - 'G'
  0x05, 0x7f, 0x08, 0x08, 0x08, 0x7f, 0x00, 0x00,  // 72 - 'H'
  0x03, 0x41, 0x7f, 0x41, 0x00, 0x00, 0x00, 0x00,  // 73 - 'I'
  0x05, 0x20, 0x40, 0x41, 0x3f, 0x01, 0x00, 0x00,  // 74 - 'J'
  0x05, 0x7f, 0x08, 0x14, 0x22, 0x41, 0x00, 0x00,  // 75 - 'K'
  0x05, 0x7f, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00,  // 76 - 'L'
  0x05, 0x7f, 0x02, 0x1c, 0x02, 0x7f, 0x00, 0x00,  // 77 - 'M'
  0x05, 0x7f, 0x04, 0x08, 0x10, 0x7f, 0x00, 0x00,  // 78 - 'N'
  0x05, 0x3e, 0x41, 0x41, 0x41, 0x3e, 0x00, 0x00,  // 79 - 'O'
  0x05, 0x7f, 0x09, 0x09, 0x09, 0x06, 0x00, 0x00,  // 80 - 'P'
  0x05, 0x3e, 0x41, 0x51, 0x21, 0x5e, 0x00, 0x00,  // 81 - 'Q'
  0x05, 0x7f, 0x09, 0x19, 0x29, 0x46, 0x00, 0x00,  // 82 - 'R'
  0x05, 0x26, 0x49, 0x49, 0x49, 0x32, 0x00, 0x00,  // 83 - 'S'
  0x05, 0x03, 0x01, 0x7f, 0x01, 0x03, 0x00, 0x00,  // 84 - 'T'
  0x05, 0x3f, 0x40, 0x40, 0x40, 0x3f, 0x00, 0x00,  // 85 - 'U'
  0x05, 0x1f, 0x20, 0x40, 0x20, 0x1f, 0x00, 0x00,  // 86 - 'V'
  0x05, 0x3f, 0x40, 0x38, 0x40, 0x3f, 0x00, 0x00,  // 87 - 'W'
  0x05, 0x63, 0x14, 0x08, 0x14, 0x63, 0x00, 0x00,  // 88 - 'X'
  0x05, 0x03, 0x04, 0x78, 0x04, 0x03, 0x00, 0x00,  // 89 - 'Y'
  0x05, 0x61, 0x59, 0x49, 0x4d, 0x43, 0x00, 0x00,  // 90 - 'Z'
  0x03, 0x7f, 0x41, 0x41, 0x00, 0x00, 0x00, 0x00,  // 91 - '['
  0x05, 0x02, 0x04, 0x08, 0x10, 0x20, 0x00, 0x00,  // 92 - '\'
  0x03, 0x41, 0x41, 0x7f, 0x00, 0x00, 0x00, 0x00,  // 93 - ']'
  0x05, 0x04, 0x02, 0x01, 0x02, 0x04, 0x00, 0x00,  // 94 - '^'
  0x05, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00,  // 95 - '_'
  0x02, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00,  // 96 - '`'
  0x05, 0x20, 0x54, 0x54, 0x78, 0x40, 0x00, 0x00,  // 97 - 'a'
  0x05, 0x7f, 0x28, 0x44, 0x44, 0x38, 0x00, 0x00,  // 98 - 'b'
  0x05, 0x38, 0x44, 0x44, 0x44, 0x28, 0x00, 0x00,  // 99 - 'c'
  0x05, 0x38, 0x44, 0x44, 0x28, 0x7f, 0x00, 0x00,  // 100 - 'd'
  0x05, 0x38, 0x54, 0x54, 0x54, 0x18, 0x00, 0x00,  // 101 - 'e'
  0x04, 0x08, 0x7e, 0x09, 0x02, 0x00, 0x00, 0x00,  // 102 - 'f'
  0x05, 0x18, 0xa4, 0xa4, 0x9c, 0x78, 0x00, 0x00,  // 103 - 'g'
  0x05, 0x7f, 0x08, 0x04, 0x04, 0x78, 0x00, 0x00,  // 104 - 'h'
  0x03, 0x44, 0x7d, 0x40, 0x00, 0x00, 0x00, 0x00,  // 105 - 'i'
  0x04, 0x40, 0x80, 0x80, 0x7a, 0x00, 0x00, 0x00,  // 106 - 'j'
  0x04, 0x7f, 0x10, 0x28, 0x44, 0x00, 0x00, 0x00,  // 107 - 'k'
  0x03, 0x41, 0x7f, 0x40, 0x00, 0x00, 0x00, 0x00,  // 108 - 'l'
  0x05, 0x7c, 0x04, 0x78, 0x04, 0x78, 0x00, 0x00,  // 109 - 'm'
  0x05, 0x7c, 0x08, 0x04, 0x04, 0x78, 0x00, 0x00,  // 110 - 'n'
  0x05, 0x38, 0x44, 0x44, 0x44, 0x38, 0x00, 0x00,  // 111 - 'o'
  0x05, 0xfc, 0x18, 0x24, 0x24, 0x18, 0x00, 0x00,  // 112 - 'p'
  0x05, 0x18, 0x24, 0x24, 0x18, 0xfc, 0x00, 0x00,  // 113 - 'q'
  0x05, 0x7c, 0x08, 0x04, 0x04, 0x08, 0x00, 0x00,  // 114 - 'r'
  0x05, 0x48, 0x54, 0x54, 0x54, 0x24, 0x00, 0x00,  // 115 - 's'
  0x04, 0x04, 0x3f, 0x44, 0x24, 0x00, 0x00, 0x00,  // 116 - 't'
  0x05, 0x3c, 0x40, 0x40, 0x20, 0x7c, 0x00, 0x00,  // 117 - 'u'
  0x05, 0x1c, 0x20, 0x40, 0x20, 0x1c, 0x00, 0x00,  // 118 - 'v'
  0x05, 0x3c, 0x40, 0x30, 0x40, 0x3c, 0x00, 0x00,  // 119 - 'w'
  0x05, 0x44, 0x28, 0x10, 0x28, 0x44, 0x00, 0x00,  // 120 - 'x'
  0x05, 0x4c, 0x90, 0x90, 0x90, 0x7c, 0x00, 0x00,  // 121 - 'y'
  0x05, 0x44, 0x64, 0x54, 0x4c, 0x44, 0x00, 0x00,  // 122 - 'z'
  0x03, 0x08, 0x36, 0x41, 0x00, 0x00, 0x00, 0x00,  // 123 - '{'
  0x01, 0x77, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // 124 - '|'
  0x03, 0x41, 0x36, 0x08, 0x00, 0x00, 0x00, 0x00,  // 125 - '}'
  0x05, 0x02, 0x01, 0x02, 0x04, 0x02, 0x00, 0x00,  // 126 - '~'
  0x05, 0x3c, 0x26, 0x23, 0x26, 0x3c, 0x00, 0x00,  // 127 - 'Hollow Up Arrow'
  0x07, 0x0E, 0x1F, 0x3F, 0x7E, 0x3F, 0x1F, 0x0E,  // 128 (200) - heart СЕРЦЕ
  0x07, 0x0E, 0x11, 0x04, 0x7A, 0x04, 0x11, 0x0E,  // 129 (201)антена точки доступа 
  0x04, 0x06, 0x09, 0x09, 0x06, 0x00, 0x00, 0x00,  // 130 (202) - градус цельсия
  0x05, 0x7E, 0x81, 0x81, 0xC3, 0x42, 0x00, 0x00,  // 131 (203) - C - большая
  0x05, 0x3e, 0x49, 0x49, 0x49, 0x22, 0x00, 0x00,  // 132 - 'Є ukr'
  0x05, 0x7E, 0x02, 0x02, 0x02, 0x01, 0x00, 0x00,  // 133 - 'Ґ ukr'
  0x03, 0x41, 0x7f, 0x41, 0x00, 0x00, 0x00, 0x00,  // 134 - 'I ukr'
  0x05, 0x01, 0x40, 0x7E, 0x40, 0x01, 0x00, 0x00,  // 135 - 'Ї ukr'
  0x05, 0x04, 0x02, 0x7F, 0x02, 0x04, 0x00, 0x00,  // 136 (210) - 'стрелка вверх 0 градусов'
  0x05, 0x10, 0x20, 0x7F, 0x20, 0x10, 0x00, 0x00,  // 137 (211) - 'стрелка вниз 180 градусов'
  0x05, 0x60, 0xFE, 0xF9, 0xFE, 0x60, 0x00, 0x00,  // 138 (212) - 'Градусник'
  0x05, 0x38, 0x44, 0x43, 0x44, 0x38, 0x00, 0x00,  // 139 (213) - 'Капелька'
  0x06, 0x24, 0x12, 0x12, 0x24, 0x24, 0x12, 0x00,  // 140 (214) - 'Ветер'
  0x05, 0x44, 0x48, 0x5F, 0x48, 0x44, 0x00, 0x00,  // 141 (215) - 'Давление'
  0x07, 0x1C, 0x22, 0x22, 0x24, 0x28, 0x24, 0x18,  // 142 (216) - 'Облачность'
  0x07, 0x3C, 0x42, 0x95, 0xA1, 0x95, 0x42, 0x3C,  // 143 (217) - ' :) '
  0x07, 0x3C, 0x42, 0xA5, 0x91, 0xA5, 0x42, 0x3C,  // 144 (220) - ' :( '
  0x05, 0x14, 0x3E, 0x55, 0x41, 0x22, 0x00, 0x00,  // 145 (221) - 'Евро'
  0x05, 0x14, 0x35, 0x5D, 0x56, 0x14, 0x00, 0x00,  // 146 (222) - 'Гривна'
  0x07, 0x64, 0x54, 0x4C, 0x00, 0x08, 0x7F, 0x04,  // 147 (223) - 'Злотый'
  0x05, 0x38, 0x54, 0x54, 0x44, 0x28, 0x00, 0x00,  // 148 (224) - 'є ukr'
  0x04, 0x7c, 0x04, 0x04, 0x02, 0x00, 0x00, 0x00,  // 149 (225) - 'ґ ukr'
  0x03, 0x44, 0x7d, 0x40, 0x00, 0x00, 0x00, 0x00,  // 150 (226) - 'i ukr'
  0x03, 0x4A, 0x78, 0x42, 0x00, 0x00, 0x00, 0x00,  // 151 (227) - 'ї ukr'
  0x07, 0x08, 0x1C, 0x2A, 0x08, 0x08, 0x08, 0x08,  // 152 (230) - 'Стрелка влево 270 градусов'
  0x07, 0x08, 0x08, 0x08, 0x08, 0x2A, 0x1C, 0x08,  // 153 (231) - 'Стрелка вправо 90 градусов'
  0x07, 0x40, 0x20, 0x10, 0x09, 0x05, 0x03, 0x0F,  // 154 (232) - 'Стрелка 45 градусов'
  0x07, 0x01, 0x02, 0x04, 0x48, 0x50, 0x60, 0x78,  // 155 (233) - 'Стрелка 135 градусов'
  0x07, 0x78, 0x60, 0x50, 0x48, 0x04, 0x02, 0x01,  // 156 (234) - 'Стрелка 225 градусов'
  0x07, 0x0F, 0x03, 0x05, 0x09, 0x10, 0x20, 0x40,  // 157 (235) - 'Стрелка 315 градусов'
  0x05, 0xff, 0x09, 0x29, 0xf6, 0x20, 0x00, 0x00,  // 158 - 'R +'
  0x05, 0xc0, 0x88, 0x7e, 0x09, 0x03, 0x00, 0x00,  // 159 - 'f notation'
  0x05, 0x20, 0x54, 0x54, 0x79, 0x41, 0x00, 0x00,  // 160 - 'a acute'
  0x03, 0x44, 0x7d, 0x41, 0x00, 0x00, 0x00, 0x00,  // 161 - 'i acute'
  0x05, 0x30, 0x48, 0x48, 0x4a, 0x32, 0x00, 0x00,  // 162 - 'o acute'
  0x05, 0x38, 0x40, 0x40, 0x22, 0x7a, 0x00, 0x00,  // 163 - 'u acute'
  0x04, 0x7a, 0x0a, 0x0a, 0x72, 0x00, 0x00, 0x00,  // 164 - 'n accent'
  0x05, 0x7d, 0x0d, 0x19, 0x31, 0x7d, 0x00, 0x00,  // 165 - 'N accent'
  0x05, 0x26, 0x29, 0x29, 0x2f, 0x28, 0x00, 0x00,  // 166
  0x05, 0x26, 0x29, 0x29, 0x29, 0x26, 0x00, 0x00,  // 167
  0x05, 0x30, 0x48, 0x4d, 0x40, 0x20, 0x00, 0x00,  // 168 - 'Inverted ?'
  0x05, 0x38, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00,  // 169 - 'LH top corner'
  0x05, 0x08, 0x08, 0x08, 0x08, 0x38, 0x00, 0x00,  // 170 - 'RH top corner'
  0x05, 0x2f, 0x10, 0xc8, 0xac, 0xba, 0x00, 0x00,  // 171 - '1/2'
  0x05, 0x2f, 0x10, 0x28, 0x34, 0xfa, 0x00, 0x00,  // 172 - '1/4'
  0x01, 0x7b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // 173 - '| split'
  0x05, 0x08, 0x14, 0x2a, 0x14, 0x22, 0x00, 0x00,  // 174 - '<<'
  0x05, 0x22, 0x14, 0x2a, 0x14, 0x08, 0x00, 0x00,  // 175 - '>>'
  0x04, 0x06, 0x09, 0x09, 0x06, 0x00, 0x00, 0x00,  // 176 - 'градус Цельсия''
  0x05, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x00, 0x00,  // 177 - '50% shading'
  0x05, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00,  // 178 - 'Right side'
  0x05, 0x10, 0x10, 0x10, 0x10, 0xff, 0x00, 0x00,  // 179 - 'Right T'
  0x05, 0x14, 0x14, 0x14, 0x14, 0xff, 0x00, 0x00,  // 180 - 'Right T double H'
  0x05, 0x10, 0x10, 0xff, 0x00, 0xff, 0x00, 0x00,  // 181 - 'Right T double V'
  0x05, 0x10, 0x10, 0xf0, 0x10, 0xf0, 0x00, 0x00,  // 182 - 'Top Right double V'
  0x05, 0x14, 0x14, 0x14, 0x14, 0xfc, 0x00, 0x00,  // 183 - 'Top Right double H'
  0x05, 0x14, 0x14, 0xf7, 0x00, 0xff, 0x00, 0x00,  // 184 - 'Right T double all'
  0x05, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00,  // 185 - 'Right side double'
  0x05, 0x14, 0x14, 0xf4, 0x04, 0xfc, 0x00, 0x00,  // 186 - 'Top Right double'
  0x05, 0x14, 0x14, 0x17, 0x10, 0x1f, 0x00, 0x00,  // 187 - 'Bot Right double'
  0x05, 0x10, 0x10, 0x1f, 0x10, 0x1f, 0x00, 0x00,  // 188 - 'Bot Right double V'
  0x05, 0x14, 0x14, 0x14, 0x14, 0x1f, 0x00, 0x00,  // 189 - 'Bot Right double H'
  0x05, 0x10, 0x10, 0x10, 0x10, 0xf0, 0x00, 0x00,  // 190 - 'Top Right'
  0x05, 0x00, 0x00, 0x00, 0x1f, 0x10, 0x00, 0x00,  // 191 - 'Bot Left'
  0x05, 0x7e, 0x11, 0x11, 0x11, 0x7e, 0x00, 0x00,  // 192 - 'А'  x0c0
  0x05, 0x7f, 0x49, 0x49, 0x49, 0x31, 0x00, 0x00,  // 193 - 'Б'  x0c1
  0x05, 0x7f, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00,  // 194 - 'B'  x0c2
  0x05, 0x7f, 0x01, 0x01, 0x01, 0x03, 0x00, 0x00,  // 195 - 'Г'  x0c3
  0x06, 0xc0, 0x7e, 0x41, 0x41, 0x7e, 0xc0, 0x00,  // 196 - 'Д'  x0c4
  0x05, 0x7f, 0x49, 0x49, 0x49, 0x41, 0x00, 0x00,  // 197 - 'E'  x0c5
  0x05, 0x77, 0x08, 0x7f, 0x08, 0x77, 0x00, 0x00,  // 198 - 'Ж'  x0c6
  0x05, 0x41, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00,  // 199 - 'З'  x0c7
  0x05, 0x7f, 0x10, 0x08, 0x04, 0x7f, 0x00, 0x00,  // 200 - 'И'  x0c8
  0x05, 0x7f, 0x10, 0x09, 0x04, 0x7f, 0x00, 0x00,  // 201 - 'Й'  x0c9
  0x05, 0x7f, 0x08, 0x14, 0x22, 0x41, 0x00, 0x00,  // 202 - 'K'  x0ca
  0x05, 0x40, 0x3e, 0x01, 0x01, 0x7f, 0x00, 0x00,  // 203 - 'Л'  x0cb
  0x05, 0x7f, 0x02, 0x0c, 0x02, 0x7f, 0x00, 0x00,  // 204 - 'M'  x0cc
  0x05, 0x7f, 0x08, 0x08, 0x08, 0x7f, 0x00, 0x00,  // 205 - 'H'  x0cd
  0x05, 0x3e, 0x41, 0x41, 0x41, 0x3e, 0x00, 0x00,  // 206 - 'O'  x0ce
  0x05, 0x7f, 0x01, 0x01, 0x01, 0x7f, 0x00, 0x00,  // 207 - 'П'  x0cf
  0x05, 0x7f, 0x09, 0x09, 0x09, 0x06, 0x00, 0x00,  // 208 - 'Р'  x0d0
  0x05, 0x3e, 0x41, 0x41, 0x41, 0x22, 0x00, 0x00,  // 209 - 'C'  x0d1
  0x05, 0x03, 0x01, 0x7f, 0x01, 0x03, 0x00, 0x00,  // 210 - 'T'  x0d2
  0x05, 0x27, 0x48, 0x48, 0x48, 0x3f, 0x00, 0x00,  // 211 - 'У'  x0d3
  0x07, 0x1c, 0x22, 0x22, 0x7f, 0x22, 0x22, 0x1c,  // 212 - 'Ф'  x0d4
  0x05, 0x63, 0x14, 0x08, 0x14, 0x63, 0x00, 0x00,  // 213 - 'X'  x0d5
  0x06, 0x7f, 0x40, 0x40, 0x40, 0x7f, 0xc0, 0x00,  // 214 - 'Ц'  x0d6
  0x05, 0x07, 0x08, 0x08, 0x08, 0x7f, 0x00, 0x00,  // 215 - 'Ч'  x0d7
  0x05, 0x7f, 0x40, 0x7e, 0x40, 0x7f, 0x00, 0x00,  // 216 - 'Ш'  x0d8
  0x06, 0x7f, 0x40, 0x7e, 0x40, 0x7f, 0xc0, 0x00,  // 217 - 'Щ'  x0d9
  0x07, 0x03, 0x01, 0x7f, 0x48, 0x48, 0x30, 0x00,  // 218 - 'Ъ'  x0da
  0x07, 0x7f, 0x48, 0x48, 0x30, 0x00, 0x7F, 0x00,  // 219 - 'Ы'  x0db
  0x05, 0x7f, 0x48, 0x48, 0x48, 0x30, 0x00, 0x00,  // 220 - 'Ь'  x0dc
  0x05, 0x22, 0x41, 0x49, 0x49, 0x3e, 0x00, 0x00,  // 221 - 'Э'  x0dd
  0x06, 0x7f, 0x08, 0x3e, 0x41, 0x41, 0x3e, 0x00,  // 222 - 'Ю'  x0de
  0x05, 0x46, 0x29, 0x19, 0x09, 0x7f, 0x00, 0x00,  // 223 - 'Я'  x0df
  0x05, 0x20, 0x54, 0x54, 0x54, 0x78, 0x00, 0x00,  // 224 - 'а'  x0e0
  0x05, 0x3c, 0x4a, 0x4a, 0x49, 0x31, 0x00, 0x00,  // 225 - 'б'  x0e1
  0x05, 0x7c, 0x54, 0x54, 0x54, 0x28, 0x00, 0x00,  // 226 - 'в'  x0e2
  0x04, 0x7c, 0x04, 0x04, 0x0c, 0x00, 0x00, 0x00,  // 227 - 'г'  x0e3
  0x06, 0xc0, 0x78, 0x44, 0x44, 0x78, 0xc0, 0x00,  // 228 - 'д'  x0e4
  0x05, 0x38, 0x54, 0x54, 0x54, 0x18, 0x00, 0x00,  // 229 - 'e'  x0e5
  0x05, 0x6c, 0x10, 0x7c, 0x10, 0x6c, 0x00, 0x00,  // 230 - 'ж'  x0e6
  0x05, 0x44, 0x54, 0x54, 0x54, 0x28, 0x00, 0x00,  // 231 - 'з'  x0e7
  0x05, 0x7c, 0x20, 0x10, 0x08, 0x7c, 0x00, 0x00,  // 232 - 'и'  x0e8
  0x05, 0x7c, 0x20, 0x12, 0x08, 0x7c, 0x00, 0x00,  // 233 - 'й'  x0e9
  0x05, 0x7c, 0x10, 0x10, 0x28, 0x44, 0x00, 0x00,  // 234 - 'к'  x0ea
  0x05, 0x40, 0x38, 0x04, 0x04, 0x7c, 0x00, 0x00,  // 235 - 'л'  x0eb
  0x05, 0x7c, 0x08, 0x10, 0x08, 0x7c, 0x00, 0x00,  // 236 - 'м'  x0ec
  0x05, 0x7c, 0x10, 0x10, 0x10, 0x7c, 0x00, 0x00,  // 237 - 'н'  x0ed
  0x05, 0x38, 0x44, 0x44, 0x44, 0x38, 0x00, 0x00,  // 238 - 'o'  x0ee
  0x05, 0x7c, 0x04, 0x04, 0x04, 0x7c, 0x00, 0x00,  // 239 - 'п'  x0ef
  0x05, 0xfc, 0x24, 0x24, 0x24, 0x18, 0x00, 0x00,  // 240 - 'р'  x0f0
  0x04, 0x38, 0x44, 0x44, 0x44, 0x00, 0x00, 0x00,  // 241 - 'с'  x0f1
  0x05, 0x0c, 0x04, 0x7c, 0x04, 0x0c, 0x00, 0x00,  // 242 - 'т'  x0f2
  0x05, 0x4c, 0x90, 0x90, 0x50, 0x3c, 0x00, 0x00,  // 243 - 'у'  x0f3
  0x07, 0x10, 0x28, 0x28, 0xfc, 0x28, 0x28, 0x10,  // 244 - 'ф'  x0f4
  0x05, 0x44, 0x28, 0x10, 0x28, 0x44, 0x00, 0x00,  // 245 - 'x'  x0f5
  0x05, 0x7c, 0x40, 0x40, 0x7c, 0xc0, 0x00, 0x00,  // 246 - 'ц'  x0f6
  0x05, 0x0c, 0x10, 0x10, 0x10, 0x7c, 0x00, 0x00,  // 247 - 'ч'  x0f7
  0x05, 0x7c, 0x40, 0x78, 0x40, 0x7c, 0x00, 0x00,  // 248 - 'ш'  x0f8
  0x06, 0x7c, 0x40, 0x78, 0x40, 0x7c, 0xc0, 0x00,  // 249 - 'щ'  x0f9
  0x06, 0x04, 0x7c, 0x50, 0x50, 0x50, 0x20, 0x00,  // 250 - 'ъ'  x0fa
  0x05, 0x7c, 0x50, 0x50, 0x20, 0x7c, 0x00, 0x00,  // 251 - 'ы'  x0fb
  0x05, 0x7c, 0x50, 0x50, 0x50, 0x20, 0x00, 0x00,  // 252 - 'ь'  x0fc
  0x05, 0x28, 0x44, 0x54, 0x54, 0x38, 0x00, 0x00,  // 253 - 'э'  x0fd
  0x06, 0x7c, 0x10, 0x38, 0x44, 0x44, 0x38, 0x00,  // 254 - 'ю'  x0fe
  0x05, 0x48, 0x34, 0x14, 0x14, 0x7c, 0x00, 0x00,  // 255 - 'я'  x0ff

  0x05, 0x7e, 0x4b, 0x4a, 0x4b, 0x42, 0x00, 0x00,  // 256 - 'Ё' 0x100
  0x05, 0x38, 0x55, 0x54, 0x55, 0x18, 0x00, 0x00,  // 257 - 'ё' 0x101
  
  0x05, 0x18, 0x18, 0x24, 0x42, 0xff, 0x00, 0x00,  // 258 - Vol 0x102
};
