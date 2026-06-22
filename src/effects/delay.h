#ifndef DELAY_H
#define DELAY_H

#include <vector>
#include <string>
#include "effect.h"

namespace upc {
  class Delay : public upc::Effect {
    private:
      std::vector<float> buffer;
      unsigned int write_ptr;
      float time, feedback, mix;
      
    public:
      Delay(const std::string &param = "");
      void operator()(std::vector<float> &x) override;
      void command(unsigned int comm) override;
  };
}

#endif
