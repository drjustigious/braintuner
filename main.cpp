#include <iostream>
#include <vector>
#include <complex>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "BrainConfig.hpp"
#include "BrainRecorder.hpp"
#include "BrainSpectrum.hpp"
#include "FFT.hpp"


int main()
{
    // Initialize SFML GUI
    sf::RenderWindow window(sf::VideoMode(BrainConfig::INITIAL_WINDOW_WIDTH, BrainConfig::INITIAL_WINDOW_HEIGHT), "FFT works!");
    window.setFramerateLimit(BrainConfig::FRAME_RATE);

    if (!BrainRecorder::isAvailable()) {
        // error: audio capture is not available on this system
        std::cout << "No audio recording device was found. Please plug one in and restart Brain Tuner." << std::endl;
        return 0;
    }
    else {
        std::cout << "Initializing audio recording device..." << std::endl;
    }

    // Test a custom recorder
    BrainRecorder recorder;
    recorder.initialize();

    BrainSpectrum spectrum;
    std::vector<double> powerSpectrum, frequencies;
    std::size_t bufferSize;

    // Run SFML GUI
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        spectrum.update(1.0/(float)BrainConfig::FRAME_RATE);

        bufferSize = recorder.getBufferSize();
        if (bufferSize > 2*spectrum.NUM_CHANNELS_ANALYZED) {
            if ( bufferSize > 2*recorder.getSampleRate() ) {
                std::cout << "Recorder buffer size too large: " << recorder.getBufferSize() << std::endl;
                recorder.clearBuffer();
            }
            else {
                powerSpectrum = recorder.getPowerSpectrum(2*spectrum.NUM_CHANNELS_ANALYZED);
                frequencies = recorder.getFrequencies( powerSpectrum.size() );
                spectrum.loadNewSpectrum(frequencies, powerSpectrum);
            }
        }

        window.clear();
        spectrum.draw(window);
        window.display();
    }

    recorder.stop();

    /*
    std::cout << "Record buffer size: " << recorder.getBufferSize() << std::endl;

    std::vector<double> powerSpectrum = recorder.getPowerSpectrum(4096);
    std::vector<double> frequencies = recorder.getFrequencies( powerSpectrum.size() );
    std::cout << "FFT of recording:" << std::endl;

    BrainSpectrum spectrum;
    spectrum.updateSpectrum(frequencies, powerSpectrum);
    */

    return 0;
}
