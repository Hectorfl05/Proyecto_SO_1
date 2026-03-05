/**
 * @file main.cpp
 * @brief Punto de entrada del programa Productor-Consumidor.
 *
 * ─────────────────────────────────────────────────────────────────────────────
 *  DESCRIPCIÓN DEL ALGORITMO
 * ─────────────────────────────────────────────────────────────────────────────
 *  Este programa implementa el clásico problema de Productor-Consumidor con
 *  las siguientes condiciones especiales:
 *
 *   • 1 hilo PRODUCTOR: lee números desde un archivo de texto y los deposita
 *     en un buffer compartido de tamaño finito.
 *
 *   • 3 hilos CONSUMIDOR (múltiplos de 3 si se desea escalar):
 *       - Consumidor 1: toma únicamente números PARES.
 *       - Consumidor 2: toma únicamente números IMPARES.
 *       - Consumidor 3: toma únicamente números PRIMOS.
 *
 *   • Cada consumidor muestra en tiempo real la suma acumulada de los
 *     números que ha consumido (animación de consola con colores ANSI).
 *
 * ─────────────────────────────────────────────────────────────────────────────
 *  MECANISMOS DE SINCRONIZACIÓN
 * ─────────────────────────────────────────────────────────────────────────────
 *  Se utilizan primitivas de la STL de C++:
 *   • std::mutex        → garantiza acceso exclusivo al buffer (sección crítica).
 *   • std::condition_variable → bloquea al productor si el buffer está lleno,
 *                                y bloquea a los consumidores si no hay datos
 *                                de su tipo disponibles.
 *   • std::thread       → cada actor (productor/consumidor) corre en su propio hilo.
 *
 * ─────────────────────────────────────────────────────────────────────────────
 *  COMPILACIÓN
 * ─────────────────────────────────────────────────────────────────────────────
 *   g++ -std=c++17 -pthread main.cpp src/Buffer.cpp src/Productor.cpp src/Consumidor.cpp -o productor_consumidor
 *
 * ─────────────────────────────────────────────────────────────────────────────
 *  EJECUCIÓN
 * ─────────────────────────────────────────────────────────────────────────────
 *   ./productor_consumidor
 *
 * @author  Grupo - Sistemas Operativos, Universidad Rafael Landívar
 */

#include <iostream>
#include <thread>
#include <vector>
#include "include/Buffer.h"
#include "include/Productor.h"
#include "include/Consumidor.h"

int main() {
    // ── Configuración ────────────────────────────────────────────────────────
    const int    CAPACIDAD_BUFFER = 10;          // Tamaño máximo del buffer
    const int    NUM_CONSUMIDORES = 3;           // Debe ser múltiplo de 3
    const std::string ARCHIVO     = "data/numeros.txt"; // Archivo de entrada

    // ── Cabecera de la animación ─────────────────────────────────────────────
    std::cout << "\033[1;37m";
    std::cout << "╔══════════════════════════════════════════════════════════╗\n";
    std::cout << "║         PROBLEMA PRODUCTOR - CONSUMIDOR                 ║\n";
    std::cout << "║         Universidad Rafael Landívar                     ║\n";
    std::cout << "║         Sistemas Operativos                             ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════╣\n";
    std::cout << "║  Buffer tamaño : " << CAPACIDAD_BUFFER
              << "                                       ║\n";
    std::cout << "║  Consumidores  : " << NUM_CONSUMIDORES
              << " (Pares | Impares | Primos)          ║\n";
    std::cout << "║  Archivo       : " << ARCHIVO
              << "               ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════╝\n";
    std::cout << "\033[0m\n";

    // ── Crea el buffer compartido ───────────────────────────────────────────
    Buffer buffer(CAPACIDAD_BUFFER);

    // ── Crea el objeto Productor ────────────────────────────────────────────────
    // El productor leerá números del archivo y los insertará en el buffer.
    Productor productor(buffer, ARCHIVO);

    // ── Crea objetos Consumidor ─────────────────────────────────────────────
    // tipo % 3 determina si es consumidor de pares (0), impares (1) o primos (2).
    // Esto permite escalar a 6, 9, 12... consumidores manteniendo los 3 tipos.
    std::vector<Consumidor> consumidores;
    consumidores.reserve(NUM_CONSUMIDORES);
    for (int i = 0; i < NUM_CONSUMIDORES; ++i) {
        consumidores.emplace_back(i + 1, i % 3, buffer);
    }
    // ── Creacion de Threads ─────────────────────────────────────────────────────────
    // Primero lanzamos los consumidores para que estén listos cuando lleguen datos
    std::vector<std::thread> hilosConsumidores;
    hilosConsumidores.reserve(NUM_CONSUMIDORES);
    for (auto& c : consumidores) {
        hilosConsumidores.emplace_back(std::ref(c));
    }

    // Lanzamos el hilo del productor
    std::thread hiloProductor(std::ref(productor));

    // ── Espera a que todos los threads terminen ────────────────────────────────
    // join() bloquea al main hasta que cada hilo finalice su ejecución.
    hiloProductor.join();

    for (auto& h : hilosConsumidores) {
        h.join();
    }

    // ── Resumen final ────────────────────────────────────────────────────────
    std::cout << "\n\033[1;37m";
    std::cout << "╔══════════════════════════════════════════════════════════╗\n";
    std::cout << "║       TODOS LOS HILOS FINALIZARON CORRECTAMENTE         ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════╝\n";
    std::cout << "\033[0m\n";

    return 0;
}
