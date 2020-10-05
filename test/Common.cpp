#include <bitset>
#include <iostream>
#include <vector>

#include "Arduino.h"
#include "ArduinoUnitTests.h"
#include "LiquidCrystal_CI.h"
#include "ci/ObservableDataStream.h"

const byte rs = 1;
const byte rw = 2;
const byte enable = 3;
const byte d0 = 10;
const byte d1 = 11;
const byte d2 = 12;
const byte d3 = 13;
const byte d4 = 14;
const byte d5 = 15;
const byte d6 = 16;
const byte d7 = 17;

class BitCollector : public DataStreamObserver {
private:
  bool fourBitMode;
  bool showData;
  vector<int> pinLog;
  GodmodeState *state;

public:
  BitCollector(bool showData = false, bool fourBitMode = true)
      : DataStreamObserver(false, false) {
    this->fourBitMode = fourBitMode;
    this->showData = showData;
    state = GODMODE();
    state->reset();
    state->digitalPin[enable].addObserver("lcd", this);
  }

  ~BitCollector() { state->digitalPin[enable].removeObserver("lcd"); }

  virtual void onBit(bool aBit) {
    if (aBit) {
      int value = 0;
      value = (value << 1) + state->digitalPin[rs];
      value = (value << 1) + state->digitalPin[rw];
      value = (value << 1) + state->digitalPin[d7];
      value = (value << 1) + state->digitalPin[d6];
      value = (value << 1) + state->digitalPin[d5];
      value = (value << 1) + state->digitalPin[d4];
      value = (value << 1) + state->digitalPin[d3];
      value = (value << 1) + state->digitalPin[d2];
      value = (value << 1) + state->digitalPin[d1];
      value = (value << 1) + state->digitalPin[d0];
      pinLog.push_back(value);
      if (showData) {
        std::cout.width(5);
        std::cout << std::right << value << " : " << ((value >> 9) & 1) << "  "
                  << ((value >> 8) & 1) << "  ";
        if (fourBitMode) {
          std::bitset<4> bits((value >> 4) & 0x0F);
          if ((pinLog.size() - 1) % 2) {
            std::cout << "    ";
          }
          std::cout << bits;
        } else {
          std::bitset<8> bits(value & 0xFF);
          std::cout << bits;
        }
        std::cout << std::endl;
      }
    }
  }

  bool isEqualTo(const vector<int> &expected) {
    if (pinLog.size() != expected.size()) {
      return false;
    }
    for (int i = 0; i < pinLog.size(); ++i) {
      if (pinLog.at(i) != expected.at(i)) {
        return false;
      }
    }
    return true;
  }

  virtual String observerName() const { return "BitCollector"; }
};

// we don't look at the pins here, just verify that we can call the constructors
unittest(constructors) {
  LiquidCrystal_Test lcd1(rs, enable, d4, d5, d6, d7);
  LiquidCrystal_Test lcd2(rs, rw, enable, d4, d5, d6, d7);
  LiquidCrystal_Test lcd3(rs, enable, d0, d1, d2, d3, d4, d5, d6, d7);
  LiquidCrystal_Test lcd4(rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7);
  LiquidCrystal_Test *lcd5 = new LiquidCrystal_Test(rs, enable, d4, d5, d6, d7);
  LiquidCrystal_Test *lcd6 =
      new LiquidCrystal_Test(rs, rw, enable, d4, d5, d6, d7);
  LiquidCrystal_Test *lcd7 =
      new LiquidCrystal_Test(rs, enable, d0, d1, d2, d3, d4, d5, d6, d7);
  LiquidCrystal_Test *lcd8 =
      new LiquidCrystal_Test(rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7);
  assertNotNull(lcd5);
  assertNotNull(lcd6);
  assertNotNull(lcd7);
  assertNotNull(lcd8);
  delete lcd8;
  delete lcd7;
  delete lcd6;
  delete lcd5;
}

