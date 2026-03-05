#include "../include/Buffer.h"
#include <iostream>
#include <cmath>

/**
 * @file Buffer.cpp
 * @brief Implementación del buffer compartido (Bounded Buffer).
 *
 * El buffer usa:
 *  - Un std::mutex para proteger la sección crítica (inserción/extracción).
 *  - std::condition_variable noLleno_ para bloquear al productor si el buffer está lleno.
 *  - std::condition_variable noVacio_ para bloquear a consumidores si no hay datos de su tipo.
 */

Buffer::Buffer(int capacidad)
    : capacidad_(capacidad), produccionFinalizada_(false) {}

// ─────────────────────────────────────────────
//  Función del PRODUCTOR
// ─────────────────────────────────────────────
void Buffer::producir(int valor) {
    // Adquirimos el lock: inicio de sección crítica
    std::unique_lock<std::mutex> lock(mtx_);

    // Si el buffer está lleno, el productor espera (libera el lock mientras espera)
    noLleno_.wait(lock, [this]() {
        return static_cast<int>(buffer_.size()) < capacidad_;
    });

    // Insertamos el número en la cola compartida
    buffer_.push(valor);

    // Notificamos a TODOS los consumidores que hay un nuevo elemento disponible
    noVacio_.notify_all();
    // El lock se libera automáticamente al salir del scope
}

// ─────────────────────────────────────────────
//  Función de los CONSUMIDORES
// ─────────────────────────────────────────────
bool Buffer::consumir(int tipo, int& valor) {
    std::unique_lock<std::mutex> lock(mtx_);

    // Espera hasta que: haya un elemento compatible con este consumidor,
    // o la producción haya terminado y no haya elementos compatibles para este tipo.
    noVacio_.wait(lock, [this, tipo]() {
        return hayElementoParaTipo(tipo) || (produccionFinalizada_ && !hayElementoParaTipo(tipo));
    });

    // Si la producción terminó y no quedan elementos compatibles, salimos
    if (produccionFinalizada_ && !hayElementoParaTipo(tipo)) {
        return false;
    }

    // Buscamos el primer elemento del buffer que cumpla con el tipo del consumidor
    // Nota: std::queue no permite iteración directa; usamos una cola temporal.
    std::queue<int> temp;
    bool encontrado = false;

    while (!buffer_.empty()) {
        int front = buffer_.front();
        buffer_.pop();

        if (!encontrado) {
            // Evaluamos si este elemento es para este consumidor.
            // NOTA: Los primos (excepto 2) son también impares, por lo que
            // el consumidor de impares (tipo 1) NO debe tomar un primo,
            // ya que ese número le corresponde al consumidor de primos (tipo 2).
            bool esCompatible = false;
            if (tipo == 0 && front % 2 == 0 && !esPrimo(front))  esCompatible = true; // pares no primos
            else if (tipo == 0 && front == 2)                     esCompatible = false; // 2 es primo, no lo toma pares
            else if (tipo == 1 && front % 2 != 0 && !esPrimo(front)) esCompatible = true; // impares no primos
            else if (tipo == 2 && esPrimo(front))                 esCompatible = true; // primos (incluye 2)

            if (esCompatible) {
                valor = front;
                encontrado = true;
                // El resto de elementos regresa al buffer
                continue;
            }
        }
        temp.push(front); // Reinsertamos los que no tomamos
    }

    // Reconstruimos el buffer con los elementos que no fueron tomados
    buffer_ = temp;

    // Si se tomó un elemento, notificamos al productor que hay espacio
    if (encontrado) {
        noLleno_.notify_one();
    } else {
        // No encontramos elemento compatible; notificamos igualmente para
        // que otros consumidores o el productor no queden bloqueados
        noVacio_.notify_all();
    }

    return encontrado;
}

// ─────────────────────────────────────────────
//  Señal de fin de producción
// ─────────────────────────────────────────────
void Buffer::finalizarProduccion() {
    std::lock_guard<std::mutex> lock(mtx_);
    produccionFinalizada_ = true;
    // Despertamos a todos los consumidores para que evalúen la condición de salida
    noVacio_.notify_all();
}

// ─────────────────────────────────────────────
//  Consultas de estado
// ─────────────────────────────────────────────
int Buffer::size() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return static_cast<int>(buffer_.size());
}

int Buffer::capacidad() const {
    return capacidad_;
}

// ─────────────────────────────────────────────
//  Funciones auxiliares privadas
// ─────────────────────────────────────────────

/**
 * @brief Verifica si n es primo.
 * Un número es primo si es mayor que 1 y no tiene divisores entre 2 y sqrt(n).
 */
bool Buffer::esPrimo(int n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (int i = 3; i <= static_cast<int>(std::sqrt(n)); i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}

/**
 * @brief Recorre la cola y verifica si algún elemento es compatible con el tipo dado.
 *
 * Esta función se llama dentro de la condición de espera del consumidor,
 * por lo que se ejecuta con el lock ya adquirido.
 */
bool Buffer::hayElementoParaTipo(int tipo) const {
    std::queue<int> temp = buffer_; // Copia para no modificar el buffer original
    while (!temp.empty()) {
        int val = temp.front();
        temp.pop();
        if (tipo == 0 && val % 2 == 0 && !esPrimo(val)) return true; // pares no primos
        if (tipo == 1 && val % 2 != 0 && !esPrimo(val)) return true; // impares no primos
        if (tipo == 2 && esPrimo(val))                  return true; // primos
    }
    return false;
}
