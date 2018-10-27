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
        void updateSpectrum(std::vector<double> frequencies, std::vector<double> magnitudes);
        const unsigned int NUM_CHANNELS_ANALYZED = 4096; // number of analyzed positive-frequency channels (2.7 Hz resolution at 8192 channels)

    private:
        sf::VertexArray spectrumLines;
        std::vector<sf::RectangleShape*> spectrumShapes;

        const unsigned int CHANNELS_PER_BIN = 16; // how many analyzed channels are contained per rendered channel
        const unsigned int NUM_SPECTRUM_BINS = NUM_CHANNELS_ANALYZED/CHANNELS_PER_BIN; // number of rendered channels

        const unsigned int SPECTRUM_WIDTH = 512; // drawn spectrum width in pixels
        const unsigned int SPECTRUM_HEIGHT = 512; // max drawn spectrum height in pixels

        const float INITIAL_SIGNAL = 0.2; // initial signal level (for animations), range 0...1
        const float RENDER_FILTER_TIME_CONSTANT = 2.0; // natural time constant of rendered signal's filter

        enum BinningLogic {MEDIAN, MAXIMUM}; // different options for binning logic
        BinningLogic BINNING_LOGIC = MAXIMUM;


};


#endif /* BRAIN_SPECTRUM_HPP */
