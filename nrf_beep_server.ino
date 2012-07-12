/* NRF24L01 server
 * Waits for pings from client and sends ack packets
 */

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

const int rfChannel = 35;             // 0 to 127 (0 to 84 in USA)
byte myAddr[]   = {"pingS"};          // address of this device
byte destAddr[] = {"pingC"};          // address of remote device
const int payloadSize = sizeof(byte); // 1 byte payload size

const int pinLedBlink = 11;
const int blinkTime = 100;
unsigned long blinkOffTime = 0;

// these must be in byte array form, even if they're just one byte
byte pingPacket[payloadSize]    = {12};
byte confirmPacket[payloadSize] = {84};

byte recvData[payloadSize] = {0}; // incoming data buffer

void setup() {
	Serial.begin(115200);

	pinMode(pinLedBlink, OUTPUT);

	Mirf.spi = &MirfHardwareSpi;
	Mirf.init();
	Mirf.setRADDR(myAddr);
	Mirf.setTADDR(destAddr);
	Mirf.payload = payloadSize;
	Mirf.channel = rfChannel;
	Mirf.configRegister(RF_CH, 15);
	Mirf.config();
	Serial.println("Server is ready to go!");

	Serial.print("My address:     ");
	for (int i = 0; i < 5; i++) {
		Serial.print(myAddr[i], HEX);
		Serial.print(' ');
	}
	Serial.println();
	Serial.print("Target address: ");
	for (int i = 0; i < 5; i++) {
		Serial.print(destAddr[i], HEX);
		Serial.print(' ');
	}
	Serial.println();
	byte tempRfCh[1] = {0};
	Mirf.readRegister(RF_CH, tempRfCh, 1);
	Serial.print("RF channel:     ");
	Serial.println(tempRfCh[0]);
}

void loop() {
	if (millis() >= blinkOffTime) {
		digitalWrite(pinLedBlink, LOW);
	}
	if (!Mirf.isSending() && Mirf.dataReady()) {
		Mirf.getData(recvData);
		Serial.print("Data received:     ");
		Serial.println(recvData[0]);
		if (recvData[0] = pingPacket[0]) {
			Mirf.send(confirmPacket);
			Serial.print("Ping acknowledged: ");
			Serial.println(confirmPacket[0]);
			blinkLed();
		}
	}
}

void blinkLed() {
	blinkOffTime = millis() + blinkTime;
	digitalWrite(pinLedBlink, HIGH);
}