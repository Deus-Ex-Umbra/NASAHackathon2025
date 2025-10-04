#pragma once
#include "TiposDeDatos.hpp"

class CalculadoraOrbital {
public:
    ResultadoOrbital calcularInterseccion(const ParametrosOrbitales& params_asteroide);

private:
    Vector3D calcularPosicion(const ParametrosOrbitales& params, double dias_desde_epoca);
};