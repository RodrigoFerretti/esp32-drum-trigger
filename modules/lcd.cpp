#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

void display_parameter(const char *name, int value, int position)
{
    lcd.setCursor(position, 0);
    lcd.print(name);
    lcd.print(":");
    if (value < 10)
    {
        lcd.print("00");
    }

    else if (value < 100)
    {
        lcd.print("0");
    }

    lcd.print(value);
}

void display_directory(const char *path, int position)
{
    lcd.setCursor(0, 2);
    lcd.print("                    ");
    std::string directory_name(path + 9);

    lcd.setCursor(0, 2);
    if (position < 10)
    {
        lcd.print("0");
    }
    lcd.print(position);
    lcd.print("-");
    lcd.print(directory_name.c_str());

    lcd.setCursor(0, 3);
    lcd.print("                    ");
}

void lcd_setup(int threshold, int resolution, int dynamic, const char *directory_path, int directory_position)
{
    lcd.init();
    lcd.backlight();

    display_parameter("TH", threshold, 0);
    display_parameter("RS", resolution, 7);
    display_parameter("DY", dynamic, 14);

    display_directory(directory_path, directory_position + 1);
}
