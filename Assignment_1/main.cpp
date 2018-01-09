#include "mbed.h"

Serial pc(USBTX, USBRX); // tx, rx
DigitalIn pb(p5);

// SPST Pushbutton demo using internal PullUp function
// no external PullUp resistor needed
// Pushbutton from P8 to GND.
int main() {
    pb.mode(PullUp);
    int current = pb.read();
    while(1) {
        if (pb.read()!= current){
            wait(0.001);
            if (pb.read()!=current){
                current=pb.read();
                if (current == 0){
                    pc.printf("Pressed\r\n");
                }
            }
        }
    }
}