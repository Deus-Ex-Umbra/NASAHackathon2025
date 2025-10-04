#include "CalculadoraOrbital.hpp"
#include <cmath>
#include <vector>
#include <limits>
const double PI = 3.14159265358979323846;
const double UA_A_KM = 149597870.7;
const double MU_SOL = 1.32712440018e11;
const double RADIO_TIERRA_KM = 6371.0;
const double UMBRAL_IMPACTO_KM = 6571.0;
double gradosARadianes(double grados) {
	return grados * PI / 180.0;
}
double radianesAGrados(double radianes) {
	return radianes * 180.0 / PI;
}
double resolverEcuacionKepler(double anomalia_media_rad, double excentricidad, double tolerancia = 1e-10) {
	double E = anomalia_media_rad;
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
	double x_orbital = r_km * std::cos(nu_rad);
	double y_orbital = r_km * std::sin(nu_rad);
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
	double n_km_dia = n_rad_dia * a_km;
	double vx_orbital = -n_km_dia * a_km / r_km * std::sin(E_rad);
	double vy_orbital = n_km_dia * a_km / r_km * std::sqrt(1.0 - params.excentricidad * params.excentricidad) * std::cos(E_rad);
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
	vel.x /= 86400.0;
	vel.y /= 86400.0;
	vel.z /= 86400.0;
	return vel;
}
ResultadoOrbital CalculadoraOrbital::calcularInterseccion(const ParametrosOrbitales& params_asteroide) {
	ResultadoOrbital resultado;
	resultado.distancia_minima_km = std::numeric_limits<double>::max();
	resultado.hay_impacto = false;
	int dias_simulacion = 36500;
	int paso_dias = 1;
	for (int dia = 0; dia < dias_simulacion; dia += paso_dias) {
		Vector3D pos_asteroide = calcularPosicion(params_asteroide, dia);
		double distancia_km = std::sqrt(
			pos_asteroide.x * pos_asteroide.x +
			pos_asteroide.y * pos_asteroide.y +
			pos_asteroide.z * pos_asteroide.z
		);
		if (distancia_km < resultado.distancia_minima_km) {
			resultado.distancia_minima_km = distancia_km;
			int anio_actual = 2025 + dia / 365;
			int dia_anio = dia % 365;
			resultado.anio_impacto = anio_actual;
			resultado.mes_impacto = (dia_anio / 30) + 1;
			resultado.dia_impacto = (dia_anio % 30) + 1;
			Vector3D vel_asteroide = calcularVelocidad(params_asteroide, dia);
			resultado.velocidad_relativa_kms = std::sqrt(
				vel_asteroide.x * vel_asteroide.x +
				vel_asteroide.y * vel_asteroide.y +
				vel_asteroide.z * vel_asteroide.z
			);
			resultado.latitud_impacto_grados = radianesAGrados(std::asin(pos_asteroide.z / distancia_km));
			resultado.longitud_impacto_grados = radianesAGrados(std::atan2(pos_asteroide.y, pos_asteroide.x));
		}
		if (distancia_km <= UMBRAL_IMPACTO_KM) {
			resultado.hay_impacto = true;
			break;
		}
	}
	return resultado;
}