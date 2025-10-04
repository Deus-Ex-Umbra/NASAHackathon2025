#pragma once
#include "TiposDeDatos.hpp"
class CalculadoraImpacto {
public:
	ResultadoImpactoFisico calcularConsecuencias(
		const ParametrosAstrofisicos& params_fisicos,
		double velocidad_impacto_kms
	);
	double calcularEfectoAtmosferico(
		const ParametrosAstrofisicos& params_fisicos,
		double velocidad_impacto_kms
	);
	double calcularRadioDestruccion(
		double energia_megatones,
		double presion_psi = 5.0
	);
	double calcularAlturaOla(
		double energia_megatones,
		double distancia_costa_km
	);
};