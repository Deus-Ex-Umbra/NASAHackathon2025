NASAHackathon.exe - Simulador de Impacto de Asteroides
Este documento proporciona una guía exhaustiva para usar el CLI NASAHackathon.exe, incluyendo descripciones, opciones y casos de uso. El CLI es un ejecutable único con flags para diferentes modos. Los comandos están formateados para facilitar la copia y el uso. Cada sección incluye el formato general, opciones y ejemplos prácticos.
Ejecuta siempre desde PowerShell o CMD en Windows, en el directorio donde se encuentra el ejecutable (e.g., x64/Release).
Uso General
Ejecuta el simulador con opciones para consultas IA, simulaciones orbitales o ayuda.
``` cmd
.\NASAHackathon.exe [opciones]
[opciones]
--help | -h: Muestra la ayuda con todas las opciones.
--asteroide-id arg: ID de SPK del asteroide (ej. 2099942 para Apophis). Obtiene datos de NASA API.
--semieje-mayor arg: Semieje mayor (a) en UA (manual, alternativo a --asteroide-id).
--excentricidad arg: Excentricidad (e) (manual).
--inclinacion arg: Inclinación (i) en grados (manual).
--nodo-ascendente arg: Longitud del nodo ascendente (Ω) en grados (manual).
--arg-perihelio arg: Argumento del perihelio (ω) en grados (manual).
--anomalia-media arg: Anomalía media inicial (M₀) en grados (manual).
--diametro arg: Diámetro del asteroide en metros (default: 500.0).
--densidad arg (=3000.0): Densidad en kg/m³.
--generar-resumen: Genera resumen ejecutivo con IA (Gemini).
--calculos-extendidos: Incluye efectos atmosféricos, radios de destrucción y altura de olas.
--consulta-gemini arg: Consulta de texto libre a Gemini (ignora otras opciones).
Nota: Debes usar --asteroide-id O parámetros manuales. La salida es JSON con detalles orbitales, impactos y más.
Ejemplos de Uso:
``` cmd
.\NASAHackathon.exe --help  # Muestra todas las opciones.
.\NASAHackathon.exe --asteroide-id 2099942  # Simulación básica para Apophis.
.\NASAHackathon.exe --semieje-mayor 1.078 --excentricidad 0.088 --inclinacion 10.593 --nodo-ascendente 204.446 --arg-perihelio 151.246 --anomalia-media 245.892  # Simulación manual.
.\NASAHackathon.exe --asteroide-id 2099942 --diametro 370 --densidad 2600 --generar-resumen --calculos-extendidos  # Completa con IA y extendidos.
Consulta a Gemini
Realiza una consulta libre a la API de Gemini para respuestas IA generales.
``` cmd
.\NASAHackathon.exe --consulta-gemini "consulta de texto"
[opciones específicas]
No aplica; ignora otras flags.
Ejemplos de Uso:
``` cmd
.\NASAHackathon.exe --consulta-gemini "Explica el asteroide Apophis"  # Consulta simple.
.\NASAHackathon.exe --consulta-gemini "Cómo calcular órbitas keplerianas"  # Tema técnico.
Salida Esperada (JSON aproximado):
``` json
{
"consulta": "Explica el asteroide Apophis",
"respuesta_ia": "Apophis es un asteroide cercano a la Tierra descubierto en 2004. Pasará cerca en 2029, pero no impactará."
}
Simulación con ID de Asteroide
Obtiene datos orbitales de NASA y simula trayectoria/impacto.
``` cmd
.\NASAHackathon.exe --asteroide-id ID [otras opciones]
[opciones específicas]
--diametro arg: Para cálculos de masa/energía.
--densidad arg: Para precisión física.
--generar-resumen: Agrega resumen IA.
--calculos-extendidos: Agrega efectos adicionales.
Ejemplos de Uso:
``` cmd
.\NASAHackathon.exe --asteroide-id 2099942  # Básico para Apophis.
.\NASAHackathon.exe --asteroide-id 2099942 --diametro 370 --calculos-extendidos  # Con extendidos.
Salida Esperada (JSON aproximado, sin impacto):
``` json
{
"calculo_orbital": {
"hay_impacto": false,
"distancia_minima_km": 32000.0,
"velocidad_relativa_kms": 12.5,
"fecha_aproximacion": {
"dia": 13,
"mes": 4,
"anio": 2029
},
"zona_aproximacion": {
"latitud": 37.6,
"longitud": -49.0
}
},
"efectos_extendidos": {
"polvo_atmosferico_tg": 0.1,
"radios_destruccion": {
"5_psi_km": 5.0,
"10_psi_km": 3.0
},
"altura_ola_ejemplo_100km": 1.0
},
"nota": "No se detecto impacto directo. Se reporta la aproximacion mas cercana."
}
Simulación con Parámetros Manuales
Simula con elementos orbitales personalizados (hipotéticos).
``` cmd
.\NASAHackathon.exe --semieje-mayor valor --excentricidad valor --inclinacion valor --nodo-ascendente valor --arg-perihelio valor --anomalia-media valor [otras opciones]
[opciones específicas]
Igual que arriba.
Ejemplos de Uso:
``` cmd
.\NASAHackathon.exe --semieje-mayor 1.078 --excentricidad 0.088 --inclinacion 10.593 --nodo-ascendente 204.446 --arg-perihelio 151.246 --anomalia-media 245.892 --diametro 150  # Básico manual.
.\NASAHackathon.exe --semieje-mayor 1.078 --excentricidad 0.088 --inclinacion 10.593 --nodo-ascendente 204.446 --arg-perihelio 151.246 --anomalia-media 245.892 --generar-resumen  # Con resumen IA.
Salida Esperada (JSON aproximado, con impacto hipotético):
``` json
{
"calculo_orbital": {
"hay_impacto": true,
"distancia_minima_km": 5000.0,
"velocidad_relativa_kms": 20.0,
"fecha_aproximacion": {
"dia": 15,
"mes": 6,
"anio": 2035
},
"zona_aproximacion": {
"latitud": 40.7,
"longitud": -74.0
}
},
"consecuencias_impacto": {
"energia_cinetica_megatones": 100.0,
"velocidad_impacto_kms": 20.0,
"crater": {
"diametro_km": 2.5,
"profundidad_km": 0.5
},
"magnitud_sismica_richter": 6.0
},
"resumen_ia": "Escenario hipotético de impacto en 2035 con energía de 100 megatones. Posible cráter de 2.5 km."
}
Operadores de Shell para NASAHackathon.exe
Estos operadores permiten redirigir salida, encadenar comandos o automatizar.
Tubería | (Pipe)
Conecta salida de un comando a entrada de otro.
``` cmd
.\NASAHackathon.exe [opciones] | {otro_comando}
Ejemplos de Uso:
``` cmd
.\NASAHackathon.exe --asteroide-id 2099942 | jq .calculo_orbital  # Filtra JSON con jq (si instalado).
.\NASAHackathon.exe --consulta-gemini "Hola" | tee response.txt  # Guarda y muestra respuesta.
Redirección de Salida >
Redirige salida a archivo (sobrescribe).
``` cmd
.\NASAHackathon.exe [opciones] > {archivo}
Ejemplos de Uso:
``` cmd
.\NASAHackathon.exe --asteroide-id 2099942 > apophis.json  # Guarda simulación en JSON.
.\NASAHackathon.exe --help > options.txt  # Guarda ayuda.
Redirección de Salida con Añadido >>
Añade salida a archivo existente.
``` cmd
.\NASAHackathon.exe [opciones] >> {archivo_log}
Ejemplos de Uso:
``` cmd
.\NASAHackathon.exe --asteroide-id 2099942 >> simulations.log  # Añade a log.
echo "Simulación Apophis: $(date)" >> log.txt; .\NASAHackathon.exe --asteroide-id 2099942 >> log.txt  # Añade timestamp y salida.
Redirección de Entrada <
Usa archivo como input (poco común para este CLI, ya que no lee stdin).
``` cmd
.\NASAHackathon.exe [opciones] < {archivo_input}
Ejemplos de Uso:
No aplica directamente, pero para scripts: crea params.txt con opciones, pero CLI usa args.
Here Document <<
Proporciona input multi-línea (útil para consultas Gemini en scripts).
``` cmd
.\NASAHackathon.exe --consulta-gemini << EOF
Consulta multi-línea
aquí.
EOF
Ejemplos de Uso:
``` cmd
.\NASAHackathon.exe --consulta-gemini << EOF
Explica Apophis en detalle.
Incluye datos orbitales.
EOF
Consulta multi-línea.
```