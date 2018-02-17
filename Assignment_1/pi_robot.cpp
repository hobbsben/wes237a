#include "create/create.h"
#include <iostream>
#include <math.h>       /* atan */
#include <stdlib.h>     /* abs */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

using namespace std;

bool WANDERMODETIMER = true;

//#define SCALING 0.025;
//#define ROTATEUSECSCALE 46778;
//qualcomm = 46778

create::Create* robot;

float currentx = 0;
float currenty = 0;
float currentangle = 0;
char radarbuffer[1000];
//int currentCharCursor = 0;
int radarCalcCoordx[500] = { -1 };
int radarCalcCoordy[500] = { -1 };
int numberofobjects = 0;

int fd;

void createOcpMap(){
	int radarDistances[100] = { -1 };
	int radarDegree[100] = { -1 };

	string str(radarbuffer);
	radarbuffer[0] = '\0';

	int previousCursor = 0;
	int objectCount = 0;
	for (int count = 0; count < str.length(); count++){
		if (str.at(count) == ','){
			radarDistances[objectCount] = std::stoi(str.substr(previousCursor, count - 1));
			radarDistances[objectCount] = radarDistances[objectCount] * 0.0393701;
			previousCursor = count + 1;
			//cout << radarDistances[objectCount] << "d,";
		}
		else if (str.at(count) == '?'){
			radarDegree[objectCount] = std::stoi(str.substr(previousCursor, count - 1));
			previousCursor = count + 1;
			//cout << radarDegree[objectCount] << "g,";
			objectCount++;
		}
	}

	int ocpX;
	int ocpY;
	char ocpbuff[4] = { '\0' };

	for (int count = 0; count < numberofobjects; count++){
		ocpX = radarCalcCoordx[count] + (radarDistances[count] * cos(radarDegree[count] / 180 * create::util::PI));
		cout << radarCalcCoordx[count] << "x,";
		ocpY = radarCalcCoordy[count] + (radarDistances[count] * sin(radarDegree[count] / 180 * create::util::PI));
		cout << radarCalcCoordy[count] << "y,";
		if (ocpX<0 || ocpX>128 || ocpY<0 || ocpY>64){
			continue;
		}
		cout << ocpX << std::endl;
		cout << ocpY << std::endl << std::endl;
		sprintf(ocpbuff, "%d", ocpX);
		for (int i = 0; i < 4; i++){
			if (ocpbuff[i] != '\0'){
				//cout << ocpbuff[i] << std::endl;
				if (ocpbuff[i] == '1'){
					write(fd, "1", 1);
				}
				else if (ocpbuff[i] == '2'){
					write(fd, "2", 1);
				}
				else if (ocpbuff[i] == '3'){
					write(fd, "3", 1);
				}
				else if (ocpbuff[i] == '4'){
					write(fd, "4", 1);
				}
				else if (ocpbuff[i] == '5'){
					write(fd, "5", 1);
				}
				else if (ocpbuff[i] == '6'){
					write(fd, "6", 1);
				}
				else if (ocpbuff[i] == '7'){
					write(fd, "7", 1);
				}
				else if (ocpbuff[i] == '8'){
					write(fd, "8", 1);
				}
				else if (ocpbuff[i] == '9'){
					write(fd, "9", 1);
				}
				else{
					write(fd, "0", 1);
				}
				ocpbuff[i] = '\0';
				sleep(1);
			}
		}
		write(fd, ",", 1);
		//cout << "," << std::endl;
		sleep(1);
		sprintf(ocpbuff, "%d", ocpY);
		for (int i = 0; i < 4; i++){
			if (ocpbuff[i] != '\0'){
				//cout << ocpbuff[i] << std::endl;
				if (ocpbuff[i] == '1'){
					write(fd, "1", 1);
				}
				else if (ocpbuff[i] == '2'){
					write(fd, "2", 1);
				}
				else if (ocpbuff[i] == '3'){
					write(fd, "3", 1);
				}
				else if (ocpbuff[i] == '4'){
					write(fd, "4", 1);
				}
				else if (ocpbuff[i] == '5'){
					write(fd, "5", 1);
				}
				else if (ocpbuff[i] == '6'){
					write(fd, "6", 1);
				}
				else if (ocpbuff[i] == '7'){
					write(fd, "7", 1);
				}
				else if (ocpbuff[i] == '8'){
					write(fd, "8", 1);
				}
				else if (ocpbuff[i] == '9'){
					write(fd, "9", 1);
				}
				else{
					write(fd, "0", 1);
				}
				ocpbuff[i] = '\0';
				sleep(1);
			}
		}
		write(fd, "?", 1);
		//cout << "?" << std::endl;
		sleep(1);
	}

	write(fd, "y", 1);

	for (int i = 0; i < 500; i++) {
		if (radarCalcCoordx[i] != -1) {
			radarCalcCoordx[i] = -1;
			radarCalcCoordy[i] = -1;
		}
	}
	numberofobjects = 0;
}

