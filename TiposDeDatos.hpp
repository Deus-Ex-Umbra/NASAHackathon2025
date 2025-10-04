#pragma once
#include <string>
#include <optional>
struct Vector3D {
	double x = 0.0, y = 0.0, z = 0.0;
};
struct ParametrosOrbitales {
	double semieje_mayor_ua;
	double excentricidad;
	double inclinacion_grados;
	double nodo_ascendente_grados;
	double arg_perihelio_grados;
	double anomalia_media_grados;
};
struct ParametrosAstrofisicos {
	double diametro_m;
	double densidad_kgm3;
};
struct ResultadoOrbital {
	bool hay_impacto = false;
	int dia_impacto = 0;
	int mes_impacto = 0;
	int anio_impacto = 0;
	double distancia_minima_km = 1e12;
	double velocidad_relativa_kms = 0.0;
	double latitud_impacto_grados = 0.0;
	double longitud_impacto_grados = 0.0;
};
struct ResultadoImpactoFisico {
	double energia_cinetica_megatones;
	double diametro_crater_km;
	double profundidad_crater_km;
	double magnitud_sismica_richter;
};