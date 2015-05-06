// EmonLib - openenergymonitor.org, Licence GNU GPL V3
// https://github.com/openenergymonitor/EmonLib

#include "EmonLib.h"

const int intervalo_medicao_ms = 1000; // Intervalo entre as medições
const int pin_sensor = 1; // Pino 	analógico que o sensor está conectado
const int numero_amostras = 1480; // Número de amostras que o sensor irá obter para cada requisição
const double rede = 127.0; // Tensao da rede elétrica em Volts
const double calibragem = 66.0; // Parâmetro de calibragem do sensor
const double leitura_inicial = 0.24; // Leitura residual do sensor quando nada está conectado
const double preco_kwh = 0.71; // Preço de 1 kW.h da Light

double potencia_total_segundos = 0.0;

EnergyMonitor emon;

void setup()
{
  Serial.begin(9600);

  Serial.write(22); // Start LCD with no cursor and no blink

  emon.current(pin_sensor, calibragem);
}

void loop()
{
  double irms = emon.calcIrms(numero_amostras);
  double corrente = irms - leitura_inicial;
  double potencia = corrente * rede;

  potencia_total_segundos += potencia;

  double potencia_total_horas = potencia_total_segundos / 3600;
  double custo_total = (potencia_total_horas / 1000) * preco_kwh;
  if (custo_total < 0) 
  {
  	custo_total = 0;
  }

  double custo_est_hora = (potencia / 1000) * preco_kwh;
  if (custo_est_hora < 0) 
  {
  	custo_est_hora = 0;
  }

  // Limpar LCD
  Serial.write(12);
  delay(5);

  Serial.print("Hora: R$");
  Serial.print(custo_est_hora);

  Serial.write(13); // New line

  Serial.print("Amp.: ");
  if (corrente > 0.01)
  {
  	Serial.print(corrente);
 	 Serial.print("A");
  }
  else
  {
  	Serial.print("---");
  }
  

  delay(intervalo_medicao_ms);
}