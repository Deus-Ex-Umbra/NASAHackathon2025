#include <iostream>
#include <string>
#include <sstream>
#include <boost/program_options.hpp>
#include "nlohmann/json.hpp"
#include "TiposDeDatos.hpp"
#include "CalculadoraOrbital.hpp"
#include "CalculadoraImpacto.hpp"
#include "ClienteApiNasa.hpp"
#include "ClienteApiGemini.hpp"

namespace po = boost::program_options;
using json = nlohmann::json;

int main(int argc, char* argv[]) {
    try {
        po::options_description desc("Opciones de simulación y consulta");
        desc.add_options()
            ("help,h", "Muestra la ayuda")
            ("asteroide-id", po::value<std::string>(), "ID de SPK del asteroide (ej. 2099942 para Apophis)")
            ("usar-clave-nasa", po::bool_switch()->default_value(false), "Indica si se debe usar la clave de API para la NASA")
            ("semieje-mayor", po::value<double>(), "Semieje mayor (a) en UA")
            ("excentricidad", po::value<double>(), "Excentricidad (e)")
            ("inclinacion", po::value<double>(), "Inclinación (i) en grados")
            ("nodo-ascendente", po::value<double>(), "Longitud del nodo ascendente (Ω) en grados")
            ("arg-perihelio", po::value<double>(), "Argumento del perihelio (ω) en grados")
            ("anomalia-media", po::value<double>(), "Anomalía media inicial (M₀) en grados")
            ("diametro", po::value<double>(), "Diámetro del asteroide en metros")
            ("densidad", po::value<double>(), "Densidad en kg/m^3")
            ("generar-resumen", po::bool_switch()->default_value(false), "Genera un resumen del impacto con IA")
            ("consulta-gemini", po::value<std::string>(), "Realiza una consulta de texto libre a la API de Gemini");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << std::endl;
            return 0;
        }

        ClienteApiGemini cliente_gemini;

        if (vm.count("consulta-gemini")) {
            std::string consulta = vm["consulta-gemini"].as<std::string>();
            std::string respuesta = cliente_gemini.realizarConsulta(consulta);
            json output;
            output["consulta"] = consulta;
            output["respuesta_ia"] = respuesta;
            std::cout << output.dump(4) << std::endl;
            return 0;
        }

        ParametrosOrbitales params_orbitales;
        if (vm.count("asteroide-id")) {
            ClienteApiNasa cliente_nasa;
            bool usar_clave = vm["usar-clave-nasa"].as<bool>();
            auto datos_obtenidos = cliente_nasa.obtenerDatosDeAsteroide(vm["asteroide-id"].as<std::string>());
            if (datos_obtenidos) {
                params_orbitales = *datos_obtenidos;
            }
            else {
                throw std::runtime_error("No se pudieron obtener los datos del asteroide desde la API de la NASA.");
            }
        }
        else if (vm.count("semieje-mayor")) {
            params_orbitales.semieje_mayor_ua = vm["semieje-mayor"].as<double>();
            params_orbitales.excentricidad = vm["excentricidad"].as<double>();
            params_orbitales.inclinacion_grados = vm["inclinacion"].as<double>();
            params_orbitales.nodo_ascendente_grados = vm["nodo-ascendente"].as<double>();
            params_orbitales.arg_perihelio_grados = vm["arg-perihelio"].as<double>();
            params_orbitales.anomalia_media_grados = vm["anomalia-media"].as<double>();
        }
        else {
            throw std::runtime_error("Debe proporcionar un --asteroide-id o los parámetros orbitales manuales.");
        }

        ParametrosAstrofisicos params_fisicos;
        params_fisicos.diametro_m = vm.count("diametro") ? vm["diametro"].as<double>() : 500.0;
        params_fisicos.densidad_kgm3 = vm.count("densidad") ? vm["densidad"].as<double>() : 3000.0;

        CalculadoraOrbital calc_orbital;
        ResultadoOrbital res_orbital = calc_orbital.calcularInterseccion(params_orbitales);

        json output;
        output["calculo_orbital"] = {
            {"hay_impacto", res_orbital.hay_impacto},
            {"fecha_impacto", {
                {"dia", res_orbital.dia_impacto},
                {"mes", res_orbital.mes_impacto},
                {"anio", res_orbital.anio_impacto}
            }},
            {"zona_impacto", {
                {"latitud", res_orbital.latitud_impacto_grados},
                {"longitud", res_orbital.longitud_impacto_grados}
            }}
        };

        if (res_orbital.hay_impacto) {
            CalculadoraImpacto calc_impacto;
            double velocidad_impacto_kms = 20.0;
            ResultadoImpactoFisico res_fisico = calc_impacto.calcularConsecuencias(params_fisicos, velocidad_impacto_kms);

            output["consecuencias_impacto"] = {
                {"energia_cinetica_megatones", res_fisico.energia_cinetica_megatones},
                {"crater", {
                    {"diametro_km", res_fisico.diametro_crater_km},
                    {"profundidad_km", res_fisico.profundidad_crater_km}
                }},
                {"magnitud_sismica_richter", res_fisico.magnitud_sismica_richter}
            };

            if (vm["generar-resumen"].as<bool>()) {
                std::stringstream prompt;
                prompt << "Actúa como un experto en comunicación de riesgos planetarios. "
                    << "Genera un resumen ejecutivo claro y conciso (máximo 150 palabras) sobre el siguiente escenario de impacto de asteroide. "
                    << "No uses lenguaje técnico excesivo. Los datos son: "
                    << "Fecha de impacto: " << res_orbital.dia_impacto << "/" << res_orbital.mes_impacto << "/" << res_orbital.anio_impacto << ". "
                    << "Lugar de impacto: Latitud " << res_orbital.latitud_impacto_grados << ", Longitud " << res_orbital.longitud_impacto_grados << ". "
                    << "Energía liberada: " << res_fisico.energia_cinetica_megatones << " megatones de TNT. "
                    << "Diámetro del cráter resultante: " << res_fisico.diametro_crater_km << " kilómetros. "
                    << "Magnitud sísmica generada: " << res_fisico.magnitud_sismica_richter << " en la escala de Richter.";
                output["resumen_ia"] = cliente_gemini.realizarConsulta(prompt.str());
            }
        }

        std::cout << output.dump(4) << std::endl;

    }
    catch (const std::exception& e) {
        json error_output;
        error_output["status"] = "error";
        error_output["message"] = e.what();
        std::cerr << error_output.dump(4) << std::endl;
        return 1;
    }
    return 0;
}