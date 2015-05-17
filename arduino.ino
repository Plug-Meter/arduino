#include <SoftwareSerial.h>
#include "CurrentSensor.h"
#include "Meter.h"
#include "ESP8266.h"

#define BUFFER_SIZE 128

const int pin_sensor = 1; // Pino analógico que o sensor está conectado
const int pin_relay = 4; // Pino do relé
const int pin_lcd = 10; // Pino do display lcd (conectado à porta RX do display)
const int pin_esp8266_rx = 2; // Pino do ESP8266 (conectado à porta TX do ESP)
const int pin_esp8266_tx = 3; // Pino do ESP8266 (conectado à porta RX do ESP)
const double rede = 127.0; // Tensao da rede elétrica em Volts
const double sensibilidade = 66.0; // Parâmetro de sensibilidade do sensor (66mV/A)
const double preco_kwh = 0.71; // Preço de 1 kW.h da Light

HardwareSerial& dbgTerminal = Serial; // Serial usado para debug
SoftwareSerial espSerial(pin_esp8266_rx, pin_esp8266_tx); // Serial usado para o ESP8266
ESP8266 wifi(espSerial);
SoftwareSerial lcdTerminal(0, pin_lcd); // Serial usado para o display lcd
int relay_state = LOW;

char buffer[BUFFER_SIZE];
double potencia_total_segundos = 0.0;
unsigned long millis_ultima_medicao = 0;

CurrentSensor sensor(pin_sensor, sensibilidade);
Meter meter(rede, preco_kwh);

void setup()
{
	pinMode(pin_relay, OUTPUT);

	dbgTerminal.begin(9600);

	lcdTerminal.begin(9600);
	lcdTerminal.write(22); // Start LCD with no cursor and no blink
	lcdTerminal.print("Iniciando.");
	lcdTerminal.write(13); // New line
	lcdTerminal.print("Aguarde...");

	setupWiFi();

	dbgTerminal.println("Calibrando sensor...");
	sensor.determineVQ(); // Read initial measurements for calibration
	dbgTerminal.println("Sensor calibrado.");
}

void loop()
{
	// Ver se 
	unsigned long millis_atual = millis();
	unsigned long millis_intervalo_ultima_medicao = millis_atual - millis_ultima_medicao;
	float corrente = sensor.readCurrent();
	meter.setCorrente(corrente);

	dbgTerminal.print("Corrente atual: ");
	dbgTerminal.print(corrente);
	dbgTerminal.println("A");

	atualizaDisplay();

	millis_ultima_medicao = millis_atual;
	dbgTerminal.print("intervalo desde a ultima medicao: ");
	dbgTerminal.println(millis_intervalo_ultima_medicao);

	httpServerLoop();
}

void atualizaDisplay()
{
	// Limpar LCD
	lcdTerminal.write(12);
	delay(5);

	lcdTerminal.print("Hora: R$");
	lcdTerminal.print(meter.getCustoEstimadoHora(), 2);

	lcdTerminal.write(13); // New line

	lcdTerminal.print("Amp.: ");
	float corrente = meter.getCorrente();
	if (corrente > 0.01)
	{
		corrente += 0.005; // Arredondar para duas casas decimais
		lcdTerminal.print(corrente, 2);
		lcdTerminal.print("A");
	}
	else
	{
		lcdTerminal.print("---");
	}
}

void setupWiFi() {
	dbgTerminal.print("Setting up Wi-Fi\r\n");

	espSerial.listen();
	espSerial.setTimeout(100);

	if (!wifi.setOprToStationSoftAP()) {
		dbgTerminal.print("Error creating AP\r\n");
	}

	dbgTerminal.println(GetResponse("AT+CWSAP=\"Plug Meter 1\",\"plugmeter\",1,0"));
	delay(10);

 	//    while (1) {
 	//    	dbgTerminal.println("Trying to connect to ap");
	//     if (wifi.joinAP(SSID, PASSWORD)) {
	//         dbgTerminal.print("Join AP success\r\n");
	//         dbgTerminal.print("IP: ");       
	//         dbgTerminal.println(wifi.getLocalIP().c_str());
	//         break;
	//     } else {
	//         dbgTerminal.print("Join AP failure\r\n");
	//     }
	// }

	if (!wifi.enableMUX()) {
		dbgTerminal.print("multiple err\r\n");
	}

	if (!wifi.startTCPServer(80)) {
		dbgTerminal.print("start tcp server err\r\n");
	}

	if (!wifi.setTCPServerTimeout(10)) { 
		dbgTerminal.print("set tcp server timout err\r\n");
	}

	clearSerialBuffer();

	dbgTerminal.print("Setup end\r\n");

	dbgTerminal.println(wifi.getStationIp());
}

