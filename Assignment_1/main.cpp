#include "mbed.h"
#include "sMotor.h"

InterruptIn pb(p13);

Serial pc(USBTX, USBRX);
sMotor motor(p14, p15, p16, p17); // creates new stepper motor: IN1, IN2, IN3, IN4

Ticker measure;

int step_speed = 2000 ; // set default motor speed
int numstep = 512 ; // defines full turn of 360 degree
//you might want to calibrate this value according to your motor

void checkmeasure() {
    measuredist = 1;
}

void flip() {
    wait(0.001);
    if (pb.read()==0){
        if (pause == false){
            measure.detach();
        }
        if (pause == true){
            measure.attach(&checkmeasure, 0.5);
        }
        pause = !pause;
    }
}

int main()
{
    measure.attach(&checkmeasure, 0.5);
    pb.mode(PullUp);
    // Delay for initial pullup to take effect
    wait(.001);
    pb.fall(&flip);
    while (1) {
        motor.step(numstep/2,0,step_speed);
        motor.step(numstep/2,1,step_speed);
    }
}