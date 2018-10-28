#include <iostream>
#include <vector>
#include <complex>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "BrainConfig.hpp"
#include "BrainRecorder.hpp"
#include "BrainSpectrum.hpp"
#include "FFT.hpp"

const sf::Color BACKGROUND_COLOR = sf::Color(16,24,32,255);


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
            switch (event.type) {
                case sf::Event::Closed: {
                    // The "close window" button was clicked.
                    window.close();
                    break;
                }

                case sf::Event::Resized: {
                    // The window was resized. Enforce 4:3 aspect ratio and minimum size.
                    unsigned int newHeight = std::max(event.size.height, BrainConfig::MINIMUM_WINDOW_HEIGHT);
                    window.setSize( sf::Vector2u(newHeight*4/3, newHeight) );
                    break;
                }

                case sf::Event::MouseWheelScrolled: {
                    // Mouse wheel was used for scrolling or zooming
                    if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                        if (spectrum.active) {
                            spectrum.handleScroll(event.mouseWheelScroll.x, event.mouseWheelScroll.y, window.getSize().x, window.getSize().y, event.mouseWheelScroll.delta);
                        }
                    }
                    break;
                }

                case sf::Event::MouseMoved: {
                    // Mouse was moved
                    if (spectrum.active) {
                        spectrum.handleMouseMove(event.mouseMove.x, event.mouseMove.y, window.getSize().x, window.getSize().y);
                    }
                    break;
                }

                default: {
                    break;
                }
            }
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

        window.clear(BACKGROUND_COLOR);
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
