#include <iostream>
#include <math.h>
#include "FMSynth.h"
#include "keyvalue.h"
#include <stdlib.h>

using namespace upc;
using namespace std;

float FMSynth::SamplingRate = 44100;

// Constructor: Inicializa el EnvelopeADSR de la UPC
FMSynth::FMSynth(const std::string &param) 
  : adsr(SamplingRate, param) { //El objeto EnvelopeADSR de PAV suele inicializarse solo con param o usando métodos heredados
  bActive = false;
  
  // Usamos el método heredado de Instrument para asegurar el tamaño de x
  x.resize(BSIZE); 

  KeyValue kv(param);

  if (!kv.to_float("N1", N1)) N1 = 1.0;
  if (!kv.to_float("N2", N2)) N2 = 1.0;
  if (!kv.to_float("I", I_semitonos)) I_semitonos = 0.0; 

  A = 0.0;
  f0 = 0.0;
  fase_car = 0.0;
  fase_mod = 0.0;
}

// Gestión de eventos MIDI calcado al comportamiento de Seno
void FMSynth::command(long cmd, long note, long vel) {
  if (cmd == 9) {   // NOTE ON
    bActive = true;
    adsr.start();   // Activa la envolvente tal cual lo hace Seno
    
    f0 = 440.0 * pow(2.0, (note - 69.0) / 12.0); 
    A = vel / 127.0;
    
    fase_car = 0.0;
    fase_mod = 0.0;
  }
  else if (cmd == 8) {  // NOTE OFF
    adsr.stop();
  }
  else if (cmd == 0) {  // Extinción inmediata
    adsr.end();
  }
}

// Generación de las muestras de audio
const vector<float> & FMSynth::synthesize() {
  // 1. Si la envolvente ya no está activa, silenciamos el bloque y apagamos el instrumento
  if (not adsr.active()) {
    x.assign(x.size(), 0);
    bActive = false;
    return x;
  }
  else if (not bActive) {
    return x;
  }

  float fc = N1 * f0;
  float fm = N2 * f0;
  
  // Índice de modulación máximo (convertido de semitonos a desviación de frecuencia)
  float delta_f = fc * (pow(2.0, I_semitonos / 12.0) - 1.0);
  float d = (fm != 0.0) ? (delta_f / fm) : 0.0;

  float inc_fase_c = 2.0 * M_PI * fc / SamplingRate;
  float inc_fase_m = 2.0 * M_PI * fm / SamplingRate;

  // 2. Generamos la señal FM pura escalada por la velocidad MIDI (A)
  for (unsigned int i = 0; i < x.size(); ++i) {
    // Generamos la forma de onda base
    x[i] = A * sin(fase_car + d * sin(fase_mod));

    fase_car += inc_fase_c;
    fase_mod += inc_fase_m;
    
    while (fase_car >= 2.0 * M_PI) fase_car -= 2.0 * M_PI;
    while (fase_mod >= 2.0 * M_PI) fase_mod -= 2.0 * M_PI;
  }
  
  // 3. ¡CRÍTICO!: Aplicamos la envolvente ADSR sobre el bloque generado. 
  // Esta llamada modifica el vector 'x' multiplicándolo por los coeficientes de la curva ADSR (Atack, Decay, Sustain, Release)
  adsr(x); 

  return x;
}