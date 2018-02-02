#include "mbed.h"
#include "Adafruit_SSD1306.h"
#include "Adafruit_GFX.h"
#define PI 3.1416


// an I2C sub-class that provides a constructed default
class I2CPreInit : public I2C
{
public:
    I2CPreInit(PinName sda, PinName scl) : I2C(sda, scl) {
        frequency(400000);
        start();
    };
};

I2CPreInit myI2C(p9,p10);
Adafruit_SSD1306_I2c myGUI(myI2C,p27,0x7a,64,128);

int main()
{

    myGUI.begin();
    wait_ms(2000); // 2 s for Logo to be displayed
    myGUI.clearDisplay();

    while(1) {
        for(double degree=0; degree<180; degree++) {
            double x1 = 0;
            double y1 = 0;
            double rad = 0;
            rad = degree * PI/180;
            x1 = 64 + (64 * cos(rad));
            y1 = 64 - (64 * sin(rad));
            myGUI.clearDisplay();
            myGUI.drawCircle(64, 64, 63, WHITE);
            myGUI.drawCircle(64, 64, 47, WHITE);
            myGUI.drawCircle(64, 64, 31, WHITE);
            myGUI.drawCircle(64, 64, 15, WHITE);
            myGUI.drawLine(0, 63, 128, 63, WHITE);
            myGUI.drawLine(64,64,x1,y1,WHITE);
            myGUI.display();
        }
        for(double degree=180; degree>0; degree--) {
            double x1 = 0;
            double y1 = 0;
            double rad = 0;
            rad = degree * PI/180;
            x1 = 64 + (64 * cos(rad));
            y1 = 64 - (64 * sin(rad));
            myGUI.clearDisplay();
           myGUI.drawCircle(64, 64, 63, WHITE);
            myGUI.drawCircle(64, 64, 47, WHITE);
            myGUI.drawCircle(64, 64, 31, WHITE);
            myGUI.drawCircle(64, 64, 15, WHITE);
            myGUI.drawLine(0, 63, 128, 63, WHITE);
            myGUI.drawLine(64,64,x1,y1,WHITE);
            myGUI.display();
        }
    }
}