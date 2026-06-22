#ifndef FMSYNTH_H
#define FMSYNTH_H

#include "instrument.h"
#include <vector>
#include <string>
#include "envelope_adsr.h"

namespace upc {
  class FMSynth : public upc::Instrument {
  private:
  EnvelopeADSR adsr;
    float N1, N2, I_semitonos;
    float fase_car, fase_mod;
    float A, f0;
    static float SamplingRate;

  public:
    FMSynth(const std::string &param);
    
    // TIENE QUE QUEDAR ASÍ EN EL .H:
    void command(long cmd, long note, long velocity = 100) override;
    
    const std::vector<float> & synthesize() override;
  };
}

#endif