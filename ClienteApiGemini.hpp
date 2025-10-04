#pragma once
#include <string>
class ClienteApiGemini {
public:
	std::string realizarConsulta(const std::string& consulta);
private:
	std::string api_key = "AIzaSyDH97weA9KAucEIugXHsLGiRHA_MuY8iYw";
};