void getPose(){
	create::Pose pose = robot->getPose();

	//currentangle = -pose.yaw * 180.0 / create::util::PI;
	//currentx = -pose.y;
	//currenty = pose.x;

	// Print pose
	std::cout << "x: " << currentx
		<< "\ty: " << currenty
		<< "\tyaw: " << currentangle << std::endl << std::endl;
}

void drivetoCoordinate(float distance, float orientation){
	//rotate
	//while (abs(orientation - currentangle) > 2) {
	//	robot->drive(0.0, -0.1);
	//	usleep(1000 * 100); //10hz
	//	getPose();
	//}
	char temp[8];
	int isMeasured = false;

	if (orientation != currentangle){
		robot->drive(0.0, -0.1);
		int cycles = 0;
		if (orientation > currentangle){
			cycles = orientation - currentangle;
			//usleep(70778 * (orientation - currentangle));
		}
		else{
			cycles = (360 - currentangle) + orientation;
			//usleep(70778 * ((360 - currentangle) + orientation));
		}

		for (int i = 0; i < cycles; i++){
			usleep(72778);
			//getPose();
			isMeasured = read(fd, temp, sizeof(temp));  // read a char at a time
			if (isMeasured == 8){
				strcat(radarbuffer, temp);
				radarCalcCoordx[numberofobjects] = currentx;
				radarCalcCoordy[numberofobjects] = currenty;
				temp[0] = '\0';
				numberofobjects++;
				isMeasured = 0;
			}
		}

		robot->drive(0, 0);
		currentangle = orientation;
	}

	//move
	//if (scaledx > currentx && scaledy > currenty){
	//	while (scaledx > currentx || scaledy > currenty) {
	//		//if (orientation - currentangle < -1){
	//		//	robot->drive(0.1, 0.35);
	//		//	cout << "Adjust left";
	//		//}
	//		//else if (orientation - currentangle > 1){
	//		//	robot->drive(0.1, -0.1);
	//		//	cout << "Adjust right";
	//		//}
	//		//else{
	//		//	robot->drive(0.1, 0.0);
	//		//	cout << "Straight";
	//		//}
	//		robot->drive(0.1, 0.0);
	//		cout << "Straight";
	//		usleep(210909); //10hz
	//		getPose();
	//	}
	//	robot->drive(0, 0);
	//	return;
	//}
	//else if (scaledx > currentx && scaledy < currenty){
	//	while (scaledx > currentx || scaledy < currenty) {
	//		//if (orientation - currentangle < -1){
	//		//	robot->drive(0.1, 0.35);
	//		//	cout << "Adjust left";
	//		//}
	//		//else if (orientation - currentangle > 1){
	//		//	robot->drive(0.1, -0.1);
	//		//	cout << "Adjust right";
	//		//}
	//		//else{
	//		//	robot->drive(0.1, 0.0);
	//		//	cout << "Straight";
	//		//}
	//		robot->drive(0.1, 0.0);
	//		cout << "Straight";
	//		usleep(210909); //10hz
	//		robot->drive(0, 0);
	//		getPose();
	//	}
	//	return;
	//}
	//else if (scaledx < currentx && scaledy > currenty){
	//	while (scaledx < currentx || scaledy > currenty) {
	//		//if (orientation - currentangle < -1){
	//		//	robot->drive(0.1, 0.35);
	//		//	cout << "Adjust left";
	//		//}
	//		//else if (orientation - currentangle > 1){
	//		//	robot->drive(0.1, -0.1);
	//		//	cout << "Adjust right";
	//		//}
	//		//else{
	//		//	robot->drive(0.1, 0.0);
	//		//	cout << "Straight";
	//		//}
	//		robot->drive(0.1, 0.0);
	//		cout << "Straight";
	//		usleep(210909); //10hz
	//		getPose();
	//	}
	//	robot->drive(0, 0);
	//	return;
	//}
	//else {
	//	while (scaledx < currentx || scaledy < currenty) {
	//		//if (orientation - currentangle < -1){
	//		//	robot->drive(0.1, 0.35);
	//		//	cout << "Adjust left";
	//		//}
	//		//else if (orientation - currentangle > 1){
	//		//	robot->drive(0.1, -0.1);
	//		//	cout << "Adjust right";
	//		//}
	//		//else{
	//		//	robot->drive(0.1, 0.0);
	//		//	cout << "Straight";
	//		//}
	//		robot->drive(0.1, 0.0);
	//		cout << "Straight";
	//		usleep(210909); //10hz
	//		robot->drive(0, 0);
	//		getPose();
	//	}
	//	robot->drive(0, 0);
	//	return;
	//}

	for (int i = 0; i < distance; i++) {
		if (i % 10 == 0 && i != 0){
			robot->drive(0.1, 0.1);
			usleep(210909);
			robot->drive(0.0, 0.0);
			currentx = currentx + cos(currentangle / 180 * create::util::PI);
			currenty = currenty + sin(currentangle / 180 * create::util::PI);

			isMeasured = read(fd, temp, sizeof(temp));  // read a char at a time
			if (isMeasured == 8){
				strcat(radarbuffer, temp);
				radarCalcCoordx[numberofobjects] = currentx;
				radarCalcCoordy[numberofobjects] = currenty;
				temp[0] = '\0';
				numberofobjects++;
				isMeasured = 0;
			}
		}
		else{
			robot->drive(0.1, 0.0);
			usleep(210909);
			robot->drive(0.0, 0.0);
			currentx = currentx + cos(currentangle / 180 * create::util::PI);
			currenty = currenty + sin(currentangle / 180 * create::util::PI);

			isMeasured = read(fd, temp, sizeof(temp));  // read a char at a time
			if (isMeasured == 8){
				strcat(radarbuffer, temp);
				radarCalcCoordx[numberofobjects] = currentx;
				radarCalcCoordy[numberofobjects] = currenty;
				temp[0] = '\0';
				numberofobjects++;
				isMeasured = 0;
			}
		}
		//currentx = scaledx;
		//currenty = scaledy;
	}
	robot->drive(0, 0);
}