void httpServerLoop() {
	int ch_id, packet_len;
	int body_state;
	char *pb;  
	espSerial.readBytesUntil('\n', buffer, BUFFER_SIZE);

	if(strncmp(buffer, "+IPD,", 5)==0) {
		// request: +IPD,ch,len:data
		sscanf(buffer+5, "%d,%d", &ch_id, &packet_len);
		if (packet_len > 0) {
			// read serial until packet_len character received
			// start from :
			pb = buffer+5;
			while(*pb!=':') pb++;
			pb++;
			if (strncmp(pb, "POST /relay/", 12) == 0) {
				body_state = pb[12] - '0';

				clearSerialBuffer();

				if (body_state == 1) {
					relay_state = HIGH;
				} else if (body_state == 0) {
					relay_state = LOW;
				} else {
					dbgTerminal.println("Post body incorrect");
				}

				digitalWrite(pin_relay, relay_state);

				String content = String(relay_state);
				httpResponseWithBody(ch_id, content);

			} else if (strncmp(pb, "GET /current ", 6) == 0) {
				clearSerialBuffer();

				String content = String(meter.getCorrente());
				httpResponseWithBody(ch_id, content);

			} else if (strncmp(pb, "GET / ", 6) == 0) {
				clearSerialBuffer();

				String content = String(relay_state);
				httpResponseWithBody(ch_id, content);

			} else {
				httpResponseWith404(ch_id);
			}
		}
	}
	clearBuffer();
}

void httpResponseWith404(int ch_id) {
	String header;
	String content = String("404 Not found");

	header =  "HTTP/1.1 404 Not found\r\n";
	header += "Content-Type: text/html\r\n";
	header += "Connection: close\r\n";  


	header += "Content-Length: ";
	header += (int)(content.length());
	header += "\r\n\r\n";

	espSerial.print("AT+CIPSEND=");
	espSerial.print(ch_id);
	espSerial.print(",");
	espSerial.println(header.length()+content.length());
	delay(10);

	if (espSerial.find(">")) {
		espSerial.print(header);
		espSerial.print(content);
		delay(10);
	}
}

void httpResponseWithBody(int ch_id, String& content) {
	String header;

	header =  "HTTP/1.1 200 OK\r\n";
	header += "Content-Type: text/html\r\n";
	header += "Connection: close\r\n";  

	header += "Content-Length: ";
	header += (int)(content.length());
	header += "\r\n\r\n";

	espSerial.print("AT+CIPSEND=");
	espSerial.print(ch_id);
	espSerial.print(",");
	espSerial.println(header.length()+content.length());
	delay(10);

	if (espSerial.find(">")) {
		espSerial.print(header);
		espSerial.print(content);
		delay(10);
	}
}


// Get the data from the WiFi module and send it to the debug serial port
String GetResponse(String AT_Command){
	String tmpData;

	espSerial.println(AT_Command);

	delay(10);

	while (espSerial.available() >0 )  {
		char c = espSerial.read();
		tmpData += c;

		if (tmpData.indexOf(AT_Command) > -1)         
			tmpData = "";
		else
			tmpData.trim();       

	}
	
	return tmpData;
}

void clearSerialBuffer(void) {
	while (espSerial.available() > 0) {
		espSerial.read();
	}
}

void clearBuffer(void) {
	for (int i=0; i<BUFFER_SIZE; i++) {
		buffer[i] = 0;
	}
}

