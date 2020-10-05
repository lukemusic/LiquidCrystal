
#define LiquidCrystal_Test LiquidCrystal
#include "Common.cpp"

unittest(testingClassName) {
  LiquidCrystal_Test lcd(rs, enable, d4, d5, d6, d7);
  assertEqual("LiquidCrystal_CI", lcd.className());
}

unittest(getRows) {
  LiquidCrystal_Test lcd(rs, enable, d4, d5, d6, d7);
  assertEqual(1, lcd.getRows());
  lcd.begin(16, 2);
  assertEqual(2, lcd.getRows());
}

unittest(clearBuffer) {
  // create lcd object
  LiquidCrystal_Test lcd(rs, enable, d4, d5, d6, d7);
  // get currently displayed lines
  std::vector<std::string> lines = lcd.getLines();
  // verify that display contains 1 empty line
  assertEqual(1, lines.size());
  assertEqual(0, lines.at(0).length());

  // reset lcd to have two lines
  lcd.begin(16, 2);
  // verify that begin clears the display
  lines = lcd.getLines();
  // verify that display contains 2 empty lines
  assertEqual(2, lines.size());
  assertEqual(0, lines.at(0).length());
  assertEqual(0, lines.at(1).length());

  // write something to display
  lcd.print("hello world");

  // verify display not empty
  lines = lcd.getLines();
  assertEqual(2, lines.size());
  // assertEqual(11, lines.at(0).length());
  // assertEqual("hello world", lines.at(0));
  assertEqual(0, lines.at(1).length());

  // clear display
  lcd.clear();

  // verify display is empty
  lines = lcd.getLines();
  assertEqual(2, lines.size());
  assertEqual(0, lines.at(0).length());
  assertEqual(0, lines.at(1).length());
}

unittest(display_noDisplay) {
  LiquidCrystal_Test lcd(rs, enable, d4, d5, d6, d7);

  bool isDisplay = lcd.isDisplay();

  // default
  assertEqual(0, isDisplay);

  // test is display on
  lcd.display();
  isDisplay = lcd.isDisplay();
  assertEqual(1, isDisplay);

  // test is display off
  lcd.noDisplay();
  isDisplay = lcd.isDisplay();
  assertEqual(0, isDisplay);
}

unittest(blink_noBlink) {
  // create lcd object
  LiquidCrystal_Test lcd(rs, enable, d4, d5, d6, d7);

  bool blinking = lcd.isBlink();

  // check default - noBlink
  assertEqual(0, blinking);

  // check blink function
  lcd.blink();
  blinking = lcd.isBlink();
  assertEqual(1, blinking);

  // check noBlink function
  lcd.noBlink();
  blinking = lcd.isBlink();
  assertEqual(0, blinking);
}

unittest(setCursorHighLevel) {
  // create lcd object
  LiquidCrystal_Test lcd(rs, enable, d4, d5, d6, d7);

  // reset lcd to have two lines
  lcd.begin(16, 2);

  // verify cursor is at beginning
  assertEqual(0, lcd.getCursorCol());
  assertEqual(0, lcd.getCursorRow());

  // set cursor to second line
  lcd.setCursor(0, 1);
  // verify cursor position
  assertEqual(0, lcd.getCursorCol());
  assertEqual(1, lcd.getCursorRow());

  // set cursor to middle of first line
  lcd.setCursor(4, 0);
  // verify cursor position
  assertEqual(4, lcd.getCursorCol());
  assertEqual(0, lcd.getCursorRow());
}

unittest_main()
