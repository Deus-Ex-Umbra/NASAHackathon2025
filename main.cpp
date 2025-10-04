#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
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
            ("semieje-mayor", po::value<double>(), "Semieje mayor (a) en UA")
            ("excentricidad", po::value<double>(), "Excentricidad (e)")
            ("inclinacion", po::value<double>(), "Inclinación (i) en grados")
            ("nodo-ascendente", po::value<double>(), "Longitud del nodo ascendente (Ω) en grados")
            ("arg-perihelio", po::value<double>(), "Argumento del perihelio (ω) en grados")
            ("anomalia-media", po::value<double>(), "Anomalía media inicial (M₀) en grados")
            ("diametro", po::value<double>(), "Diámetro del asteroide en metros")
            ("densidad", po::value<double>()->default_value(3000.0), "Densidad en kg/m³")
            ("generar-resumen", po::bool_switch()->default_value(false), "Genera un resumen del impacto con IA")
            ("calculos-extendidos", po::bool_switch()->default_value(false), "Incluye cálculos de efectos atmosféricos y destrucción")
            ("consulta-gemini", po::value<std::string>(), "Realiza una consulta de texto libre a la API de Gemini");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << std::endl;
            return 0;
        }

        ClienteApiGemini cliente_gemini;

        // Modo de consulta directa a Gemini
        if (vm.count("consulta-gemini")) {
            std::string consulta = vm["consulta-gemini"].as<std::string>();
            std::string respuesta = cliente_gemini.realizarConsulta(consulta);
            json output;
            output["consulta"] = consulta;
            output["respuesta_ia"] = respuesta;
            std::cout << output.dump(4) << std::endl;
            return 0;
        }

        // Obtener parámetros orbitales
        ParametrosOrbitales params_orbitales;
        if (vm.count("asteroide-id")) {
            ClienteApiNasa cliente_nasa;
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

        // Parámetros físicos del asteroide
        ParametrosAstrofisicos params_fisicos;
        params_fisicos.diametro_m = vm.count("diametro") ? vm["diametro"].as<double>() : 500.0;
        params_fisicos.densidad_kgm3 = vm["densidad"].as<double>();

        // Cálculos orbitales
        CalculadoraOrbital calc_orbital;
        ResultadoOrbital res_orbital = calc_orbital.calcularInterseccion(params_orbitales);

        json output;
        output["calculo_orbital"] = {
            {"hay_impacto", res_orbital.hay_impacto},
            {"distancia_minima_km", res_orbital.distancia_minima_km},
            {"velocidad_relativa_kms", res_orbital.velocidad_relativa_kms},
            {"fecha_aproximacion", {
                {"dia", res_orbital.dia_impacto},
                {"mes", res_orbital.mes_impacto},
                {"anio", res_orbital.anio_impacto}
            }},
            {"zona_aproximacion", {
                {"latitud", res_orbital.latitud_impacto_grados},
                {"longitud", res_orbital.longitud_impacto_grados}
            }}
        };

        if (res_orbital.hay_impacto) {
            CalculadoraImpacto calc_impacto;

            // Usar velocidad relativa calculada o valor por defecto
            double velocidad_impacto_kms = res_orbital.velocidad_relativa_kms > 0.0
                ? res_orbital.velocidad_relativa_kms
                : 20.0;

            ResultadoImpactoFisico res_fisico = calc_impacto.calcularConsecuencias(
                params_fisicos,
                velocidad_impacto_kms
            );

            output["consecuencias_impacto"] = {
                {"energia_cinetica_megatones", res_fisico.energia_cinetica_megatones},
                {"velocidad_impacto_kms", velocidad_impacto_kms},
                {"crater", {
                    {"diametro_km", res_fisico.diametro_crater_km},
                    {"profundidad_km", res_fisico.profundidad_crater_km}
                }},
                {"magnitud_sismica_richter", res_fisico.magnitud_sismica_richter}
            };

            // Cálculos extendidos si se solicitan
            if (vm["calculos-extendidos"].as<bool>()) {
                double polvo_atmosferico = calc_impacto.calcularEfectoAtmosferico(
                    params_fisicos,
                    velocidad_impacto_kms
                );

                double radio_5psi = calc_impacto.calcularRadioDestruccion(
                    res_fisico.energia_cinetica_megatones,
                    5.0
                );

                double radio_10psi = calc_impacto.calcularRadioDestruccion(
                    res_fisico.energia_cinetica_megatones,
                    10.0
                );

                output["efectos_extendidos"] = {
                    {"polvo_atmosferico_tg", polvo_atmosferico},
                    {"radios_destruccion", {
                        {"5_psi_km", radio_5psi},
                        {"10_psi_km", radio_10psi}
                    }}
                };

                // Si el impacto es oceánico (simplificación basada en latitud/longitud)
                // En producción, se consultaría una base de datos geográfica
                output["efectos_extendidos"]["altura_ola_ejemplo_100km"] =
                    calc_impacto.calcularAlturaOla(res_fisico.energia_cinetica_megatones, 100.0);
            }

            // Generar resumen con IA si se solicita
            if (vm["generar-resumen"].as<bool>()) {
                std::stringstream prompt;
                prompt << "Actúa como un experto en comunicación de riesgos planetarios. "
                    << "Genera un resumen ejecutivo claro y conciso (máximo 150 palabras) sobre el siguiente escenario de impacto de asteroide. "
                    << "No uses lenguaje técnico excesivo. Los datos son: "
                    << "Fecha de posible impacto/aproximación: " << res_orbital.dia_impacto << "/"
                    << res_orbital.mes_impacto << "/" << res_orbital.anio_impacto << ". "
                    << "Distancia mínima a la Tierra: " << std::fixed << std::setprecision(0)
                    << res_orbital.distancia_minima_km << " km. "
                    << "Velocidad relativa: " << std::setprecision(1)
                    << res_orbital.velocidad_relativa_kms << " km/s. ";

                if (res_orbital.hay_impacto) {
                    prompt << "Lugar de impacto: Latitud " << std::setprecision(2)
                        << res_orbital.latitud_impacto_grados << "°, Longitud "
                        << res_orbital.longitud_impacto_grados << "°. "
                        << "Energía liberada: " << std::setprecision(2)
                        << res_fisico.energia_cinetica_megatones << " megatones de TNT. "
                        << "Diámetro del cráter resultante: " << std::setprecision(1)
                        << res_fisico.diametro_crater_km << " kilómetros. "
                        << "Magnitud sísmica generada: " << std::setprecision(1)
                        << res_fisico.magnitud_sismica_richter << " en la escala de Richter.";
                }

                output["resumen_ia"] = cliente_gemini.realizarConsulta(prompt.str());
            }
        }
        else {
            output["nota"] = "No se detectó impacto directo. Se reporta la aproximación más cercana.";
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