# NASAHackathon.exe - Simulador de Impacto de Asteroides

## Descripción

NASAHackathon.exe es un simulador de impactos de asteroides que integra datos de la NASA API, cálculos orbitales precisos y análisis de consecuencias físicas. Incluye soporte para generación de resúmenes con IA (Gemini) y cálculos extendidos de efectos de impacto.

## Requisitos

- Windows 10 o superior
- PowerShell o CMD
- Conexión a internet (para APIs de NASA y Gemini)

## Instalación

1. Descarga el ejecutable `NASAHackathon.exe`
2. Colócalo en el directorio de tu preferencia (ej: `x64/Release`)
3. Abre PowerShell o CMD en ese directorio
4. Ejecuta los comandos según las secciones siguientes

## Uso General

### Formato Básico

```cmd
.\NASAHackathon.exe [opciones]
```

### Opciones Disponibles

| Opción | Tipo | Descripción |
|--------|------|-------------|
| `--help`, `-h` | Flag | Muestra la ayuda con todas las opciones |
| `--asteroide-id` | String | ID de SPK del asteroide (ej: 2099942 para Apophis) |
| `--semieje-mayor` | Double | Semieje mayor (a) en UA |
| `--excentricidad` | Double | Excentricidad (e) |
| `--inclinacion` | Double | Inclinación (i) en grados |
| `--nodo-ascendente` | Double | Longitud del nodo ascendente (Ω) en grados |
| `--arg-perihelio` | Double | Argumento del perihelio (ω) en grados |
| `--anomalia-media` | Double | Anomalía media inicial (M₀) en grados |
| `--diametro` | Double | Diámetro del asteroide en metros (default: 500.0) |
| `--densidad` | Double | Densidad en kg/m³ (default: 3000.0) |
| `--generar-resumen` | Flag | Genera resumen ejecutivo con IA (Gemini) |
| `--calculos-extendidos` | Flag | Incluye efectos atmosféricos, radios de destrucción y tsunamis |
| `--consulta-gemini` | String | Consulta de texto libre a Gemini (ignora otras opciones) |

**Nota:** Debes usar `--asteroide-id` O los parámetros orbitales manuales (semieje-mayor, excentricidad, etc.). La salida es JSON con detalles orbitales y de impacto.

---

## Casos de Uso

### 1. Mostrar Ayuda

```cmd
.\NASAHackathon.exe --help
```

### 2. Simulación Básica con ID de Asteroide

Obtiene datos orbitales desde la NASA API y simula la trayectoria.

#### Apophis (ID: 2099942)

```cmd
.\NASAHackathon.exe --asteroide-id 2099942
```

#### Con diámetro y densidad personalizados

```cmd
.\NASAHackathon.exe --asteroide-id 2099942 --diametro 370 --densidad 2600
```

#### Con cálculos extendidos

```cmd
.\NASAHackathon.exe --asteroide-id 2099942 --calculos-extendidos
```

#### Con resumen de IA

```cmd
.\NASAHackathon.exe --asteroide-id 2099942 --generar-resumen
```

#### Simulación completa (todos los parámetros)

```cmd
.\NASAHackathon.exe --asteroide-id 2099942 --diametro 370 --densidad 2600 --generar-resumen --calculos-extendidos
```

### 3. Simulación con Parámetros Orbitales Manuales

Define elementos orbitales personalizados para escenarios hipotéticos.

#### Ejemplo básico

```cmd
.\NASAHackathon.exe --semieje-mayor 1.078 --excentricidad 0.088 --inclinacion 10.593 --nodo-ascendente 204.446 --arg-perihelio 151.246 --anomalia-media 245.892
```

#### Con diámetro personalizado

```cmd
.\NASAHackathon.exe --semieje-mayor 1.078 --excentricidad 0.088 --inclinacion 10.593 --nodo-ascendente 204.446 --arg-perihelio 151.246 --anomalia-media 245.892 --diametro 150
```

#### Con resumen de IA

```cmd
.\NASAHackathon.exe --semieje-mayor 1.078 --excentricidad 0.088 --inclinacion 10.593 --nodo-ascendente 204.446 --arg-perihelio 151.246 --anomalia-media 245.892 --generar-resumen
```

#### Completo con cálculos extendidos

```cmd
.\NASAHackathon.exe --semieje-mayor 1.078 --excentricidad 0.088 --inclinacion 10.593 --nodo-ascendente 204.446 --arg-perihelio 151.246 --anomalia-media 245.892 --diametro 200 --densidad 2800 --generar-resumen --calculos-extendidos
```

### 4. Consultas a Gemini (IA)

Realiza consultas de texto libre a la API de Gemini sin ejecutar simulaciones.

#### Consulta simple

```cmd
.\NASAHackathon.exe --consulta-gemini "Explica el asteroide Apophis"
```

#### Consulta técnica

```cmd
.\NASAHackathon.exe --consulta-gemini "¿Cómo se calculan las órbitas keplerianas?"
```

#### Consulta sobre impactos

