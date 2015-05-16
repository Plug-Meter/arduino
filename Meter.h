#ifndef Meter_h
#define Meter_h

class Meter {
	public:
		Meter(const float rede, const float preco_kwh);
		void setCorrente(const float corrente);
		void setRede(const float rede);
		void setPrecoKwh(const float preco_kwh);
		float getCorrente();
		float getPotencia();
		float getPotenciaTotalSegundos();
		float getPotenciaTotalHoras();
		float getCustoTotal();
		float getCustoEstimadoHora();
		float getRede();
		float getPrecoKwh();
	private:
		float corrente;
		float potencia_total_segundos;
		float rede;
		float preco_kwh;
};

#endif /* Meter_h */