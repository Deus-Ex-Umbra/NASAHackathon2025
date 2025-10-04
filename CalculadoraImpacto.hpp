#pragma once
#include "TiposDeDatos.hpp"

class CalculadoraImpacto {
public:
    ResultadoImpactoFisico calcularConsecuencias(
        const ParametrosAstrofisicos& params_fisicos,
        double velocidad_impacto_kms
    );
};