```cmd
.\NASAHackathon.exe --consulta-gemini "¿Qué efectos tendría un asteroide de 1 km de diámetro?"
```

---

## Formato de Salida

### Sin Impacto Detectado

```json
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
    "nota": "No se detecto impacto directo. Se reporta la aproximacion mas cercana."
}
```

### Con Impacto Detectado

```json
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
    }
}
```

### Con Cálculos Extendidos

```json
{
    "calculo_orbital": { ... },
    "consecuencias_impacto": { ... },
    "efectos_extendidos": {
        "polvo_atmosferico_tg": 0.1,
        "radios_destruccion": {
            "5_psi_km": 5.0,
            "10_psi_km": 3.0
        },
        "altura_ola_ejemplo_100km": 1.0
    }
}
```

### Consulta Gemini

```json
{
    "consulta": "Explica el asteroide Apophis",
    "respuesta_ia": "Apophis es un asteroide cercano a la Tierra..."
}
```

---

## Operadores de Shell (PowerShell/CMD)

### Redirección de Salida a Archivo (`>`)

Guarda la salida JSON en un archivo (sobrescribe si existe).

```cmd
.\NASAHackathon.exe --asteroide-id 2099942 > apophis_resultado.json
```

```cmd
.\NASAHackathon.exe --help > ayuda.txt
```

### Añadir a Archivo Existente (`>>`)

Agrega la salida al final de un archivo.

```cmd
.\NASAHackathon.exe --asteroide-id 2099942 >> simulaciones.log
```

```cmd
.\NASAHackathon.exe --asteroide-id 2099942 --generar-resumen >> historial.json
```

### Tubería (`|`)

Conecta la salida con otro comando (requiere herramientas adicionales como `jq`).

```cmd
.\NASAHackathon.exe --asteroide-id 2099942 | jq .calculo_orbital
```

```cmd
.\NASAHackathon.exe --consulta-gemini "Explica Apophis" | findstr "respuesta_ia"
```

### Combinación de Comandos (`&`, `&&`)

Ejecuta múltiples comandos en secuencia.

```cmd
.\NASAHackathon.exe --asteroide-id 2099942 > output.json & type output.json
```

```cmd
.\NASAHackathon.exe --asteroide-id 2099942 && echo "Simulacion completada"
```

---

## Ejemplos Avanzados

### Guardar múltiples simulaciones con timestamp

```cmd
.\NASAHackathon.exe --asteroide-id 2099942 > simulacion_%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%.json
```

### Ejecutar simulación y guardar solo si tiene éxito

```cmd
.\NASAHackathon.exe --asteroide-id 2099942 --generar-resumen && echo "Exito" || echo "Error"
```

### Crear reporte completo

```cmd
echo "=== REPORTE DE SIMULACION ===" > reporte.txt && .\NASAHackathon.exe --asteroide-id 2099942 --diametro 370 --generar-resumen --calculos-extendidos >> reporte.txt
```

### Batch de múltiples asteroides

Crea un archivo `simular_asteroides.bat`:

```batch
@echo off
echo Simulando Apophis...
.\NASAHackathon.exe --asteroide-id 2099942 > apophis.json

echo Simulando Bennu...
.\NASAHackathon.exe --asteroide-id 101955 > bennu.json

echo Simulaciones completadas.
```

---

## IDs de Asteroides Comunes (NASA)

| Nombre | SPK ID | Descripción |
|--------|--------|-------------|
| Apophis | 2099942 | Asteroide potencialmente peligroso |
| Bennu | 101955 | Objetivo de la misión OSIRIS-REx |
| Didymos | 65803 | Objetivo de la misión DART |
| Ryugu | 162173 | Objetivo de la misión Hayabusa2 |
| Eros | 433 | Primer asteroide orbitado por una nave |

### Ejemplo con Bennu

```cmd
.\NASAHackathon.exe --asteroide-id 101955 --diametro 490 --generar-resumen --calculos-extendidos
```

---

## Solución de Problemas

### Error: "La clave de API de Gemini no ha sido configurada"

Verifica que el ejecutable tenga acceso a internet y que las APIs estén activas.

### Error: "No se pudieron obtener los datos del asteroide desde la API de la NASA"

- Verifica el ID del asteroide
- Comprueba tu conexión a internet
- Intenta con otro ID conocido como 2099942

### Error de JSON al parsear

Asegúrate de que la salida no esté corrupta. Prueba redirigir a un archivo:

```cmd
.\NASAHackathon.exe --asteroide-id 2099942 > salida.json
```

### El comando no se reconoce

Asegúrate de estar en el directorio correcto donde se encuentra el ejecutable:

```cmd
cd ruta\al\directorio\x64\Release
```

---

## Licencia y Créditos

Desarrollado para el NASA Hackathon. Utiliza:
- NASA JPL SBDB API para datos orbitales
- Google Gemini API para generación de texto
- Cálculos de mecánica orbital kepleriana
- Modelos físicos de impacto

---

## Contacto y Soporte

Para reportar problemas o sugerencias, contacta al equipo de desarrollo.

**Última actualización:** Octubre 2025