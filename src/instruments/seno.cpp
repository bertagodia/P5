#include <iostream>
#include <math.h>
#include "seno.h"
#include "keyvalue.h"
#include <stdlib.h>

using namespace upc;
using namespace std;

// Constructor: Inicializa la tabla de ondas con un ciclo de seno
Seno::Seno(const std::string &param) 
  : adsr(SamplingRate, param) {
  bActive = false;
  x.resize(BSIZE);

  KeyValue kv(param);
  int N;

  if (!kv.to_int("N", N)) 
    N = 40; // Valor por defecto si no encuentra N
  
  tbl.resize(N);
  float phase_init = 0, step_init = 2 * M_PI / (float)N;
  
  for (int i = 0; i < N; ++i) {
    tbl[i] = sin(phase_init);
    phase_init += step_init;
  }
  phase = 0;
}

// Gestión de eventos MIDI (pulsar/soltar tecla)
void Seno::command(long cmd, long note, long vel) {
  if (cmd == 9) {   // Key pressed: inicia el ataque
    bActive = true;
    adsr.start();
    A = vel / 127.0;
    // Cálculo del incremento de fase posicional según la frecuencia de la nota
    this->step = 440 * pow(2, (note - 69) / 12.0) * tbl.size() / SamplingRate;
  }
  else if (cmd == 8) {  // Key released: inicia el release
    adsr.stop();
  }
  else if (cmd == 0) {  // Extinción inmediata
    adsr.end();
  }
}

// Generación de las muestras de audio
const vector<float> & Seno::synthesize() {
  if (not adsr.active()) {
    x.assign(x.size(), 0);
    bActive = false;
    return x;
  }
  else if (not bActive)
    return x;

  for (unsigned int i = 0; i < x.size(); ++i) {
    // Lectura de la tabla mediante el método de redondeo al entero más cercano
    x[i] = A * tbl[(int)(phase + 0.5)];
    phase += step;
    
    // Bucle circular para mantener la fase dentro de los límites de la tabla
    while (phase >= tbl.size() - 0.5) {
      phase -= tbl.size();
    }
  }
  
  adsr(x); // Aplica la envolvente ADSR al bloque de audio generado

  return x;
}