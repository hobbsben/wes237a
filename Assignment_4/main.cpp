#include "mbed.h"
#include "ultrasonic.h"

int temp=0;

RawSerial  pi(USBTX, USBRX);

void dist(int distance) //this needs to be here...... check why
{
   if(distance<=200) {
       if(temp!=1){
       temp=1;
       pi.putc('1');
       }
       //printf("1");
   } else {
       if(temp!=0){
       pi.putc('0');
       temp=0;
       }
       //printf("0");
   }
}
ultrasonic mu(p11, p12, .5, 1, &dist);

int main()
{
   pi.baud(9600);

   mu.startUpdates();//start measuring the distance
   while(1) {
       mu.checkDistance();
       long distance = mu.getCurrentDistance();
   }
}