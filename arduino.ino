#include "CurrentSensor.h"

const int intervalo_medicao_ms = 150; // Intervalo entre as medições
const int pin_sensor = 1; // Pino analógico que o sensor está conectado
const double rede = 127.0; // Tensao da rede elétrica em Volts
const double sensibilidade = 66.0; // Parâmetro de sensibilidade do sensor (66mV/A)
const double preco_kwh = 0.71; // Preço de 1 kW.h da Light

double potencia_total_segundos = 0.0;

CurrentSensor sensor(pin_sensor, sensibilidade);

void setup()
{
  Serial.begin(9600);

  Serial.write(22); // Start LCD with no cursor and no blink

  Serial.print("Calibrando.");
  Serial.write(13); // New line
  Serial.print("Aguarde...");

  sensor.determineVQ(); // Read initial measurements for calibration
}

void loop()
{
  float corrente = sensor.readCurrent();
  float potencia = corrente * rede;

  potencia_total_segundos += potencia;

  float potencia_total_horas = potencia_total_segundos / 3600;
  float custo_total = (potencia_total_horas / 1000) * preco_kwh;
  if (custo_total < 0) 
  {
  	custo_total = 0;
  }

  float custo_est_hora = (potencia / 1000) * preco_kwh;
  if (custo_est_hora < 0) 
  {
  	custo_est_hora = 0;
  }

  // Limpar LCD
  Serial.write(12);
  delay(5);

  Serial.print("Hora: R$");
  Serial.print(custo_est_hora, 2);

  Serial.write(13); // New line

  Serial.print("Amp.: ");
  if (corrente > 0.01)
  {
    corrente += 0.005; // Arredondar para duas casas decimais
  	Serial.print(corrente, 2);
 	  Serial.print("A");
  }
  else
  {
  	Serial.print("---");
  }
  

  delay(intervalo_medicao_ms);
}