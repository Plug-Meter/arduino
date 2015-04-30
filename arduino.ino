// EmonLib - openenergymonitor.org, Licence GNU GPL V3
// https://github.com/openenergymonitor/EmonLib

#include "EmonLib.h"

const int intervalo_medicao_ms = 1000; // Intervalo entre as medições
const int pin_sensor = 1; // Pino analógico que o sensor está conectado
const int numero_amostras = 1480; // Número de amostras que o sensor irá obter para cada requisição
const double rede = 127.0; // Tensao da rede elétrica em Volts
const double calibragem = 88.0; // Parâmetro de calibragem do sensor
const double leitura_inicial = 0.035; // Leitura residual do sensor quando nada está conectado
const double preco_kwh = 0.71; // Preço de 1 kW.h da Light

double potencia_total_segundos = 0.0;

EnergyMonitor emon;

void setup()
{  
  Serial.begin(9600);
  
  emon.current(pin_sensor, calibragem);
}

void loop()
{
  double irms = emon.calcIrms(numero_amostras);
  double corrente = irms - (leitura_inicial * calibragem);
  double potencia = corrente * rede;

  potencia_total_segundos += potencia;

  double potencia_total_horas = potencia_total_segundos / 3600;
  double custo_total = (potencia_total_horas/1000) * preco_kwh;

  Serial.print("Custo total desde o começo da medição: R$");
  Serial.println(custo_total);
   
  delay(intervalo_medicao_ms);
}
