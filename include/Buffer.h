#ifndef BUFFER_H
#define BUFFER_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdexcept>

/**
 * @file Buffer.h
 * @brief Buffer compartido de tamaño finito (Bounded Buffer).
 *
 * Esta clase implementa el buffer circular del problema Productor-Consumidor.
 * Utiliza mutex y variables de condición para garantizar acceso seguro en
 * entornos multihilo, evitando condiciones de carrera e interbloqueo.
 */
class Buffer {
public:
    /**
     * @brief Constructor del buffer.
     * @param capacidad Número máximo de elementos que puede contener el buffer.
     */
    explicit Buffer(int capacidad);

    /**
     * @brief El productor inserta un número en el buffer.
     *
     * Bloquea si el buffer está lleno y espera hasta que haya espacio.
     * Utiliza un mutex para garantizar acceso exclusivo a la sección crítica.
     *
     * @param valor Número a insertar en el buffer.
     */
    void producir(int valor);

    /**
     * @brief Un consumidor intenta tomar un número del buffer según su tipo.
     *
     * Revisa si hay algún número que cumpla con el criterio del consumidor.
     * Si no hay ninguno disponible, el hilo espera.
     *
     * @param tipo Tipo de consumidor: 0=pares, 1=impares, 2=primos.
     * @param valor [out] El número tomado del buffer.
     * @return true si se tomó un número, false si el buffer está vacío y terminó la producción.
     */
    bool consumir(int tipo, int& valor);

    /**
     * @brief Indica al buffer que el productor terminó de insertar números.
     *
     * Notifica a todos los consumidores para que salgan de espera si ya no
     * quedan elementos que procesar.
     */
    void finalizarProduccion();

    /**
     * @brief Retorna cuántos elementos hay actualmente en el buffer.
     */
    int size() const;

    /**
     * @brief Retorna la capacidad máxima del buffer.
     */
    int capacidad() const;

private:
    std::queue<int>      buffer_;       ///< Cola de números compartida
    int                  capacidad_;    ///< Tamaño máximo del buffer
    bool                 produccionFinalizada_; ///< Señal de fin de producción

    mutable std::mutex           mtx_;          ///< Mutex para la sección crítica
    std::condition_variable      noLleno_;      ///< Condición: buffer no lleno (para productor)
    std::condition_variable      noVacio_;      ///< Condición: hay datos (para consumidores)

    /**
     * @brief Verifica si un número es primo.
     * @param n Número a verificar.
     * @return true si es primo, false en caso contrario.
     */
    static bool esPrimo(int n);

    /**
     * @brief Verifica si en la cola hay al menos un elemento del tipo solicitado.
     * @param tipo Tipo de consumidor.
     * @return true si existe al menos un elemento compatible.
     */
    bool hayElementoParaTipo(int tipo) const;
};

#endif // BUFFER_H
