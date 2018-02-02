#include "mbed.h"
#include "sMotor.h"
#include "ultrasonic.h"
#include "circBuffer.h"
#include "Adafruit_SSD1306.h"
#include "LSM303DLHC.h"

#define PI 3.1416
#define Rad2Deg 57.2958

LSM303DLHC compass(p9, p10);//compass

char *cardDir;

// an I2C sub-class that provides a constructed default
class I2CPreInit : public I2C
{
public:
    I2CPreInit(PinName sda, PinName scl) : I2C(sda, scl) {
        frequency(400000);
        start();
    };
};

I2CPreInit gI2C(p9,p10);
Adafruit_SSD1306_I2c gOled2(gI2C,p27,0x7a,64,128);

#define CIRCBUF_DEF(x,y) fooptr x##_dataSpace[y]; circBuf_t x = {.buffer = x##_dataSpace,.head = 0,.tail = 0,.maxLen = y}
CIRCBUF_DEF(buffP1, 32);
CIRCBUF_DEF(buffP2, 32);
CIRCBUF_DEF(buffP3, 32);

InterruptIn pb(p13);

Serial pc(USBTX, USBRX);
sMotor motor(p14, p15, p16, p17); // creates new stepper motor: IN1, IN2, IN3, IN4

Ticker measure;

int step_speed = 2000 ; // set default motor speed
int numstep = 540 ; // defines full turn of 360 degree
int motordirection = 0;
int motorpositionstep = 0;
int sensorDistance;
bool pause = false;
double rad = 0;
//this is not hard-coded hacked way TODO
double foundx[20]= {-1};
double foundy[20]= {-1};
int numberFound=0;

void drawRadarPin()
{
    double x1 = 0;
    double y1 = 0;
    x1 = 64 + (64 * cos(rad));
    y1 = 64 - (64 * sin(rad));
    gOled2.clearDisplay();
    gOled2.printf("                  %s\r",cardDir);
    gOled2.drawCircle(64, 64, 63, WHITE);
    gOled2.drawCircle(64, 64, 47, WHITE);
    gOled2.drawCircle(64, 64, 31, WHITE);
    gOled2.drawCircle(64, 64, 15, WHITE);
    gOled2.drawLine(0, 63, 128, 63, WHITE);
    gOled2.drawLine(64,64,x1,y1,WHITE);

    //TODO remove hack
    for (int i =0; i<21; i++) {
        if (foundx[i] != -1) {
            gOled2.fillCircle(foundx[i], foundy[i],3,WHITE);
        }
    }

    gOled2.display();
}

void printDistance(void)
{
    if (sensorDistance < 500 && sensorDistance > 20) {
        //pc.printf("%d %d\r\n",motorpositionstep*180/270, sensorDistance);
        double scaledRange = sensorDistance*64/500;
        double x1=64+(scaledRange*cos(rad));
        double y1=64-(scaledRange*sin(rad));
        foundx[numberFound]=x1;
        foundy[numberFound]=y1;
        numberFound++;
    }
}

void moveMotor()
{
    motor.step(1, motordirection, step_speed);
    wait(0.01);
    if (motordirection == 0) {
        motorpositionstep++;
        if (motorpositionstep == 270) {
            motordirection = 1;
            //TODO remove hack
            for (int i =0; i<21; i++) {
                if (foundx[i] != -1) {
                    foundx[i]=-1;
                    foundy[i]=-1;
                    numberFound=0;
                }
            }
        }
    } else if (motordirection == 1) {
        motorpositionstep--;
        if (motorpositionstep == 0) {
            motordirection = 0;
            //TODO remove hack
            for (int i =0; i<21; i++) {
                if (foundx[i] != -1) {
                    foundx[i]=-1;
                    foundy[i]=-1;
                    numberFound=0;
                }
            }
        }
    }
    if (circBuffPush(&buffP3, &moveMotor)) {
        pc.printf("P3 buffer full\r\n");
    }
}

void readCompass()
{
    int magReading[3] = {0};
    double x,y;
    double deg;
    compass.readMag(magReading);

    x = magReading[0];
    y = magReading[1];

    deg = atan2(y,x);
    deg *= Rad2Deg;

    if (deg <0) deg = 360.0 + deg;
    //if (deg > 360.0) deg = deg - 360.0 ;

    if (337.5 <= deg && deg < 360) {
        cardDir = "N";
    } else if (0 <= deg && deg < 22.5) {
        cardDir = "N";
    } else if (22.5 <= deg && deg < 67.5) {
        cardDir = "NE";
    } else if (67.5 <= deg && deg < 112.5) {
        cardDir = "E";
    } else if (112.5 <= deg && deg < 157.5) {
        cardDir = "SE";
    } else if (157.5 <= deg && deg < 202.5) {
        cardDir = "S";
    } else if (202.5 <= deg && deg < 247.5) {
        cardDir = "SW";
    } else if (247.5 <= deg && deg < 292.5) {
        cardDir = "W";
    } else if (292.5 <= deg && deg < 337.5) {
        cardDir = "NW";
    } else {
        pc.printf("something went wrong!\r\n");
    }
}

void dist(int distance)
{
    sensorDistance = distance;
    rad = motorpositionstep * PI/270;
    if (circBuffPush(&buffP2, &printDistance)) {
        pc.printf("P2 buffer full\r\n");
    }
    if (circBuffPush(&buffP2, &readCompass)) {
        pc.printf("P2 buffer full\r\n");
    }
    if (circBuffPush(&buffP3, &drawRadarPin)) {
        pc.printf("P3 buffer full\r\n");
    }
}

ultrasonic mu(p11, p12, .1, 1, &dist);

void checkmeasure()
{
    mu.checkDistance();
}

void buttonAction()
{
    if (pause == true) {
        measure.detach();
        //pc.printf("Paused\r\n");
        if (circBuffPush(&buffP1, &buttonAction)) {
            pc.printf("P1 buffer full\r\n");
        }
    }
    if (pause == false) {
        measure.attach(&checkmeasure, 0.5);
    }
}

void flipISR()
{
    wait(0.01);
    if (pb.read()==0) {
        pause=!pause;
        if (circBuffPush(&buffP1, &buttonAction)) {
            pc.printf("P1 buffer full\r\n");
        }
    }
}

int main()
{
    fooptr runningPtr;

    mu.startUpdates();//start measuring the distance

    measure.attach(&checkmeasure, 0.5);

    compass.init();
    compass.setOffset(29.50, -0.50, 4.00); // example calibration
    compass.setScale(1.00, 1.03, 1.21);    // example calibration

    pb.mode(PullUp);
    // Delay for initial pullup to take effect
    wait(.001);
    pb.fall(&flipISR);

    wait_ms(2000); // 2 s for Logo to be displayed
    gOled2.clearDisplay();

    if (circBuffPush(&buffP3, &moveMotor)) {
        pc.printf("P3 buffer full\r\n");
    }

    while(1) {
        if(circBuffPop(&buffP1, &runningPtr) == 0) {
            runningPtr();
            continue;
        }

        if(circBuffPop(&buffP2, &runningPtr) == 0) {
            runningPtr();
            continue;
        }

        if(circBuffPop(&buffP3, &runningPtr) == 0) {
            runningPtr();
            continue;
        }
    }
}