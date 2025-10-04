#include "ClienteApiNasa.hpp"
#include <cpr/cpr.h>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

std::optional<ParametrosOrbitales> ClienteApiNasa::obtenerDatosDeAsteroide(const std::string& spk_id) {
    // Construir URL con el parámetro correcto 'sstr' en lugar de 's-str'
    std::string url = "https://ssd-api.jpl.nasa.gov/sbdb.api?sstr=" + spk_id;

    // Solicitar datos orbitales
    cpr::Response r = cpr::Get(
        cpr::Url{ url },
        cpr::Timeout{ 10000 },
        cpr::VerifySsl(false)
    );

    if (r.status_code != 200) {
        throw std::runtime_error("Error de red con API NASA: " + std::to_string(r.status_code) +
            ". Respuesta: " + r.text);
    }

    try {
        json data = json::parse(r.text);

        // Verificar si la respuesta contiene datos orbitales
        if (!data.contains("orbit") || !data["orbit"].contains("elements")) {
            throw std::runtime_error("La respuesta de la API no contiene datos orbitales válidos.");
        }

        auto& orbital_data = data["orbit"]["elements"];
        ParametrosOrbitales params;

        // Extraer elementos orbitales
        // El orden de los elementos es: a, e, i, om, w, ma, tp, per, q, ad, n, epoch
        for (const auto& elemento : orbital_data) {
            std::string nombre = elemento["name"].get<std::string>();
            std::string valor_str = elemento["value"].get<std::string>();
            double valor = std::stod(valor_str);

            if (nombre == "e") {
                params.excentricidad = valor;
            }
            else if (nombre == "a") {
                params.semieje_mayor_ua = valor;
            }
            else if (nombre == "i") {
                params.inclinacion_grados = valor;
            }
            else if (nombre == "om") {
                params.nodo_ascendente_grados = valor;
            }
            else if (nombre == "w") {
                params.arg_perihelio_grados = valor;
            }
            else if (nombre == "ma") {
                params.anomalia_media_grados = valor;
            }
        }

        return params;
    }
    catch (const std::exception& e) {
        throw std::runtime_error(std::string("Fallo al procesar JSON de NASA: ") + e.what());
    }
}