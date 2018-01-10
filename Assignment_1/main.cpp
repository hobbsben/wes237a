#include "mbed.h"
#include "ultrasonic.h"

Serial pc(USBTX, USBRX);

void dist(int distance) //this needs to be here...... check why
{
    
}
 
ultrasonic mu(p8, p9, .1, 1, &dist);

int main() {
    mu.startUpdates();//start measuring the distance
    while(1) {
        if(pc.getc()!=NULL) {
            mu.checkDistance();
            long distance = mu.getCurrentDistance();
            printf("Distance: %d\r\n", distance);
            wait(0.1);
        }
    }
}