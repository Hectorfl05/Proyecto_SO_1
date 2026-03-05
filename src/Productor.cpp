#include "../include/Productor.h"
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>

/**
 * @file Productor.cpp
 * @brief Implementación del hilo Productor.
 *
 * El productor:
 *  1. Abre el archivo de números.
 *  2. Lee cada número y lo deposita en el buffer.
 *  3. Muestra en pantalla la acción de producción (animación).
 *  4. Señaliza al buffer que terminó al leer todos los números.
 */

Productor::Productor(Buffer& buffer, const std::string& archivo)
    : buffer_(buffer), archivo_(archivo) {}

void Productor::operator()() {
    std::ifstream archivo(archivo_);

    // Verificamos que el archivo se haya abierto correctamente
    if (!archivo.is_open()) {
        std::cerr << "[PRODUCTOR] ERROR: No se pudo abrir el archivo: " << archivo_ << "\n";
        buffer_.finalizarProduccion();
        return;
    }

    std::cout << "\033[1;34m╔══════════════════════════════════════╗\033[0m\n";
    std::cout << "\033[1;34m║     PRODUCTOR iniciado               ║\033[0m\n";
    std::cout << "\033[1;34m╚══════════════════════════════════════╝\033[0m\n";

    int numero;
    // Leemos número por número del archivo
    while (archivo >> numero) {
        // Depositamos el número en el buffer (puede bloquearse si está lleno)
        buffer_.producir(numero);

        // ── Animación: mostramos qué número se produjo y el estado del buffer ──
        std::cout << "\033[1;34m[PRODUCTOR]\033[0m Produjo: "
                  << "\033[1;33m" << numero << "\033[0m"
                  << "  | Buffer: " << buffer_.size() << "/" << buffer_.capacidad() << "\n";

        // Pequeña pausa para que la animación sea visible en tiempo real
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }

    archivo.close();

    // Notificamos al buffer que ya no habrá más producción
    buffer_.finalizarProduccion();

    std::cout << "\033[1;34m[PRODUCTOR]\033[0m Terminó de leer el archivo. Señal de fin enviada.\n";
}
