#include "mbed.h"

Serial pc(USBTX, USBRX); // Define serial TX RX

DigitalOut PWM1(p5);
DigitalOut PWM2(p6);
DigitalOut PWM3(p7);
DigitalOut PWM4(p8);




int main()
{
    char c = pc.getc();
    pc.printf("ready \r\n");
    while(1) {
        // specify period first
        if(pc.readable()) {
            c = pc.getc();
        }
        if((c == 'f')) { //if f presed clock wise
          
            PWM4.write(1);
            wait(.01);
            PWM4.write(0);

            wait(.01);

            PWM3.write(1);
            wait(.01);
            PWM3.write(0);

            wait(.01);

            PWM2.write(1);
            wait(.01);
            PWM2.write(0);

            wait(.01);

            PWM1.write(1);
            wait(.01);
            PWM1.write(0);
        }


//if r pressed, counter clock
        if((c == 'r')) {


            PWM1.write(1);
            wait(.01);
            PWM1.write(0);

            wait(.01);

            PWM2.write(1);
            wait(.01);
            PWM2.write(0);

            wait(.01);

            PWM3.write(1);
            wait(.01);
            PWM3.write(0);

            wait(.01);

            PWM4.write(1);
            wait(.01);
            PWM4.write(0);
        }


    }
}
