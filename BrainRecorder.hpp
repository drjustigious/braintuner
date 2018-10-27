#ifndef BRAIN_RECORDER_HPP
#define BRAIN_RECORDER_HPP

#include <iostream>
#include <vector>
#include <complex>
#include <cmath>

#include <SFML/Audio.hpp>
#include <SFML/System.hpp>

#include "FFT.hpp"

typedef std::complex<double> Complex;


//*******************************
// SOUND RECORDER COMPONENT
//*******************************

class BrainRecorder : public sf::SoundRecorder {
    public:
        bool onProcessSamples(const sf::Int16* samples, std::size_t sampleCount);
        void initialize();
        void clearBuffer();
        unsigned int getBufferSize();
        std::vector<Complex> getComplexBuffer(std::size_t numSamples);
        std::vector<double> getPowerSpectrum(std::size_t numSamples);
        std::vector<double> getFrequencies(std::size_t numSamples);

    private:
        std::vector<sf::Int16> buffer;
        std::size_t bufferSize;
        sf::Mutex recorderMutex; // Used to prevent concurrent access to the audio record buffer
};

#endif /* BRAIN_RECORDER_HPP */
