#include <iostream>
#include "delay.h"
#include "keyvalue.h"

using namespace upc;
using namespace std;

static float SamplingRate = 44100;

Delay::Delay(const std::string &param) {
  KeyValue kv(param);

  if (!kv.to_float("time", time))
    time = 0.3f; // Default delay time: 0.3 seconds

  if (!kv.to_float("feedback", feedback))
    feedback = 0.4f; // Default decay: 40% feedback

  if (!kv.to_float("mix", mix))
    mix = 0.4f; // Default mix: 40% wet, 60% dry

  // Sanity checks on parameters
  if (mix < 0.0f) mix = 0.0f;
  if (mix > 1.0f) mix = 1.0f;
  if (feedback < 0.0f) feedback = 0.0f;
  if (feedback >= 1.0f) feedback = 0.99f; // Prevent infinite runaway gain

  unsigned int buffer_size = (unsigned int)(time * SamplingRate);
  if (buffer_size < 1) buffer_size = 1;

  buffer.assign(buffer_size, 0.0f);
  write_ptr = 0;
}

void Delay::command(unsigned int comm) {
  if (comm == 1) {
    // Reset circular buffer to avoid leftover noise from previous notes
    buffer.assign(buffer.size(), 0.0f);
    write_ptr = 0;
  }
}

void Delay::operator()(std::vector<float> &x) {
  for (unsigned int i = 0; i < x.size(); i++) {
    float in_sample = x[i];
    float delayed_sample = buffer[write_ptr];

    // Compute the output sample as a mix of dry (original) and wet (delayed) signal
    float out_sample = (1.0f - mix) * in_sample + mix * delayed_sample;

    // Save current sample plus feedback of the delayed sound to the buffer
    buffer[write_ptr] = in_sample + feedback * delayed_sample;

    // Overwrite input sample with output
    x[i] = out_sample;

    // Move to next position in circular buffer
    write_ptr = (write_ptr + 1) % buffer.size();
  }
}