//DONT USE
void resetDegree(){
	while (abs(0 - currentangle) > 2) {
		robot->drive(0.0, -0.1);
		usleep(1000 * 100); //10hz
		getPose();
	}
	robot->drive(0, 0);
}

void setupdrivetoCoordinate(int scaledx, int scaledy){
	//float scaledx = unscaledx*SCALING;
	//float scaledy = unscaledy*SCALING;

	float degree;
	float distance;

	//getPose();

	if (scaledx > currentx && scaledy > currenty){
		//degree = atan(abs(scaledx - currentx) / abs(scaledy - currenty)) * 180 / create::util::PI;
		degree = atan(abs(scaledx - currentx) / abs(scaledy - currenty)) * 180 / create::util::PI;

		distance = sqrt(pow(abs(scaledx - currentx), 2.0) + pow(abs(scaledy - currenty), 2.0));

		cout << "1";
	}
	else if (scaledx > currentx && scaledy < currenty){
		//degree = atan(abs(scaledy - currenty) / abs(scaledx - currentx)) * 180 / create::util::PI;
		//degree = atan(abs(currenty - scaledy) / abs(scaledx - currentx)) * 180 / create::util::PI;
		//degree = 90 + degree;
		degree = atan(abs(scaledx - currentx) / abs(currenty - scaledy)) * 180 / create::util::PI;
		degree = 180 - degree;

		distance = sqrt(pow(abs(currenty - scaledy), 2.0) + pow(abs(scaledx - currentx), 2.0));

		cout << "2";
	}
	else if (scaledx < currentx && scaledy > currenty){
		//degree = atan(abs(scaledx - currentx) / abs(scaledy - currenty)) * 180 / create::util::PI;
		degree = atan(abs(currentx - scaledx) / abs(scaledy - currenty)) * 180 / create::util::PI;
		//degree = -degree;
		degree = 360 - degree;

		distance = sqrt(pow(abs(currentx - scaledx), 2.0) + pow(abs(scaledy - currenty), 2.0));

		cout << "3";
	}
	else if (scaledx == currentx){
		if (scaledy < currenty){
			degree = 180;
			distance = currenty - scaledy;
			cout << "4";
		}
		else{
			degree = 0;
			distance = scaledy - currenty;
			cout << "5";
		}
	}
	else if (scaledy == currenty){
		if (scaledx < currentx){
			degree = 270;
			distance = currentx - scaledx;
			cout << "6";
		}
		else{
			degree = 90;
			distance = scaledx - currentx;
			cout << "7";
		}
	}
	else {
		//degree = atan(abs(currentx - scaledx) / abs(currenty - scaledy)) * 180 / create::util::PI;
		degree = atan(abs(currentx - scaledx) / abs(currenty - scaledy)) * 180 / create::util::PI;
		//degree = -(180 - degree);
		degree = 180 + degree;

		distance = sqrt(pow(abs(currentx - scaledx), 2.0) + pow(abs(currenty - scaledy), 2.0));
		cout << "8";
	}



	cout << "Degree:" << degree << std::endl;
	cout << "Scaledx:" << scaledx << std::endl;
	cout << "Scaledy:" << scaledy << std::endl;
	cout << "Currentx:" << currentx << std::endl;
	cout << "Currenty:" << currenty << std::endl;
	cout << "Distance:" << distance << std::endl;

	drivetoCoordinate(distance, degree);

	currentx = scaledx;
	currenty = scaledy;

	//resetDegree();
}

