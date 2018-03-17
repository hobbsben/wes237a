#include "mbed.h"
#include "ultrasonic.h"
//DigitalInOut QTR1(p20);
DigitalInOut QTR1(p19);
DigitalInOut QTR2(p18);
DigitalInOut QTR3(p17);
DigitalInOut QTR4(p16);
DigitalInOut QTR5(p15);
DigitalInOut QTR6(p14);
DigitalInOut QTR7(p13);
DigitalInOut QTR8(p12);

DigitalInOut QTR9(p28);
DigitalInOut QTR10(p27);
DigitalInOut QTR11(p26);
DigitalInOut QTR12(p25);
DigitalInOut QTR13(p24);
DigitalInOut QTR14(p23);
DigitalInOut QTR15(p22);
DigitalInOut QTR16(p21);

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);

char alpha;

Timer t;
Ticker packagetimeout;
int SONAR_F = 0;

void dist(int distance)
{
	if (distance < 75) {
		SONAR_F = 1;
		packagetimeout.detach();
		alpha = '1';
	}
}

void TimerInt()
{
	SONAR_F = 1;
	packagetimeout.detach();
	alpha = '1';
}


ultrasonic mu(p9, p8, .1, 1, &dist);
RawSerial pi(USBTX, USBRX); // tx, rx

int saw1 = 0, saw2 = 0, saw3 = 0, saw4 = 0, saw5 = 0, saw6 = 0, saw7 = 0, saw8 = 0,
saw9 = 0, saw10 = 0, saw11 = 0, saw12 = 0, saw13 = 0, saw14 = 0, saw15 = 0, saw16 = 0;
//functions
int lineRead(DigitalInOut QTRx);
void readAllSensors();
int TURNAROUND = 0;
int ENDCHECK = 0;
int SKIPTURN = 0;
int waitcounter = 55;



int main()
{
	mu.startUpdates();
	pi.baud(9600);
	while (1) {
		readAllSensors();

		if (TURNAROUND == 1) { //end nodes
			if (SONAR_F == 0) {
				mu.checkDistance();
				alpha = 's';
			}
			else {
				waitcounter--;
				if (waitcounter == 45) {
					alpha = '2';
				}
				else if (waitcounter == 0) {
					waitcounter = 55;
					TURNAROUND = 0;
					led1 = 0;
					led2 = 0;
					led3 = 0;
					SONAR_F = 0;
				}
			}
		}
		else if (SKIPTURN == 1) {
			waitcounter--;
			if (waitcounter == 45) {
				alpha = '2';
			}
			else if (waitcounter == 0) {
				waitcounter = 55;
				SKIPTURN = 0;
				led1 = 0;
				led2 = 0;
				led3 = 0;
				led4 = 0;
			}
		}
		else {
			//sees a T (node)
			if (saw3 >= 2000 && saw4 >= 2000 && saw5 >= 2000 && saw6 >= 2000 && saw7 >= 2000
				&& saw8 >= 2000 && saw9 >= 2000 && saw10 >= 2000 && saw11 >= 2000 && saw12 >= 2000
				&& saw13 >= 2000 && saw14 >= 2000) {
				if (ENDCHECK == 1) {//BlackBox
					led1 = 1;
					TURNAROUND = 1;
					ENDCHECK = 0;
					alpha = '0';
					packagetimeout.attach(&TimerInt, 8.0);
				}
				else { // move forward one but check for black box next
					ENDCHECK = 1;
					led2 = 1;
					alpha = '1'; //foward
				}
			}

			else if (saw1 >= 2000 && saw2 >= 2000 && saw3 >= 2000 && saw4 >= 2000 && saw5 >= 2000 && saw6 >= 2000 && saw7 >= 2000 &&
				saw8 >= 2000 && saw9 >= 2000 && saw11 <= 2000 && saw12 <= 2000 && saw13 <= 2000 && saw14 <= 2000 && saw15 <= 2000 && saw16 <= 2000)  { //lefty
				TURNAROUND = 1;
				SONAR_F = 1;
				alpha = '1';
				led4 = 1;
			}

			//1 *straight*
			else if (saw8 >= 2000 || saw9 >= 2000 || saw10 >= 2000) {
				if (ENDCHECK == 1) {//Big turn
					SKIPTURN = 1;
					ENDCHECK = 0;
					alpha = '1';
					led3 = 1;
				}
				else {
					alpha = '1';
				}
			}
			//2*left*
			else if (saw1 >= 2000 && saw2 >= 2000 || saw2 >= 2000 && saw3 >= 2000 || saw3 >= 2000 && saw4 >= 2000 ||
				saw4 >= 2000 && saw5 >= 2000 || saw5 >= 2000 && saw6 >= 2000 || saw6 >= 2000 && saw7 >= 2000) {
				alpha = '2';

			}
			//3 *right*
			else if (saw11 >= 2000 && saw12 >= 2000 || saw12 >= 2000 && saw13 >= 2000 || saw13 >= 2000 && saw14 >= 2000 ||
				saw14 >= 2000 && saw15 >= 2000 || saw15 >= 2000 && saw16 >= 2000) {
				alpha = '3';
			}
			// turn left if sensors not reading anything
			else alpha = alpha;
		}

		pi.putc(alpha);
		wait(0.04);
	}
}

int lineRead(DigitalInOut QTRx)
{
	int sensorRead = 0;
	QTRx.output();
	QTRx.mode(PullUp);
	wait_us(2500);
	QTRx.mode(PullNone);
	QTRx = 1;
	t.start();
	QTRx.input();
	while (QTRx == 1);
	sensorRead = t.read_us();
	t.stop();
	t.reset();

	return sensorRead;

}

void readAllSensors()
{
	saw1 = lineRead(QTR1);
	saw2 = lineRead(QTR2);
	saw3 = lineRead(QTR3);
	saw4 = lineRead(QTR4);
	saw5 = lineRead(QTR5);
	saw6 = lineRead(QTR6);
	saw7 = lineRead(QTR7);
	saw8 = lineRead(QTR8);
	saw9 = lineRead(QTR9);
	saw10 = lineRead(QTR10);
	saw11 = lineRead(QTR11);
	saw12 = lineRead(QTR12);
	saw13 = lineRead(QTR13);
	saw14 = lineRead(QTR14);
	saw15 = lineRead(QTR15);
	saw16 = lineRead(QTR16);
}