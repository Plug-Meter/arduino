#include "Meter.h"

Meter::Meter(const float rede, const float preco_kwh) {
	this->rede = rede;
	this->preco_kwh = preco_kwh;
	this->potencia_total_segundos = 0;
}

void Meter::setCorrente(const float corrente) {
	this->corrente = corrente;
	this->potencia_total_segundos += this->getPotencia();
}

void Meter::setRede(const float rede) {
	this->rede = rede;
}

void Meter::setPrecoKwh(const float preco_kwh) {
	this->preco_kwh = preco_kwh;
}

float Meter::getCorrente() {
	return this->corrente;
}

float Meter::getPotencia() {
	return corrente * this->rede;
}

float Meter::getPotenciaTotalSegundos() {
	return this->potencia_total_segundos;
}

float Meter::getPotenciaTotalHoras() {
	return this->potencia_total_segundos / 3600;
}

float Meter::getCustoTotal() {
	float custo_total = (this->getPotenciaTotalHoras() / 1000) * this->preco_kwh;

	if (custo_total < 0) 
	{
		custo_total = 0;
	}

	return custo_total;
}

float Meter::getCustoEstimadoHora() {
	float custo_est_hora = (this->getPotencia() / 1000) * this->preco_kwh;
	
	if (custo_est_hora < 0) 
	{
		custo_est_hora = 0;
	}

	return custo_est_hora;
}

float Meter::getRede() {
	return this->rede;
}

float Meter::getPrecoKwh() {
	return this->preco_kwh;
}
