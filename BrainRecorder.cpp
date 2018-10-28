//#include <iostream>
//#include <SFML/Audio.hpp>
//#include <SFML/System.hpp>

#include "BrainRecorder.hpp"


bool BrainRecorder::onProcessSamples(const sf::Int16* samples, std::size_t sampleCount) {
    // Process an incoming chunk of audio samples
    sf::Lock lock(this->recorderMutex);  // prevent concurrent access to recording buffer until the function returns

    for (std::size_t i=0; i < sampleCount; i++) {
        this->buffer.push_back( *(samples+i) );
    }

    this->bufferSize = this->buffer.size(); // store buffer size so that other threads can read it

    //std::cout << sampleCount << " samples recorded" << std::endl;
    return true;
}


void BrainRecorder::initialize() {
    // Start up the recorder in mono-channel mode

    this->setChannelCount(1);
    this->start();

    std::cout << "Recording at " << this->getSampleRate() << " Hz" << std::endl;
}


void BrainRecorder::clearBuffer() {
    // Clear the record buffer

    this->buffer.clear();

    std::cout << "Record buffer cleared" << std::endl;
}


unsigned int BrainRecorder::getBufferSize() {
    // Return the number of samples in the audio buffer
    return this->bufferSize;
}


std::vector<Complex> BrainRecorder::getComplexBuffer(std::size_t numSamples) {
    // Grab (up to) the given number of Int16 samples from the beginning of the buffer and
    // return them as a vector of complex values

    std::vector<Complex> complexBuffer;
    sf::Lock lock(this->recorderMutex); // prevent concurrent access to recording buffer until the function returns

    // be careful not to iterate out of the buffer
    std::size_t numIterableSamples = std::min( numSamples, this->buffer.size() );

    for (std::size_t i = 0; i < numIterableSamples; i++) {
        complexBuffer.push_back( (Complex)this->buffer[i] );
    }

    // remove the processed samples from the beginning of the buffer
    this->buffer.erase (this->buffer.begin(),this->buffer.begin()+numIterableSamples);

     // store buffer size so that other threads can read it
    this->bufferSize = this->buffer.size();

    return complexBuffer;
}



std::vector<double> BrainRecorder::getPowerSpectrum(std::size_t numSamples) {
    // Take the given number of samples off the beginning of the buffer and return the corresponding
    // frequency-domain power spectrum

    std::vector<double> powerSpectrum;
    std::vector<Complex> complexBuffer = this->getComplexBuffer(numSamples);
    double magnitude;

    fft(complexBuffer);

    for (size_t i = 0; i < complexBuffer.size(); i++) {
        magnitude = std::abs(complexBuffer[i]);

        switch (SPECTRUM_SCALING) {
            case LINEAR: {
                powerSpectrum.push_back( magnitude );
                break;
            }
            case SQRT: {
                powerSpectrum.push_back( sqrt(magnitude) );
                break;
            }
            case LOG: {
                powerSpectrum.push_back( log10(magnitude) );
                break;
            }
        }
    }

    return powerSpectrum;
}



std::vector<double> BrainRecorder::getFrequencies(std::size_t numSamples) {
    // returns the vector of frequencies corresponding to a frequency spectrum of length numSamples

    std::vector<double> frequencies;
    unsigned int sampleRate = this->getSampleRate();
    double scalingFactor = sampleRate / (double)numSamples;

    for (size_t i = 0; i < numSamples; i++) {
        frequencies.push_back(sampleRate - i*scalingFactor);
    }

    return frequencies;
}
