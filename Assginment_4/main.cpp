#include "mbed.h"
#include "LSM303DLHC.h"
//RawSerial pc(USBTX, USBRX); // Define serial TX RX
LSM303DLHC acel(p9, p10);
RawSerial xbee(p13, p14);
DigitalIn pushbutton(p20); //Define pushbutton on pin 20
DigitalIn pb(p19);//pushbutton on 19
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);
int accel[3];
char temp;
char a;
void dev_recv()
{
char temp = 0;
while(xbee.readable()) {
temp = xbee.getc();
//printf("%c",temp);
if (temp=='1') led1=led2=led3=led4=1;
else if(temp=='0')led1=led2=led3=led4=0;
}
}
int main()
{
acel.init();
pushbutton.mode(PullUp);
pb.mode(PullUp);
wait(.001);
void acceler();
xbee.attach(&dev_recv, Serial::RxIrq);
while(1) {
if(pb.read()==0) {
wait(.001);
if(pb.read()==0) {
xbee.putc('s');
//wait(5);
}
} else if(pushbutton.read()==0) {
wait(.001);
if(pushbutton.read()==0) {
acceler();
xbee.putc(a);
}
} else {

xbee.putc('0');
}
wait(.1);
}
}
void acceler()
{
acel.readAcc(accel);
if(accel[0]>=-300 && accel[0]<=300) {
if(accel[1]>=-300 && accel[1]<=300) {
a='0';
} else if(accel[1]>=300 && accel[1]<=800) {
a='5';
} else if(accel[1]>800) {
a='6';
} else if(accel[1]>=-800 && accel[1]<=-300) {
a='7';
} else {
a='8';
}
} else if(accel[0]>=300 && accel[0]<=800) {
a='1';
} else if(accel[0]>800) {
a='2';
} else if(accel[0]>=-800 && accel[0]<=-300) {
a='3';
} else {
a='4';
}
}