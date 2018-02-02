#include "mbed.h"
#include "sMotor.h"
#include "ultrasonic.h"
#include "circBuffer.h"

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

void printDistance(void)
{
    if (sensorDistance < 500 && sensorDistance > 20) {
        pc.printf("%d %d\r\n",motorpositionstep*180/270, sensorDistance);
    }
}

void dist(int distance)
{
    sensorDistance = distance;
    if (circBuffPush(&buffP2, &printDistance)) {
        pc.printf("P2 buffer full\r\n");
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
        pc.printf("Paused\r\n");
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

void moveMotor()
{
    motor.step(1, motordirection, step_speed);
    wait(0.01);
    if (motordirection == 0) {
        motorpositionstep++;
        if (motorpositionstep == 270) {
            motordirection = 1;
        }
    } else if (motordirection == 1) {
        motorpositionstep--;
        if (motorpositionstep == 0) {
            motordirection = 0;
        }
    }

    if (circBuffPush(&buffP3, &moveMotor)) {
        pc.printf("P3 buffer full\r\n");
    }
}

int main()
{
    fooptr runningPtr;

    mu.startUpdates();//start measuring the distance

    measure.attach(&checkmeasure, 0.5);

    pb.mode(PullUp);
    // Delay for initial pullup to take effect
    wait(.001);
    pb.fall(&flipISR);

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