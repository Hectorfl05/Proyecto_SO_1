#ifndef PRODUCTOR_H
#define PRODUCTOR_H

#include <string>
#include "Buffer.h"

/**
 * @file Productor.h
 * @brief Declaración del hilo Productor.
 *
 * El productor lee números de un archivo de texto y los deposita en el
 * buffer compartido. Si el buffer está lleno, espera hasta que algún
 * consumidor libere espacio.
 */
class Productor {
public:
    /**
     * @brief Constructor del Productor.
     * @param buffer   Referencia al buffer compartido.
     * @param archivo  Ruta al archivo de texto con los números a leer.
     */
    Productor(Buffer& buffer, const std::string& archivo);

    /**
     * @brief Función que ejecuta el hilo productor.
     *
     * Lee cada número del archivo e intenta insertarlo en el buffer.
     * Al terminar, notifica al buffer que la producción finalizó.
     */
    void operator()();

private:
    Buffer&     buffer_;   ///< Referencia al buffer compartido
    std::string archivo_;  ///< Ruta del archivo de entrada
};

#endif // PRODUCTOR_H