void modeSingleCoord(){
	write(fd, "l", 1);
	string StringtoPrint;
	string temp;
	sleep(1);

	int unscaledx;
	int unscaledy;
	cout << "Enter x coordinate: ";
	cin >> unscaledx;
	temp = static_cast<ostringstream*>(&(ostringstream() << unscaledx))->str();
	StringtoPrint = StringtoPrint + " " + temp;

	cout << "Enter y coordinate: ";
	cin >> unscaledy;
	temp = static_cast<ostringstream*>(&(ostringstream() << unscaledy))->str();
	StringtoPrint = StringtoPrint + " " + temp;
	cout << "The value you entered is x:" << unscaledx;
	cout << " y:" << unscaledy << ".\n";

	for (unsigned int i = 0; i < StringtoPrint.length(); i++) {
		char c = StringtoPrint[i]; //this is your character
		write(fd, &c, 1);
		sleep(0.5);
	}

	sleep(8);
	write(fd, "x", 1);

	setupdrivetoCoordinate(unscaledx, unscaledy);
	robot->playSong(0);
	sleep(8);
	setupdrivetoCoordinate(0, 0);

	cout << radarbuffer << std::endl;
	write(fd, "o", 1);

	createOcpMap();
}

void modeCoords(){
	write(fd, "r", 1);
	string StringtoPrint;
	string temp;
	sleep(1);

	int numofCoords;
	cout << "Enter number of coordinates needed in route: ";
	cin >> numofCoords;
	int unscaledx[numofCoords];
	int unscaledy[numofCoords];
	for (int i = 0; i < numofCoords; i++) {
		cout << "Enter x coordinate for set " << i + 1 << ": ";
		cin >> unscaledx[i];
		temp = static_cast<ostringstream*>(&(ostringstream() << unscaledx[i]))->str();
		StringtoPrint = StringtoPrint + " " + temp;

		cout << "Enter y coordinate for set " << i + 1 << ": ";
		cin >> unscaledy[i];
		temp = static_cast<ostringstream*>(&(ostringstream() << unscaledy[i]))->str();
		StringtoPrint = StringtoPrint + " " + temp + ";";
		cout << "The value you entered is x:" << unscaledx[i];
		cout << " y:" << unscaledy[i] << ".\n";
	}

	for (unsigned int i = 0; i < StringtoPrint.length(); i++) {
		char c = StringtoPrint[i]; //this is your character
		write(fd, &c, 1);
		sleep(1);
	}

	write(fd, "x", 1);

	for (int i = 0; i < numofCoords; i++) {
		setupdrivetoCoordinate(unscaledx[i], unscaledy[i]);
		sleep(2);
	}

	setupdrivetoCoordinate(0, 0);

	cout << radarbuffer << std::endl;
	write(fd, "o", 1);

	createOcpMap();
}

void sigalrm_handler(int sig)
{
	WANDERMODETIMER = false;
}

