#include "CalculadoraImpacto.hpp"
#include <cmath>

const double JOULES_POR_MEGATON = 4.184e15;
const double PI = 3.1415926535;

ResultadoImpactoFisico CalculadoraImpacto::calcularConsecuencias(
    const ParametrosAstrofisicos& params_fisicos,
    double velocidad_impacto_kms
) {
    ResultadoImpactoFisico resultado;

    double radio_m = params_fisicos.diametro_m / 2.0;
    double volumen_m3 = (4.0 / 3.0) * PI * pow(radio_m, 3);
    double masa_kg = params_fisicos.densidad_kgm3 * volumen_m3;
    double velocidad_ms = velocidad_impacto_kms * 1000.0;

    double energia_joules = 0.5 * masa_kg * pow(velocidad_ms, 2);

    resultado.energia_cinetica_megatones = energia_joules / JOULES_POR_MEGATON;
    resultado.diametro_crater_km = 0.001 * 0.07 * pow(energia_joules, 1.0 / 3.4);
    resultado.profundidad_crater_km = resultado.diametro_crater_km / 4.0;
    resultado.magnitud_sismica_richter = (log10(energia_joules) - 4.8) / 1.5;

    return resultado;
}