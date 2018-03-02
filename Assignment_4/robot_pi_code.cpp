#include "create/create.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

create::Create* robot;

int main(int argc, char** argv) {
	std::string port = "/dev/ttyUSB0";
	int baud = 115200;
	create::RobotModel model = create::RobotModel::CREATE_2;

	if (argc > 1 && std::string(argv[1]) == "create1") {
		model = create::RobotModel::CREATE_1;
		baud = 57600;
		std::cout << "1st generation Create selected" << std::endl;
	}

	robot = new create::Create(model);

	// Attempt to connect to Create
	if (robot->connect(port, baud))
		std::cout << "Successfully connected to Create" << std::endl;
	else {
		std::cout << "Failed to connect to Create on port " << port.c_str() << std::endl;
		return 1;
	}

	// Note: Some functionality does not work as expected in Safe mode
	robot->setMode(create::MODE_FULL);

	std::cout << "battery level: " <<
		robot->getBatteryCharge() / (float)robot->getBatteryCapacity() * 100.0 << "%" << std::endl;

	bool drive = true;

	// Make a song
	uint8_t songLength = 16;
	uint8_t notes[16] = { 67, 67, 66, 66, 65, 65, 66, 66,
		67, 67, 66, 66, 65, 65, 66, 66 };
	float durations[songLength];
	for (int i = 0; i < songLength; i++) {
		durations[i] = 0.25;
	}
	robot->defineSong(0, songLength, notes, durations);
	//usleep(1000000);
	//robot->playSong(0);

	//string portString;
	//cout << "Enter MBED Port String: ";
	//getline(cin, portString);

	int fd = open("/dev/ttyAMA0", O_RDWR);
	if (fd == -1) {
		perror("/dev/ttyAMA0");
		return 1;
	}

	struct termios tios;
	tcgetattr(fd, &tios);
	// disable flow control and all that, and ignore break and parity errors
	tios.c_iflag = IGNBRK | IGNPAR;
	tios.c_oflag = 0;
	tios.c_lflag = 0;
	cfsetspeed(&tios, B9600);
	tcsetattr(fd, TCSAFLUSH, &tios);

	int isMeasured = 0;
	int isDistanceSensor = 0;
	char buf[10];
	buf[1] = '\r'; // end of line
	buf[2] = 0; // end of string

	char bufusb[10];
	bufusb[1] = '\r'; // end of line
	bufusb[2] = 0; // end of string

	std::string portString;
	std::cout << "Enter MBED Port String: ";
	getline(std::cin, portString);

	// Open the Port. We want read/write, no "controlling tty" status, 
	// and open it no matter what state DCD is in
	int fdusb = open(portString.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

	//open mbed's USB virtual com port
	if (fdusb == -1) {
		perror("open_port: Unable to open port - ");
		return(-1);
	}

	// Turn off blocking for reads, use (fd, F_SETFL, FNDELAY) if you want that
	fcntl(fdusb, F_SETFL, FNDELAY);
	//Linux Raw serial setup options
	struct termios options;
	tcgetattr(fdusb, &options);   //Get current serial settings in structure
	cfsetspeed(&options, B9600);   //Change a only few
	options.c_cflag &= ~CSTOPB;
	options.c_cflag |= CLOCAL;
	options.c_cflag |= CREAD;
	cfmakeraw(&options);
	tcsetattr(fdusb, TCSANOW, &options);    //Set serial to new settings
	sleep(1);

	// Quit when center "Clean" button pressed
	while (!robot->isCleanButtonPressed()) {
		// If everything is ok, drive forward using IR's to avoid obstacles
		if (drive) {
			robot->setPowerLED(0); // green
			isMeasured = read(fd, buf, 1); //read echo character
			isDistanceSensor = read(fdusb, bufusb, 1);
			//std::cout << bufusb[0] << std::endl;
			//std::cout << buf[0] << std::endl;
			//std::cout << isMeasured << std::endl;
			//printf("%s\n\r", buf);   //print in terminal window
			if (bufusb[0] == '1'){
				write(fd, "1", 1);
			}
			else if(bufusb[0] == '0'){
				write(fd, "0", 1);
			}

			if (isMeasured == 1){
				if (buf[0] == '1' && bufusb[0] == '0'){
					// drive straight
					robot->drive(0.1, 0.0);
					robot->setDigitsASCII(' ', '^', '^', ' ');
				}
				else if (buf[0] == '2'){
					// drive straight fast
					robot->drive(1, 0.0);
					robot->setDigitsASCII(' ', '^', '^', ' ');
				}
				else if (buf[0] == '3'){
					// drive back
					robot->drive(-0.1, 0.0);
					robot->setDigitsASCII(' ', '.', '.', ' ');
				}
				else if (buf[0] == '4'){
					// drive back fast
					robot->drive(-1, 0.0);
					robot->setDigitsASCII(' ', '.', '.', ' ');
				}
				else if (buf[0] == '7'){
					// turn right
					robot->drive(0.0, -0.1);
					robot->setDigitsASCII('-', '-', '-', ']');
				}
				else if (buf[0] == '8'){
					// turn right fast
					robot->drive(0.0, -1);
					robot->setDigitsASCII('-', '-', '-', ']');
				}
				else if (buf[0] == '5'){
					// turn left
					robot->drive(0.0, 0.1);
					robot->setDigitsASCII('[', '-', '-', '-');
				}
				else if (buf[0] == '6'){
					// turn left fast
					robot->drive(0.0, 1.0);
					robot->setDigitsASCII('[', '-', '-', '-');
				}
				else if (buf[0] == 's'){
					// playsong
					robot->playSong(0);
				}
				else {
					// stop
					robot->drive(0.0, 0.0);
				}
			}
			isMeasured = 0;
		}
		usleep(1000 * 100); //10hz
	}

	std::cout << "Stopping Create." << std::endl;

	// Turn off lights
	robot->setPowerLED(0, 0);
	robot->enableDebrisLED(false);
	robot->enableCheckRobotLED(false);
	robot->setDigitsASCII(' ', ' ', ' ', ' ');

	// Make sure to disconnect to clean up
	robot->disconnect();
	delete robot;

	tcdrain(fd);
	close(fd);

	tcdrain(fdusb);
	close(fdusb);

	return 0;
}