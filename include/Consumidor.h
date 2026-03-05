#ifndef CONSUMIDOR_H
#define CONSUMIDOR_H

#include "Buffer.h"

/**
 * @file Consumidor.h
 * @brief Declaración del hilo Consumidor.
 *
 * Existen 3 tipos de consumidores:
 *   - Tipo 0: Consume únicamente números PARES.
 *   - Tipo 1: Consume únicamente números IMPARES.
 *   - Tipo 2: Consume únicamente números PRIMOS.
 *
 * Cada consumidor acumula la suma de los números que consume y la muestra
 * en pantalla en tiempo real.
 */
class Consumidor {
public:
    /**
     * @brief Constructor del Consumidor.
     * @param id     Identificador único del consumidor (para animación).
     * @param tipo   Tipo de números que consume: 0=pares, 1=impares, 2=primos.
     * @param buffer Referencia al buffer compartido.
     */
    Consumidor(int id, int tipo, Buffer& buffer);

    /**
     * @brief Función que ejecuta el hilo consumidor.
     *
     * Toma números del buffer mientras la producción no haya terminado
     * y existan elementos que coincidan con su tipo. Muestra en pantalla
     * cada número tomado y la suma acumulada.
     */
    void operator()();

private:
    int     id_;     ///< Identificador del consumidor
    int     tipo_;   ///< Tipo: 0=par, 1=impar, 2=primo
    Buffer& buffer_; ///< Referencia al buffer compartido

    /**
     * @brief Retorna el nombre del tipo de consumidor.
     */
    std::string nombreTipo() const;
};

#endif // CONSUMIDOR_H
