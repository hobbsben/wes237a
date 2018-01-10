#include "mbed.h"
#include "ultrasonic.h"

Serial pc(USBTX, USBRX); // tx, rx
DigitalIn pb(p12);
Serial pc(USBTX, USBRX);

void dist(int distance) //this needs to be here...... check why
{
    
}
 
ultrasonic mu(p8, p9, .1, 1, &dist);

int main() {
    pb.mode(PullUp);
	mu.startUpdates();//start measuring the distance
    int current = pb.read();
    while(1) {
        if (pb.read()!= current){
            wait(0.001);
            if (pb.read()!=current){
                current=pb.read();
                if (current == 0){
                    mu.checkDistance();
					long distance = mu.getCurrentDistance();
					printf("Distance: %d\r\n", distance);
					wait(0.1);
                }
            }
		}
    }
}