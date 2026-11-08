# ESP32 Bare-Metal Low-Level Journey

Repositorio dedicado al estudio, experimentación y desarrollo a nivel de registros (*bare-metal*) en microcontroladores de la familia ESP32 y ESP32-S3. Este proyecto explora la inicialización directa del hardware, la gestión de memoria mediante scripts de enlace personalizados (*linkers*), código en ensamblador inline y manipulación directa de periféricos sin depender de SDKs pesados ni frameworks de alto nivel.

---

## 🛠️ Componentes y Arquitectura del Proyecto

El proyecto está diseñado bajo una estructura modular orientada a maximizar el entendimiento de la arquitectura interna del SoC (Xtensa):

* **`start.S`**: Código de arranque en ensamblador que configura manualmente el puntero de pila (*Stack Pointer*) y salta a la función principal en C.
* **`main.c`**: Lógica de la aplicación en C puro (desactivación de Watchdogs, configuración de IOMUX, retardos por contador de ciclos y parpadeo de LED).
* **`ld/`**: Scripts de enlace personalizados (`esp32.ld` y `esp32s3.ld`) que definen el mapa de memoria físico (`iram`, `dram`) y la distribución de secciones (`.text`, `.data`, `.bss`).
* **`Makefile`**: Automatización de la compilación condicional, generación de imágenes binarias y flasheo adaptado a cada modelo de chip.

---

## ⚙️ Requisitos del Sistema

Para compilar y flasehar este código necesitas tener instalado en tu entorno de desarrollo:
1. **Toolchain de Xtensa:** 
   * `xtensa-esp32s3-elf-gcc` (para ESP32-S3)
   * `xtensa-esp32-elf-gcc` (para ESP32 clásico)
2. **Utilidades de flasheo de Espressif:** 
   * `esptool.py` (para empaquetar binarios y escribirlos en la memoria flash).
3. **Make** y herramientas estándar de compilación (`GNU Make`).

---

## 🚀 Comandos de Compilación y Uso (`Makefile`)

El proyecto utiliza un Makefile flexible que detecta automáticamente qué chip y puerto serial utilizar mediante variables.

### 1. Compilación por defecto (para ESP32-S3)
```bash
make