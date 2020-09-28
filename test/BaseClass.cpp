
#define LiquidCrystal_Test LiquidCrystal_Base
#include "Common.cpp"

unittest(testingClassName) {
  LiquidCrystal_Test lcd(rs, enable, d4, d5, d6, d7);
  assertEqual("LiquidCrystal_Base", lcd.className());
}

unittest_main()
