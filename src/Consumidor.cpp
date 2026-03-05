#include "../include/Consumidor.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <string>

/**
 * @file Consumidor.cpp
 * @brief Implementación del hilo Consumidor.
 *
 * Cada consumidor corre en su propio hilo y tiene asignado un tipo:
 *   - Tipo 0 (PARES):   Solo toma números pares del buffer.
 *   - Tipo 1 (IMPARES): Solo toma números impares del buffer.
 *   - Tipo 2 (PRIMOS):  Solo toma números primos del buffer.
 *
 * Cada consumidor muestra en pantalla:
 *   - El número que consumió.
 *   - La suma acumulada hasta el momento.
 */

// Códigos ANSI para color por tipo de consumidor
static const char* COLORES[] = {
    "\033[1;32m",  // Verde  → consumidor de PARES
    "\033[1;35m",  // Magenta→ consumidor de IMPARES
    "\033[1;36m"   // Cyan   → consumidor de PRIMOS
};
static const char* RESET = "\033[0m";

Consumidor::Consumidor(int id, int tipo, Buffer& buffer)
    : id_(id), tipo_(tipo), buffer_(buffer) {}

void Consumidor::operator()() {
    long long sumaAcumulada = 0; // Suma de todos los números consumidos por este hilo
    int valor = 0;
    int contador = 0;

    std::cout << COLORES[tipo_] << "[CONSUMIDOR " << id_ << " - " << nombreTipo() << "]"
              << RESET << " Iniciado.\n";

    // El consumidor sigue tomando del buffer hasta que no queden elementos de su tipo
    while (buffer_.consumir(tipo_, valor)) {
        sumaAcumulada += valor;
        contador++;

        // ── Animación: mostramos el número consumido y la suma parcial ──
        std::cout << COLORES[tipo_]
                  << "[CONSUMIDOR " << id_ << " - " << nombreTipo() << "]"
                  << RESET
                  << " Consumió: \033[1;33m" << valor << RESET
                  << "  | Suma acumulada: \033[1;37m" << sumaAcumulada << RESET
                  << "  | Total consumidos: " << contador << "\n";

        // Pausa para que la animación sea apreciable en tiempo real
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    // Reporte final del consumidor al terminar
    std::cout << COLORES[tipo_]
              << "╔══════════════════════════════════════════════╗\n"
              << "║ CONSUMIDOR " << id_ << " [" << nombreTipo() << "] TERMINÓ\n"
              << "║ Números consumidos: " << contador << "\n"
              << "║ Suma total        : " << sumaAcumulada << "\n"
              << "╚══════════════════════════════════════════════╝"
              << RESET << "\n";
}

/**
 * @brief Retorna el nombre del tipo de consumidor para la animación.
 */
std::string Consumidor::nombreTipo() const {
    switch (tipo_) {
        case 0: return "PARES";
        case 1: return "IMPARES";
        case 2: return "PRIMOS";
        default: return "DESCONOCIDO";
    }
}
