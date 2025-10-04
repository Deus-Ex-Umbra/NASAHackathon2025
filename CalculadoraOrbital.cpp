#include "CalculadoraOrbital.hpp"
#include <cmath>
#include <vector>
#include <limits>

const double PI = 3.14159265358979323846;
const double UA_A_KM = 149597870.7;  // 1 UA en kilómetros
const double MU_SOL = 1.32712440018e11;  // Constante gravitacional del Sol (km³/s²)
const double RADIO_TIERRA_KM = 6371.0;  // Radio medio de la Tierra
const double UMBRAL_IMPACTO_KM = 6571.0;  // Radio de la Tierra + 200 km de atmósfera

// Convertir grados a radianes
double gradosARadianes(double grados) {
    return grados * PI / 180.0;
}

// Convertir radianes a grados
double radianesAGrados(double radianes) {
    return radianes * 180.0 / PI;
}

// Resolver la ecuación de Kepler para anomalía excéntrica
double resolverEcuacionKepler(double anomalia_media_rad, double excentricidad, double tolerancia = 1e-10) {
    double E = anomalia_media_rad;  // Primera aproximación
    double delta = 1.0;
    int max_iteraciones = 100;
    int iteracion = 0;

    while (std::abs(delta) > tolerancia && iteracion < max_iteraciones) {
        delta = E - excentricidad * std::sin(E) - anomalia_media_rad;
        E = E - delta / (1.0 - excentricidad * std::cos(E));
        iteracion++;
    }

    return E;
}

Vector3D CalculadoraOrbital::calcularPosicion(const ParametrosOrbitales& params, double dias_desde_epoca) {
    // Convertir ángulos a radianes
    double i_rad = gradosARadianes(params.inclinacion_grados);
    double omega_rad = gradosARadianes(params.nodo_ascendente_grados);
    double w_rad = gradosARadianes(params.arg_perihelio_grados);
    double M0_rad = gradosARadianes(params.anomalia_media_grados);

    // Convertir semieje mayor a km
    double a_km = params.semieje_mayor_ua * UA_A_KM;

    // Calcular movimiento medio (n) en radianes/día
    double periodo_dias = 365.256363 * std::pow(params.semieje_mayor_ua, 1.5);
    double n_rad_dia = 2.0 * PI / periodo_dias;

    // Calcular anomalía media en el tiempo dado
    double M_rad = M0_rad + n_rad_dia * dias_desde_epoca;

    // Normalizar M a [0, 2π]
    M_rad = std::fmod(M_rad, 2.0 * PI);
    if (M_rad < 0) M_rad += 2.0 * PI;

    // Resolver ecuación de Kepler para obtener anomalía excéntrica
    double E_rad = resolverEcuacionKepler(M_rad, params.excentricidad);

    // Calcular anomalía verdadera (ν)
    double nu_rad = 2.0 * std::atan2(
        std::sqrt(1.0 + params.excentricidad) * std::sin(E_rad / 2.0),
        std::sqrt(1.0 - params.excentricidad) * std::cos(E_rad / 2.0)
    );

    // Calcular radio orbital
    double r_km = a_km * (1.0 - params.excentricidad * std::cos(E_rad));

    // Posición en el plano orbital
    double x_orbital = r_km * std::cos(nu_rad);
    double y_orbital = r_km * std::sin(nu_rad);

    // Transformar al sistema de coordenadas eclíptico
    double cos_omega = std::cos(omega_rad);
    double sin_omega = std::sin(omega_rad);
    double cos_i = std::cos(i_rad);
    double sin_i = std::sin(i_rad);
    double cos_w = std::cos(w_rad);
    double sin_w = std::sin(w_rad);

    Vector3D pos;
    pos.x = (cos_omega * cos_w - sin_omega * sin_w * cos_i) * x_orbital +
        (-cos_omega * sin_w - sin_omega * cos_w * cos_i) * y_orbital;
    pos.y = (sin_omega * cos_w + cos_omega * sin_w * cos_i) * x_orbital +
        (-sin_omega * sin_w + cos_omega * cos_w * cos_i) * y_orbital;
    pos.z = (sin_w * sin_i) * x_orbital + (cos_w * sin_i) * y_orbital;

    return pos;
}

