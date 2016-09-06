#include "TS_DISCO_F746NG.h"
#include "LCD_DISCO_F746NG.h"
#include "ThethingsiO_DISCO_F746NG.h"

int result;

LCD_DISCO_F746NG lcd;
TS_DISCO_F746NG ts;

int touched_left, touched_right ;
stringstream temp_string;

int main()
{
    ThethingsiO_DISCO_F746NG thethings = ThethingsiO_DISCO_F746NG::ThethingsiO_DISCO_F746NG("HQ0dPZ1MGCbhE6gGtZMk7w8s80Dtjf0VtiWKghbTYSw");
    //printf("IP Address is %s\n\r", interface.getIPAddress());
    TS_StateTypeDef TS_State;
    uint16_t x, y, x_ant, y_ant;

    touched_left = touched_right = 0;
    lcd.Clear(LCD_COLOR_BLACK);

    lcd.SetTextColor(LCD_COLOR_RED);
    lcd.DrawCircle(160, 136, 50);
    wait(0.1);
    lcd.SetTextColor(LCD_COLOR_RED);
    lcd.FillCircle(160, 136, 50);
    wait(1);
    lcd.SetTextColor(LCD_COLOR_GREEN);
    lcd.DrawCircle(320, 136, 50);
    wait(0.1);
    lcd.SetTextColor(LCD_COLOR_GREEN);
    lcd.FillCircle(320, 136, 50);
    wait(1);
    ts.Init(lcd.GetXSize(), lcd.GetYSize());
    wait(3);

    lcd.SetBackColor(LCD_COLOR_BLACK);
    lcd.SetTextColor(LCD_COLOR_WHITE);
    printf("\r\n");
    while (1) {
        ts.GetState(&TS_State);
        if (TS_State.touchDetected) {
            x = TS_State.touchX[0];
            y = TS_State.touchY[0];
            if ((x_ant != x) && (y_ant != y)) {
                if ((x >= 110 && x <= 210)&&(y >= 86 && y <= 186)) {
                    touched_left++;
                    temp_string.str("");
                    temp_string << touched_left;
                    lcd.DisplayStringAt(400, LINE(1), (uint8_t *)temp_string.str().c_str(), CENTER_MODE);
                    thethings.thingWrite("demo", "1");
                } else if ((x >= 270 && x <= 370)&&(y >= 86 && y <= 186)) {
                    touched_right++;
                    temp_string.str("");
                    temp_string << touched_right;
                    lcd.DisplayStringAt(110, LINE(1), (uint8_t *)temp_string.str().c_str(), CENTER_MODE);
                    thethings.thingWrite("demo", "2");
                }
            }
            x_ant = x;
            y_ant = y;
        }
    }
}