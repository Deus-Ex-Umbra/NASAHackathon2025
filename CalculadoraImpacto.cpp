#include "CalculadoraImpacto.hpp"
#include <cmath>

const double JOULES_POR_MEGATON = 4.184e15;
const double PI = 3.14159265358979323846;

ResultadoImpactoFisico CalculadoraImpacto::calcularConsecuencias(
    const ParametrosAstrofisicos& params_fisicos,
    double velocidad_impacto_kms
) {
    ResultadoImpactoFisico resultado;

    // Cálculo de masa más preciso
    double radio_m = params_fisicos.diametro_m / 2.0;
    double volumen_m3 = (4.0 / 3.0) * PI * std::pow(radio_m, 3);
    double masa_kg = params_fisicos.densidad_kgm3 * volumen_m3;
    double velocidad_ms = velocidad_impacto_kms * 1000.0;

    // Energía cinética
    double energia_joules = 0.5 * masa_kg * std::pow(velocidad_ms, 2);
    resultado.energia_cinetica_megatones = energia_joules / JOULES_POR_MEGATON;

    // Diámetro del cráter usando la ecuación de Collins et al. (2005)
    // D = 1.161 * (W^0.267) * (rho_target/rho_projectile)^0.167 * (g_earth/g_target)^0.167 * sin(theta)^0.167
    // Para impacto vertical y condiciones terrestres estándar, simplificado:
    // D = 0.07 * E^(1/3.4) donde E es en joules, D es en metros
    resultado.diametro_crater_km = 0.001 * 0.07 * std::pow(energia_joules, 1.0 / 3.4);

    // Profundidad del cráter (típicamente diámetro/5 para cráteres complejos)
    resultado.profundidad_crater_km = resultado.diametro_crater_km / 5.0;

    // Magnitud sísmica usando la fórmula de Gutenberg-Richter modificada
    // M = (log10(E) - 4.8) / 1.5, donde E es en ergios
    // 1 Joule = 1e7 ergios
    double energia_ergios = energia_joules * 1e7;
    resultado.magnitud_sismica_richter = (std::log10(energia_ergios) - 4.8) / 1.5;

    return resultado;
}

double CalculadoraImpacto::calcularEfectoAtmosferico(
    const ParametrosAstrofisicos& params_fisicos,
    double velocidad_impacto_kms
) {
    // Cálculo del polvo atmosférico en Tg (teragramos)
    double radio_m = params_fisicos.diametro_m / 2.0;
    double volumen_m3 = (4.0 / 3.0) * PI * std::pow(radio_m, 3);
    double masa_kg = params_fisicos.densidad_kgm3 * volumen_m3;

    // Estimación de polvo atmosférico (aproximadamente 1% de la masa del impactador)
    double polvo_kg = masa_kg * 0.01;
    double polvo_tg = polvo_kg / 1e9;  // Convertir a teragramos

    return polvo_tg;
}

double CalculadoraImpacto::calcularRadioDestruccion(
    double energia_megatones,
    double presion_psi
) {
    // Cálculo del radio de destrucción basado en presión de sobrepresión
    // R = C * (E^(1/3)) donde C depende de la presión deseada
    // Para 5 psi (destrucción de estructuras): C ≈ 2.2 km/MT^(1/3)

    double factor = 2.2;  // Para 5 psi
    if (presion_psi >= 10.0) {
        factor = 1.7;  // Para 10 psi (destrucción severa)
    }
    else if (presion_psi >= 20.0) {
        factor = 1.3;  // Para 20 psi (destrucción total)
    }

    double radio_km = factor * std::pow(energia_megatones, 1.0 / 3.0);
    return radio_km;
}

double CalculadoraImpacto::calcularAlturaOla(
    double energia_megatones,
    double distancia_costa_km
) {
    // Estimación simplificada de altura de ola de tsunami
    // H = k * E^0.5 / d^0.5, donde k es una constante empírica

    if (distancia_costa_km <= 0.0) {
        return 0.0;
    }

    const double k_tsunami = 0.1;  // Constante empírica
    double altura_m = k_tsunami * std::sqrt(energia_megatones) / std::sqrt(distancia_costa_km);

    return altura_m;
}