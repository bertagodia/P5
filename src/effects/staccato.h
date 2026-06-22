#ifndef STACCATO_H
#define STACCATO_H

#include <vector>
#include <string>

namespace upc {
  class Staccato {
  private:
    float factor;
    float duracion_nota;
    unsigned long muestras_reproducidas;
    unsigned long muestra_corte;

  public:
    Staccato(const std::string &param);
    void command(unsigned int comm);
    void operator()(std::vector<float> &x);
  };
}

#endif