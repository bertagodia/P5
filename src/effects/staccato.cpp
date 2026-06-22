#include <iostream>
#include <math.h>
#include "staccato.h"
#include "keyvalue.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

static float SamplingRate = 44100;

Staccato::Staccato(const std::string &param) {
  // Contamos cuántas muestras llevamos reproduciendo de la nota actual
  muestras_reproducidas = 0;

  KeyValue kv(param);

  // 'factor' es el porcentaje de la nota que sí va a sonar (de 0.0 a 1.0)
  // Por defecto 0.4 significa que solo suena el primer 40% de la nota, el resto es silencio
  if (!kv.to_float("factor", factor))
    factor = 0.4; 

  // 'duracion_nota' nos dice cuánto dura la nota entera en segundos (vía archivo .sco)
  if (!kv.to_float("dur", duracion_nota))
    duracion_nota = 1.0; // valor por defecto de 1 segundo

  // Calculamos exactamente en qué muestra se debe cortar el sonido
  muestra_corte = duracion_nota * SamplingRate * factor;
}

// Cuando entra una nueva nota (comando), reiniciamos el contador de tiempo a cero
void Staccato::command(unsigned int comm) {
  if (comm == 1) {
    muestras_reproducidas = 0;
  }
}

// El operador modifica el buffer x que le pasa el sintetizador
void Staccato::operator()(std::vector<float> &x) {
  for (unsigned int i = 0; i < x.size(); i++) {
    
    // Si ya hemos superado el tiempo permitido para el staccato...
    if (muestras_reproducidas >= muestra_corte) {
      x[i] = 0.0; // ...hacemos un corte radical aplicando silencio
    }
    // Si no, la muestra pasa intacta (multiplicada por 1)
    
    muestras_reproducidas++;
  }
}