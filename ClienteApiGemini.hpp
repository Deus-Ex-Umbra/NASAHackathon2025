#pragma once
#include <string>

class ClienteApiGemini {
public:
    std::string realizarConsulta(const std::string& consulta);
private:
    std::string api_key = "AQUI_VA_TU_CLAVE_DE_API_DE_GEMINI";
};