void modeWander(){
	write(fd, "e", 1);
	string StringtoPrint;
	sleep(1);

	signal(SIGALRM, &sigalrm_handler);  // set a signal handler
	WANDERMODETIMER == true;

	bool contact_bumpers[2];

	int secondsToRoam;
	cout << "Enter number of seconds to roam: ";
	cin >> secondsToRoam;

	StringtoPrint = static_cast<ostringstream*>(&(ostringstream() << secondsToRoam))->str();

	for (unsigned int i = 0; i < StringtoPrint.length(); i++) {
		char c = StringtoPrint[i]; //this is your character
		write(fd, &c, 1);
		sleep(0.5);
	}

	sleep(8);
	write(fd, "x", 1);

	alarm(secondsToRoam);  // set an alarm for 10 seconds from now

	while (WANDERMODETIMER){
		contact_bumpers[0] = robot->isLeftBumper();
		contact_bumpers[1] = robot->isRightBumper();
		if (contact_bumpers[0] && contact_bumpers[1]){
			if (currentangle == 0){
				setupdrivetoCoordinate(currentx + 1, currenty);
			}
			else if (currentangle == 90){
				setupdrivetoCoordinate(currentx, currenty - 1);
			}
			else if (currentangle == 180){
				setupdrivetoCoordinate(currentx - 1, currenty);
			}
			else{
				setupdrivetoCoordinate(currentx, currenty + 1);
			}
		}
		else{
			if (currentangle == 0){
				if (currenty != 64){
					setupdrivetoCoordinate(currentx, currenty + 1);
				}
				else {
					setupdrivetoCoordinate(currentx + 1, currenty);
				}
			}
			else if (currentangle == 90){
				if (currentx != 128){
					setupdrivetoCoordinate(currentx + 1, currenty);
				}
				else {
					setupdrivetoCoordinate(currentx, currenty - 1);
				}
			}
			else if (currentangle == 180){
				if (currenty != 0){
					setupdrivetoCoordinate(currentx, currenty - 1);
				}
				else {
					setupdrivetoCoordinate(currentx - 1, currenty);
				}
			}
			else{
				if (currentx != 0){
					setupdrivetoCoordinate(currentx - 1, currenty);
				}
				else {
					setupdrivetoCoordinate(currentx, currenty + 1);
				}
			}
		}
	}

	robot->playSong(0);
	sleep(8);
	setupdrivetoCoordinate(0, 0);

	cout << radarbuffer << std::endl;
	write(fd, "o", 1);

	createOcpMap();
}

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

	bool EXITFLAG = false;

	// Make a song
	uint8_t songLength = 16;
	uint8_t notes[16] = { 67, 67, 66, 66, 65, 65, 66, 66,
		67, 67, 66, 66, 65, 65, 66, 66 };
	float durations[songLength];
	for (int i = 0; i < songLength; i++) {
		durations[i] = 0.25;
	}
	robot->defineSong(0, songLength, notes, durations);

	string portString;
	cout << "Enter MBED Port String: ";
	getline(cin, portString);

	// Open the Port. We want read/write, no "controlling tty" status, 
	// and open it no matter what state DCD is in
	fd = open(portString.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

	//open mbed's USB virtual com port
	if (fd == -1) {
		perror("open_port: Unable to open port - ");
		return(-1);
	}

	// Turn off blocking for reads, use (fd, F_SETFL, FNDELAY) if you want that
	fcntl(fd, F_SETFL, FNDELAY);
	//Linux Raw serial setup options
	struct termios options;
	tcgetattr(fd, &options);   //Get current serial settings in structure
	cfsetspeed(&options, B9600);   //Change a only few
	options.c_cflag &= ~CSTOPB;
	options.c_cflag |= CLOCAL;
	options.c_cflag |= CREAD;
	cfmakeraw(&options);
	tcsetattr(fd, TCSANOW, &options);    //Set serial to new settings
	sleep(1);

	while (EXITFLAG == false) {
		if (drive) {
			robot->setPowerLED(0); // green
			int mode;
			cout << "Enter mode to run: ";
			cin >> mode;

			if (mode == 1){
				modeSingleCoord();
			}
			else if (mode == 2){
				modeCoords();
			}
			else if (mode == 3){
				modeWander();
			}
			else if (mode == 0){
				EXITFLAG = true;
			}
			else{
				cout << "Not a valid mode";
			}
		}
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

	return 0;
}