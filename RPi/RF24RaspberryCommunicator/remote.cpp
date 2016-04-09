#include <string>
#include <getopt.h>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <RF24/RF24.h>
#include <unistd.h>

using namespace std;
//RF24 radio("/dev/spidev0.0",8000000 , 25);  
//RF24 radio(RPI_V2_GPIO_P1_15, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);

RF24 radio(RPI_V2_GPIO_P1_22, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);
//const int role_pin = 7;
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
//const uint8_t pipes[][6] = {"1Node","2Node"};

// hack to avoid SEG FAULT, issue #46 on RF24 github https://github.com/TMRh20/RF24.git
char  got_message[100];

void setup(void){
	//Prepare the radio module
	printf("\nPreparing interface\n");
	radio.begin();
	radio.setRetries( 15, 15);
	//	radio.setChannel(0x4c);
	//	radio.setPALevel(RF24_PA_MAX);
	//	radio.setPALevel(RF24_PA_MAX);

	radio.printDetails();
	radio.openWritingPipe(pipes[0]);
	radio.openReadingPipe(1,pipes[1]);
	radio.startListening();

}

void receiveMessage() {
	unsigned long started_waiting_at = millis();
	bool timeout = false;
	while (!radio.available() && !timeout) {
		if (millis() - started_waiting_at > 5000) {
			timeout = true;
		}
	}

	if (timeout) {
		printf("TIMEOUT\n\r");
	} else {
		radio.read(&got_message, sizeof("hello"));
		printf("Got message: %s\n\r",got_message);
		system("curl -H 'Content-Type: application/json' -X POST -d '{\"message\":\"from_rasp\"}' http://54.165.195.244:3000");
	}
}

int main( int argc, char ** argv){
	setup();
	while(true){
		receiveMessage();
		// sleep(1);
	}
	return 0;
}
