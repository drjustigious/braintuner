#ifndef BRAIN_SPECTRUM_HPP
#define BRAIN_SPECTRUM_HPP

#include <vector>
#include <cassert>
#include <iostream>

#include <SFML/Graphics.hpp>

//*******************************
// SPECTRUM ANALYZER COMPONENT
//*******************************

class BrainSpectrum {

    public:
        BrainSpectrum();
        void draw(sf::RenderWindow &window);
        void loadNewSpectrum(std::vector<double> frequencies, std::vector<double> magnitudes);
        void update(float dt);
        const unsigned int NUM_CHANNELS_ANALYZED = 4096; // number of analyzed positive-frequency channels (2.7 Hz resolution at 8192 channels)

    private:
        std::vector<sf::RectangleShape*> spectrumShapes;
        std::vector<double> binSignals;

        const unsigned int CHANNELS_PER_BIN = 16; // how many analyzed channels are contained per rendered channel
        const unsigned int NUM_SPECTRUM_BINS = NUM_CHANNELS_ANALYZED/CHANNELS_PER_BIN; // number of rendered channels

        const unsigned int SPECTRUM_WIDTH = 512; // drawn spectrum width in pixels
        const unsigned int SPECTRUM_HEIGHT = 512; // max drawn spectrum height in pixels
        const sf::Vector2f SPECTRUM_POSITION = sf::Vector2f(64, 64); // origin of the drawn spectrum

        const float INITIAL_SIGNAL = 0.2; // initial signal level (for animations), range 0...1
        const float RENDER_FILTER_TIME_CONSTANT = 10.0; // natural time constant of rendered signal's filter, in units of frame update time

        enum BinningLogic {MEDIAN, MAXIMUM}; // different options for binning logic
        BinningLogic BINNING_LOGIC = MAXIMUM;

        double minAnalyzedFrequency = 50;
        double maxAnalyzedFrequency = 18000;

        sf::Color ACTIVE_SPECTRUM_COLOR = sf::Color(255,255,255,255); // color of analyzed spectral region
        sf::Color INACTIVE_SPECTRUM_COLOR = sf::Color(192,192,192,255); // color of spectral regions left out of analysis


};


#endif /* BRAIN_SPECTRUM_HPP */
