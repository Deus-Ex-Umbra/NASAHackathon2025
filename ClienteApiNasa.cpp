#include "ClienteApiNasa.hpp"
#include <cpr/cpr.h>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

std::optional<ParametrosOrbitales> ClienteApiNasa::obtenerDatosDeAsteroide(const std::string& spk_id) {
    std::string url = "https://ssd-api.jpl.nasa.gov/sbdb.api?s-str=" + spk_id + "&orb=1";

    cpr::Response r = cpr::Get(cpr::Url{ url }, cpr::Timeout{ 10000 }, cpr::VerifySsl(false));

    if (r.status_code != 200) {
        throw std::runtime_error("Error de red con API NASA: " + std::to_string(r.status_code));
    }

    try {
        json data = json::parse(r.text);
        if (data.contains("orb") && data["orb"].contains("elements")) {
            auto& orbital_data = data["orb"]["elements"];
            ParametrosOrbitales params;
            params.semieje_mayor_ua = std::stod(orbital_data[0]["value"].get<std::string>());
            params.excentricidad = std::stod(orbital_data[1]["value"].get<std::string>());
            params.inclinacion_grados = std::stod(orbital_data[2]["value"].get<std::string>());
            params.nodo_ascendente_grados = std::stod(orbital_data[4]["value"].get<std::string>());
            params.arg_perihelio_grados = std::stod(orbital_data[5]["value"].get<std::string>());
            params.anomalia_media_grados = std::stod(orbital_data[7]["value"].get<std::string>());
            return params;
        }
        throw std::runtime_error("JSON de API NASA no contiene los datos orbitales esperados.");
    }
    catch (const std::exception& e) {
        throw std::runtime_error(std::string("Fallo al procesar JSON de NASA: ") + e.what());
    }
}