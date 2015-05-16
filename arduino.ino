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
const int intervalo_medicao_ms = 150; // Intervalo entre as medições
const double rede = 127.0; // Tensao da rede elétrica em Volts
const double sensibilidade = 66.0; // Parâmetro de sensibilidade do sensor (66mV/A)
const double preco_kwh = 0.71; // Preço de 1 kW.h da Light

HardwareSerial& dbgTerminal = Serial; // Serial usado para debug
SoftwareSerial lcdTerminal(0, pin_lcd); // Serial usado para o display lcd
SoftwareSerial espSerial(pin_esp8266_rx, pin_esp8266_tx); // Serial usado para o ESP8266
ESP8266 wifi(espSerial);

int relay_state = LOW;

char buffer[BUFFER_SIZE];
double potencia_total_segundos = 0.0;

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

	// setupWiFi();

	dbgTerminal.println("Calibrando sensor...");
	sensor.determineVQ(); // Read initial measurements for calibration
	dbgTerminal.println("Sensor calibrado.");
}

void loop()
{
	float corrente = sensor.readCurrent();
	meter.setCorrente(corrente);

	dbgTerminal.print("Corrente atual: ");
	dbgTerminal.print(corrente);
	dbgTerminal.println("A");

	atualizaDisplay();

	delay(intervalo_medicao_ms);
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