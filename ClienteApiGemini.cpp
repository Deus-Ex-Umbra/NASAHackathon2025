#include "ClienteApiGemini.hpp"
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
void limpiarBOMG(std::string& texto) {
	if (texto.size() >= 3 && texto[0] == (char)0xEF && texto[1] == (char)0xBB && texto[2] == (char)0xBF) {
		texto.erase(0, 3);
	}
}
std::string ClienteApiGemini::realizarConsulta(const std::string& consulta) {
	if (api_key.empty()) {
		throw std::runtime_error("La clave de API de Gemini no ha sido configurada.");
	}
	std::string url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash:generateContent";
	json request_body = {
	{"contents", {{
	{"parts", {{
	{"text", consulta}
	}}}
	}}}
	};
	cpr::Response r = cpr::Post(
		cpr::Url{ url },
		cpr::Header{
		{"Content-Type", "application/json"},
		{"x-goog-api-key", api_key}
		},
		cpr::Body{ request_body.dump() },
		cpr::Timeout{ 20000 },
		cpr::VerifySsl(false)
	);
	if (r.status_code != 200) {
		throw std::runtime_error("Error de red con API Gemini: " + std::to_string(r.status_code) +
			". Mensaje: " + r.error.message + ". Respuesta: " + r.text);
	}
	try {
		std::string respuesta_texto = r.text;
		limpiarBOMG(respuesta_texto);
		json response_json = json::parse(respuesta_texto);
		if (response_json.contains("candidates") &&
			!response_json["candidates"].empty() &&
			response_json["candidates"][0].contains("content") &&
			response_json["candidates"][0]["content"].contains("parts") &&
			!response_json["candidates"][0]["content"]["parts"].empty()) {
			return response_json["candidates"][0]["content"]["parts"][0]["text"];
		}
		throw std::runtime_error("Respuesta de API Gemini no contiene 'candidates' validos.");
	}
	catch (const json::exception& e) {
		throw std::runtime_error(std::string("Fallo al procesar JSON de Gemini: ") + e.what());
	}
}