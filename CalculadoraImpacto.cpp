#include "CalculadoraImpacto.hpp"
#include <cmath>
const double JOULES_POR_MEGATON = 4.184e15;
const double PI = 3.14159265358979323846;
ResultadoImpactoFisico CalculadoraImpacto::calcularConsecuencias(
	const ParametrosAstrofisicos& params_fisicos,
	double velocidad_impacto_kms
) {
	ResultadoImpactoFisico resultado;
	double radio_m = params_fisicos.diametro_m / 2.0;
	double volumen_m3 = (4.0 / 3.0) * PI * std::pow(radio_m, 3);
	double masa_kg = params_fisicos.densidad_kgm3 * volumen_m3;
	double velocidad_ms = velocidad_impacto_kms * 1000.0;
	double energia_joules = 0.5 * masa_kg * std::pow(velocidad_ms, 2);
	resultado.energia_cinetica_megatones = energia_joules / JOULES_POR_MEGATON;
	resultado.diametro_crater_km = 0.001 * 0.07 * std::pow(energia_joules, 1.0 / 3.4);
	resultado.profundidad_crater_km = resultado.diametro_crater_km / 5.0;
	double energia_ergios = energia_joules * 1e7;
	resultado.magnitud_sismica_richter = (std::log10(energia_ergios) - 4.8) / 1.5;
	return resultado;
}
double CalculadoraImpacto::calcularEfectoAtmosferico(
	const ParametrosAstrofisicos& params_fisicos,
	double velocidad_impacto_kms
) {
	double radio_m = params_fisicos.diametro_m / 2.0;
	double volumen_m3 = (4.0 / 3.0) * PI * std::pow(radio_m, 3);
	double masa_kg = params_fisicos.densidad_kgm3 * volumen_m3;
	double polvo_kg = masa_kg * 0.01;
	double polvo_tg = polvo_kg / 1e9;
	return polvo_tg;
}
double CalculadoraImpacto::calcularRadioDestruccion(
	double energia_megatones,
	double presion_psi
) {
	double factor = 2.2;
	if (presion_psi >= 10.0) {
		factor = 1.7;
	}
	else if (presion_psi >= 20.0) {
		factor = 1.3;
	}
	double radio_km = factor * std::pow(energia_megatones, 1.0 / 3.0);
	return radio_km;
}
double CalculadoraImpacto::calcularAlturaOla(
	double energia_megatones,
	double distancia_costa_km
) {
	if (distancia_costa_km <= 0.0) {
		return 0.0;
	}
	const double k_tsunami = 0.1;
	double altura_m = k_tsunami * std::sqrt(energia_megatones) / std::sqrt(distancia_costa_km);
	return altura_m;
}