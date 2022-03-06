#include "Arduino.h"
#include <math.h>
#include <stdlib.h>

const float PI_VALUE = 3.14159265358979323846;

struct equalizer_t
{
    int samples_in[2];
    int samples_out[2];
    int sample_rate;
    float frequency;
    float gain;
    float gain_reference;
    float band_frequency;
    float gain_band_frequency;
    float feedback_coeff_0;
    float feedback_coeff_1;
    float feedback_coeff_2;
    float feedforward_coeff_0;
    float feedforward_coeff_1;
    float feedforward_coeff_2;

    void setup()
    {
        float beta = tan(band_frequency / 2.0 * PI_VALUE / (sample_rate / 2.0)) *
                     sqrt(abs(pow(pow(10, gain_band_frequency / 20.0), 2.0) - pow(pow(10.0, gain_reference / 20.0), 2.0))) /
                     sqrt(abs(pow(pow(10.0, gain / 20.0), 2.0) - pow(pow(10.0, gain_band_frequency / 20.0), 2.0)));

        feedback_coeff_0 = 1.0;
        feedback_coeff_1 = -2.0 * cos(frequency * PI_VALUE / (sample_rate / 2.0)) / (1.0 + beta);
        feedback_coeff_2 = (1.0 - beta) / (1.0 + beta);

        feedforward_coeff_0 = (pow(10.0, gain_reference / 20.0) + pow(10.0, gain / 20.0) * beta) /
                              (1.0 + beta);
        feedforward_coeff_1 = (-2 * pow(10.0, gain_reference / 20.0) * cos(frequency * PI_VALUE / (sample_rate / 2.0))) /
                              (1.0 + beta);
        feedforward_coeff_2 = (pow(10.0, gain_reference / 20.0) - pow(10.0, gain / 20.0) * beta) /
                              (1.0 + beta);
    }

    int filter(float sample_in)
    {
        float sample_out = (feedforward_coeff_0 * sample_in) +
                         (feedforward_coeff_1 * samples_in[1]) +
                         (feedforward_coeff_2 * samples_in[0]) -
                         (feedback_coeff_1 * samples_out[1]) -
                         (feedback_coeff_2 * samples_out[0]);

        sample_out = sample_out / feedback_coeff_0;

        samples_out[0] = samples_out[1];
        samples_in[0] = samples_in[1];

        samples_in[1] = sample_in;
        samples_out[1] = sample_out;

        return sample_out;
    }
};

equalizer_t equalizer = {
    .samples_in = {0},
    .samples_out = {0},
    .sample_rate = 44100,
    .frequency = 10000,
    .gain = 15,
    .gain_reference = 0,
    .band_frequency = 2500,
    .gain_band_frequency = 9,
};