Vector3D CalculadoraOrbital::calcularVelocidad(const ParametrosOrbitales& params, double dias_desde_epoca) {
    // Calcular posición orbital
    double i_rad = gradosARadianes(params.inclinacion_grados);
    double omega_rad = gradosARadianes(params.nodo_ascendente_grados);
    double w_rad = gradosARadianes(params.arg_perihelio_grados);
    double M0_rad = gradosARadianes(params.anomalia_media_grados);

    double a_km = params.semieje_mayor_ua * UA_A_KM;
    double periodo_dias = 365.256363 * std::pow(params.semieje_mayor_ua, 1.5);
    double n_rad_dia = 2.0 * PI / periodo_dias;
    double M_rad = M0_rad + n_rad_dia * dias_desde_epoca;
    M_rad = std::fmod(M_rad, 2.0 * PI);
    if (M_rad < 0) M_rad += 2.0 * PI;

    double E_rad = resolverEcuacionKepler(M_rad, params.excentricidad);
    double nu_rad = 2.0 * std::atan2(
        std::sqrt(1.0 + params.excentricidad) * std::sin(E_rad / 2.0),
        std::sqrt(1.0 - params.excentricidad) * std::cos(E_rad / 2.0)
    );

    double r_km = a_km * (1.0 - params.excentricidad * std::cos(E_rad));

    // Calcular velocidad en el plano orbital (km/día)
    double n_km_dia = n_rad_dia * a_km;
    double vx_orbital = -n_km_dia * a_km / r_km * std::sin(E_rad);
    double vy_orbital = n_km_dia * a_km / r_km * std::sqrt(1.0 - params.excentricidad * params.excentricidad) * std::cos(E_rad);

    // Transformar al sistema eclíptico
    double cos_omega = std::cos(omega_rad);
    double sin_omega = std::sin(omega_rad);
    double cos_i = std::cos(i_rad);
    double sin_i = std::sin(i_rad);
    double cos_w = std::cos(w_rad);
    double sin_w = std::sin(w_rad);

    Vector3D vel;
    vel.x = (cos_omega * cos_w - sin_omega * sin_w * cos_i) * vx_orbital +
        (-cos_omega * sin_w - sin_omega * cos_w * cos_i) * vy_orbital;
    vel.y = (sin_omega * cos_w + cos_omega * sin_w * cos_i) * vx_orbital +
        (-sin_omega * sin_w + cos_omega * cos_w * cos_i) * vy_orbital;
    vel.z = (sin_w * sin_i) * vx_orbital + (cos_w * sin_i) * vy_orbital;

    // Convertir de km/día a km/s
    vel.x /= 86400.0;
    vel.y /= 86400.0;
    vel.z /= 86400.0;

    return vel;
}

ResultadoOrbital CalculadoraOrbital::calcularInterseccion(const ParametrosOrbitales& params_asteroide) {
    ResultadoOrbital resultado;

    resultado.distancia_minima_km = std::numeric_limits<double>::max();
    resultado.hay_impacto = false;

    // Simular órbita durante 100 años (aproximadamente 36500 días)
    int dias_simulacion = 36500;
    int paso_dias = 1;  // Paso de 1 día

    for (int dia = 0; dia < dias_simulacion; dia += paso_dias) {
        Vector3D pos_asteroide = calcularPosicion(params_asteroide, dia);

        // Calcular distancia a la Tierra (asumiendo Tierra en origen para simplificar)
        // En una implementación más precisa, se calcularía la posición de la Tierra también
        double distancia_km = std::sqrt(
            pos_asteroide.x * pos_asteroide.x +
            pos_asteroide.y * pos_asteroide.y +
            pos_asteroide.z * pos_asteroide.z
        );

        // Actualizar distancia mínima
        if (distancia_km < resultado.distancia_minima_km) {
            resultado.distancia_minima_km = distancia_km;

            // Calcular fecha aproximada
            int anio_actual = 2025 + dia / 365;
            int dia_anio = dia % 365;
            resultado.anio_impacto = anio_actual;
            resultado.mes_impacto = (dia_anio / 30) + 1;
            resultado.dia_impacto = (dia_anio % 30) + 1;

            // Calcular velocidad relativa
            Vector3D vel_asteroide = calcularVelocidad(params_asteroide, dia);
            resultado.velocidad_relativa_kms = std::sqrt(
                vel_asteroide.x * vel_asteroide.x +
                vel_asteroide.y * vel_asteroide.y +
                vel_asteroide.z * vel_asteroide.z
            );

            // Calcular lat/lon aproximadas (simplificado)
            resultado.latitud_impacto_grados = radianesAGrados(std::asin(pos_asteroide.z / distancia_km));
            resultado.longitud_impacto_grados = radianesAGrados(std::atan2(pos_asteroide.y, pos_asteroide.x));
        }

        // Verificar impacto
        if (distancia_km <= UMBRAL_IMPACTO_KM) {
            resultado.hay_impacto = true;
            break;
        }
    }

    return resultado;
}