#include "CalculadoraOrbital.hpp"
#include <cmath>
#include <vector>
#include <ctime>

#pragma warning(disable : 4996) // Desactiva la advertencia de localtime

const double PI = 3.14159265358979323846;
const double GM_SOL = 1.32712440018e20;
const double UA_A_METROS = 149597870700.0;
const double RADIO_TIERRA_KM = 6371.0;

double gradosARadianes(double grados) {
    return grados * PI / 180.0;
}

Vector3D CalculadoraOrbital::calcularPosicion(const ParametrosOrbitales& params, double dias_desde_epoca) {
    double a_m = params.semieje_mayor_ua * UA_A_METROS;
    double e = params.excentricidad;
    double i = gradosARadianes(params.inclinacion_grados);
    double omega_may = gradosARadianes(params.nodo_ascendente_grados);
    double omega_min = gradosARadianes(params.arg_perihelio_grados);
    double m0 = gradosARadianes(params.anomalia_media_grados);

    double n = sqrt(GM_SOL / pow(a_m, 3));
    double m = m0 + n * (dias_desde_epoca * 86400.0);

    double E = m;
    for (int iter = 0; iter < 5; ++iter) {
        E = E - (E - e * sin(E) - m) / (1 - e * cos(E));
    }

    double v = 2 * atan2(sqrt(1 + e) * sin(E / 2), sqrt(1 - e) * cos(E / 2));
    double r = a_m * (1 - e * cos(E));

    Vector3D pos_orbital = { r * cos(v), r * sin(v), 0 };

    double cw_may = cos(omega_may), sw_may = sin(omega_may);
    double cw_min = cos(omega_min), sw_min = sin(omega_min);
    double ci = cos(i);

    Vector3D pos_ecliptica;
    pos_ecliptica.x = pos_orbital.x * (cw_may * cw_min - sw_may * sw_min * ci) - pos_orbital.y * (cw_may * sw_min + sw_may * cw_min * ci);
    pos_ecliptica.y = pos_orbital.x * (sw_may * cw_min + cw_may * sw_min * ci) + pos_orbital.y * (cw_may * cw_min * ci - sw_may * sw_min);
    pos_ecliptica.z = pos_orbital.x * (sw_min * sin(i)) + pos_orbital.y * (cw_min * sin(i));

    return pos_ecliptica;
}

ResultadoOrbital CalculadoraOrbital::calcularInterseccion(const ParametrosOrbitales& params_asteroide) {
    ParametrosOrbitales params_tierra = { 1.00000011, 0.01671022, 0.00005, -11.26064, 102.94719, 100.46435 };
    ResultadoOrbital resultado;

    int dia_min_dist = -1;

    for (int dia = 0; dia < 365 * 15; ++dia) { // Simular 15 años
        Vector3D pos_asteroide = calcularPosicion(params_asteroide, dia);
        Vector3D pos_tierra = calcularPosicion(params_tierra, dia);

        double dist_x = (pos_asteroide.x - pos_tierra.x) / 1000.0;
        double dist_y = (pos_asteroide.y - pos_tierra.y) / 1000.0;
        double dist_z = (pos_asteroide.z - pos_tierra.z) / 1000.0;

        double distancia_actual_km = sqrt(pow(dist_x, 2) + pow(dist_y, 2) + pow(dist_z, 2));

        if (distancia_actual_km < resultado.distancia_minima_km) {
            resultado.distancia_minima_km = distancia_actual_km;
            dia_min_dist = dia;
        }
    }

    if (resultado.distancia_minima_km <= RADIO_TIERRA_KM) {
        resultado.hay_impacto = true;

        time_t tiempo_actual = time(0);
        tiempo_actual += dia_min_dist * 86400; // Añadir días hasta el impacto
        struct tm* fecha_impacto_tm = gmtime(&tiempo_actual);

        resultado.anio_impacto = fecha_impacto_tm->tm_year + 1900;
        resultado.mes_impacto = fecha_impacto_tm->tm_mon + 1;
        resultado.dia_impacto = fecha_impacto_tm->tm_mday;
    }

    return resultado;
}