#pragma once
#include "TiposDeDatos.hpp"
#include <string>
#include <optional>

class ClienteApiNasa {
public:
    std::optional<ParametrosOrbitales> obtenerDatosDeAsteroide(const std::string& spk_id);

private:
    std::string api_key = "eH13yd2sijPWcdfRxxcZTNY1ureIwsOoY2pyGCOa";
};