/*     rs rw  d7 to d0
   48 : 0  0  00110000      set to 8-bit mode (takes three tries)
   48 : 0  0  00110000      set to 8-bit mode
   48 : 0  0  00110000      set to 8-bit mode
   32 : 0  0  00100000      set to 4-bit mode, 1 line, 8-bit font
   32 : 0  0  0010          \
    0 : 0  0      0000       set to 4-bit mode, 1 line, 8-bit font
    0 : 0  0  0000          \
  192 : 0  0      1100       display on, cursor off, blink off
    0 : 0  0  0000          \
  016 : 0  0      0001       clear display
    0 : 0  0  0000          \
   96 : 0  0      0110       increment cursor position, no display shift
*/
unittest(init) {
  vector<int> expected{48, 48, 48, 32, 32, 0, 0, 192, 0, 16, 0, 96};
  BitCollector pinValues(false); // test the next line (a constructor)
  LiquidCrystal_Test lcd(rs, enable, d4, d5, d6, d7);
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
   48 : 0  0  00110000      set to 8-bit mode (takes three tries)
   48 : 0  0  00110000      set to 8-bit mode
   48 : 0  0  00110000      set to 8-bit mode
   32 : 0  0  00100000      set to 4-bit mode, 1 line, 8-bit font
   32 : 0  0  0010          \
  128 : 0  0      1000       set to 4-bit mode, 2 lines, 8-bit font
    0 : 0  0  0000          \
  192 : 0  0      1100       display on, cursor off, blink off
    0 : 0  0  0000          \
  016 : 0  0      0001       clear display
    0 : 0  0  0000          \
   96 : 0  0      0110       increment cursor position, no display shift
*/
unittest(begin) {
  vector<int> expected{48, 48, 48, 32, 32, 128, 0, 192, 0, 16, 0, 96};
  LiquidCrystal_Test lcd(rs, enable, d4, d5, d6, d7);
  BitCollector pinValues(false); // test the next line
  lcd.begin(16, 2);
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
      0 : 0  0  0000
    208 : 0  0      1101  00001101 = display on, cursor blink on
*/
unittest(blink) {
  vector<int> expected{0, 208};
  LiquidCrystal_Test lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.blink();
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
      0 : 0  0  0000
    192 : 0  0      1100  00001100 = display on, cursor blink off
*/
unittest(noBlink) {
  vector<int> expected{0, 192};
  LiquidCrystal_Test lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.noBlink();
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
      0 : 0  0  0000
    224 : 0  0      1110  00001110 = display on, cursor on
*/
unittest(cursor) {
  vector<int> expected{0, 224};
  LiquidCrystal_Test lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.cursor();
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
      0 : 0  0  0000
    192 : 0  0      1100  00001100 = display on, cursor off
*/
unittest(noCursor) {
  vector<int> expected{0, 192};
  LiquidCrystal_Test lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.noCursor();
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
   64 : 0  0  0100
    0 : 0  0      0000
  512 : 1  0  0000
  512 : 1  0      0000
  528 : 1  0  0001
  528 : 1  0      0001
  512 : 1  0  0000
  512 : 1  0      0000
  512 : 1  0  0000
  512 : 1  0      0000
  528 : 1  0  0001
  528 : 1  0      0001
  512 : 1  0  0000
  736 : 1  0      1110
  512 : 1  0  0000
  512 : 1  0      0000
  512 : 1  0  0000
  512 : 1  0      0000
*/
unittest(createChar) {
  vector<int> expected{64,  0,   512, 512, 528, 528, 512, 512, 512,
                       512, 528, 528, 512, 736, 512, 512, 512, 512};
  byte smiley[8] = {B00000, B10001, B00000, B00000, B10001, B01110, B00000};
  LiquidCrystal_Test lcd(rs, enable, d4, d5, d6, d7);
  BitCollector pinValues(false); // test the next line
  lcd.createChar(0, smiley);
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
    0 : 0  0  0000          \
   16 : 0  0      0001       clear
*/
unittest(clear) {
  vector<int> expected{0, 16};
  LiquidCrystal_Test lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.clear();
  assertTrue(pinValues.isEqualTo(expected));
}

/*      rs rw  d7 to d0
    576 : 1  0  0100      \
    640 : 1  0      1000  0x48 H
    608 : 1  0  0110      \
    592 : 1  0      0101  0x65 e
    608 : 1  0  0110      \
    704 : 1  0      1100  0x6C l
    608 : 1  0  0110      \
    704 : 1  0      1100  0x6C l
    608 : 1  0  0110      \
    752 : 1  0      1111  0x6F o
*/
unittest(print_hello) {
  vector<int> expected{576, 640, 608, 592, 608, 704, 608, 704, 608, 752};
  LiquidCrystal_Test lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.print("Hello");
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
   16 : 0  0  0001      \
  128 : 0  0      1000   00011000 = shift display left
*/
unittest(scrollDisplayLeft) {
  vector<int> expected{16, 128};
  LiquidCrystal_Test lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.scrollDisplayLeft();
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
   16 : 0  0  0001      first half of command
  192 : 0  0      1100  full command: 00011100 = shift display right
*/
unittest(scrollDisplayRight) {
  vector<int> expected{16, 192};
  LiquidCrystal_Test lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.scrollDisplayRight();
  assertTrue(pinValues.isEqualTo(expected));
}

unittest(failIfWrongSize) {
  vector<int> expected{0};
  LiquidCrystal_Test lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.clear();
  assertFalse(pinValues.isEqualTo(expected));
}

unittest(failIfWrongValues) {
  vector<int> expected{0, 255};
  LiquidCrystal_Test lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.clear();
  assertFalse(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
    0 : 0  0  0000
   32 : 0  0      0010
*/
unittest(home) {
  vector<int> expected{0, 32};
  LiquidCrystal_Test lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.home();
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
    0 : 0  0  0000
   96 : 0  0      0110  => left to right
*/
unittest(leftToRight) {
  vector<int> expected{0, 96};
  LiquidCrystal_Test lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.leftToRight();
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
    0 : 0  0  0000
   64 : 0  0      0100  => right to left
*/
unittest(rightToLeft) {
  vector<int> expected{0, 64};
  LiquidCrystal_Test lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.rightToLeft();
  assertTrue(pinValues.isEqualTo(expected));
}

/*       rs rw  d7 to d0
      0 : 0  0  0000      \
    192 : 0  0      1100   00001100 = turns on LCD display
*/
unittest(display) {
  vector<int> expected{0, 192};
  LiquidCrystal_Test lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.display();
  assertTrue(pinValues.isEqualTo(expected));
}

/*      rs rw  d7 to d0
    0 :  0  0  0000      \
  128 :  0  0      1000    00001000 = turns off LCD display
*/
unittest(noDisplay) {
  vector<int> expected{0, 128};
  LiquidCrystal_Test lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.noDisplay();
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
    0 : 0  0  0000
  112 : 0  0      0111
*/
unittest(autoscroll) {
  vector<int> expected{0, 112};
  LiquidCrystal_Test lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.autoscroll();
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
    0 : 0  0  0000
   96 : 0  0      0110
*/
unittest(noAutoscroll) {
  vector<int> expected{0, 96};
  LiquidCrystal_Test lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.noAutoscroll();
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
  128 : 0  0  1000      \
    0 : 0  0      0000  full command: 10000000 = set cursor (0,0)
  128 : 0  0  1000      \
   16 : 0  0      0001  full command: 10000001 = set cursor (1,0)
  128 : 0  0  1000      \
   32 : 0  0      0010  full command: 10000010 = set cursor (2,0)
  128 : 0  0  1000      \
   48 : 0  0      0011  full command: 10000011 = set cursor (3,0)
  128 : 0  0  1000      \
   64 : 0  0      0100  full command: 10000100 = set cursor (4,0)
  128 : 0  0  1000      \
   80 : 0  0      0101  full command: 10000101 = set cursor (5,0)
  128 : 0  0  1000      \
   96 : 0  0      0110  full command: 10000110 = set cursor (6,0)
  128 : 0  0  1000      \
  112 : 0  0      0111  full command: 10000111 = set cursor (7,0)
  128 : 0  0  1000      \
  128 : 0  0      1000  full command: 10001000 = set cursor (8,0)
  128 : 0  0  1000      \
  144 : 0  0      1001  full command: 10001001 = set cursor (9,0)
  128 : 0  0  1000      \
  160 : 0  0      1010  full command: 10001010 = set cursor (10,0)
  128 : 0  0  1000      \
  176 : 0  0      1011  full command: 10001011 = set cursor (11,0)
  128 : 0  0  1000      \
  192 : 0  0      1100  full command: 10001100 = set cursor (12,0)
  128 : 0  0  1000      \
  208 : 0  0      1101  full command: 10001101 = set cursor (13,0)
  128 : 0  0  1000      \
  224 : 0  0      1110  full command: 10001110 = set cursor (14,0)
  128 : 0  0  1000      \
  240 : 0  0      1111  full command: 10001111 = set cursor (15,0)

  192 : 0  0  1100      \
    0 : 0  0      0000  full command: 11000000 = set cursor (0,1)
  192 : 0  0  1100      \
   16 : 0  0      0001  full command: 11000001 = set cursor (1,1)
  192 : 0  0  1100      \
   32 : 0  0      0010  full command: 11000010 = set cursor (2,1)
  192 : 0  0  1100      \
   48 : 0  0      0011  full command: 11000011 = set cursor (3,1)
  192 : 0  0  1100      \
   64 : 0  0      0100  full command: 11000100 = set cursor (4,1)
  192 : 0  0  1100      \
   80 : 0  0      0101  full command: 11000101 = set cursor (5,1)
  192 : 0  0  1100      \
   96 : 0  0      0110  full command: 11000110 = set cursor (6,1)
  192 : 0  0  1100      \
  112 : 0  0      0111  full command: 11000111 = set cursor (7,1)
  192 : 0  0  1100      \
  128 : 0  0      1000  full command: 11001000 = set cursor (8,1)
  192 : 0  0  1100      \
  144 : 0  0      1001  full command: 11001001 = set cursor (9,1)
  192 : 0  0  1100      \
  160 : 0  0      1010  full command: 11001010 = set cursor (10,1)
  192 : 0  0  1100      \
  176 : 0  0      1011  full command: 11001011 = set cursor (11,1)
  192 : 0  0  1100      \
  192 : 0  0      1100  full command: 11001100 = set cursor (12,1)
  192 : 0  0  1100      \
  208 : 0  0      1101  full command: 11001101 = set cursor (13,1)
  192 : 0  0  1100      \
  224 : 0  0      1110  full command: 11001110 = set cursor (14,1)
  192 : 0  0  1100      \
  240 : 0  0      1111  full command: 11001111 = set cursor (15,1)
 */
unittest(setCursor) {
  vector<int> expected{
      128, 0,   128, 16,  128, 32,  128, 48,  128, 64,  128, 80,  128,
      96,  128, 112, 128, 128, 128, 144, 128, 160, 128, 176, 128, 192,
      128, 208, 128, 224, 128, 240, 192, 0,   192, 16,  192, 32,  192,
      48,  192, 64,  192, 80,  192, 96,  192, 112, 192, 128, 192, 144,
      192, 160, 192, 176, 192, 192, 192, 208, 192, 224, 192, 240,
  };
  LiquidCrystal_Test lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  // top row
  lcd.setCursor(0, 0);
  lcd.setCursor(1, 0);
  lcd.setCursor(2, 0);
  lcd.setCursor(3, 0);
  lcd.setCursor(4, 0);
  lcd.setCursor(5, 0);
  lcd.setCursor(6, 0);
  lcd.setCursor(7, 0);
  lcd.setCursor(8, 0);
  lcd.setCursor(9, 0);
  lcd.setCursor(10, 0);
  lcd.setCursor(11, 0);
  lcd.setCursor(12, 0);
  lcd.setCursor(13, 0);
  lcd.setCursor(14, 0);
  lcd.setCursor(15, 0);
  // bottom row
  lcd.setCursor(0, 1);
  lcd.setCursor(1, 1);
  lcd.setCursor(2, 1);
  lcd.setCursor(3, 1);
  lcd.setCursor(4, 1);
  lcd.setCursor(5, 1);
  lcd.setCursor(6, 1);
  lcd.setCursor(7, 1);
  lcd.setCursor(8, 1);
  lcd.setCursor(9, 1);
  lcd.setCursor(10, 1);
  lcd.setCursor(11, 1);
  lcd.setCursor(12, 1);
  lcd.setCursor(13, 1);
  lcd.setCursor(14, 1);
  lcd.setCursor(15, 1);
  assertTrue(pinValues.isEqualTo(expected));
}
