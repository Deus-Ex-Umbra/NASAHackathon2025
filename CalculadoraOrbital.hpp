#pragma once
#include "TiposDeDatos.hpp"

class CalculadoraOrbital {
public:
    ResultadoOrbital calcularInterseccion(const ParametrosOrbitales& params_asteroide);

    // Métodos auxiliares para cálculos orbitales
    Vector3D calcularVelocidad(const ParametrosOrbitales& params, double dias_desde_epoca);

private:
    Vector3D calcularPosicion(const ParametrosOrbitales& params, double dias_desde_epoca);
};