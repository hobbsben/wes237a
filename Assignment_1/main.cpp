#include "mbed.h"
#include "ultrasonic.h"

Serial pc(USBTX, USBRX); // tx, rx
InterruptIn pb(p12);
DigitalOut flash(LED4);

void dist(int distance) //this needs to be here...... check why
{
    
}

ultrasonic mu(p8, p9, .1, 1, &dist);

void flip() {
    wait(0.001);
    if (pb.read()==0){
        long distance = mu.getCurrentDistance();
        printf("Distance: %d\r\n", distance);
        wait(0.1);
    }
}
 
int main() {
    pb.mode(PullUp);
    // Delay for initial pullup to take effect
    wait(.001);
    pb.fall(&flip);
    mu.startUpdates();//start measuring the distance
    while(1) {
        flash = !flash;
        wait(0.25);
    }
}