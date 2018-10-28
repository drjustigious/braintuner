#ifndef BRAIN_SPECTRUM_HPP
#define BRAIN_SPECTRUM_HPP

#include <vector>
#include <cassert>
#include <iostream>
#include <cmath>

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
        const unsigned int NUM_CHANNELS_ANALYZED = 8192; // number of analyzed positive-frequency channels (2.7 Hz resolution at 8192 channels)

    private:
        std::vector<sf::RectangleShape*> spectrumShapes;
        std::vector<sf::RectangleShape*> noteShapes;
        sf::RectangleShape noiseIndicatorShape;
        sf::RectangleShape spectrumContainerShape;
        std::vector<double> binSignals;

        const unsigned int CHANNELS_PER_BIN = 16; // how many analyzed channels are contained per rendered channel
        const unsigned int NUM_SPECTRUM_BINS = NUM_CHANNELS_ANALYZED/CHANNELS_PER_BIN; // number of rendered channels

        const unsigned int SPECTRUM_WIDTH = 512; // drawn spectrum width in pixels
        const unsigned int SPECTRUM_HEIGHT = 256; // max drawn spectrum height in pixels
        const sf::Vector2f SPECTRUM_POSITION = sf::Vector2f(16, 32); // origin of the drawn spectrum
        const int NOISE_LEVEL_OVERSHOOT = 2; // how many pixels the noise level indicator extends to each side of the spectrum
        const sf::Color NOISE_INDICATOR_COLOR = sf::Color(255,63,63,192);

        const float INITIAL_SIGNAL = 0.2; // initial signal level (for animations), range 0...1
        const float SPECTRUM_RENDER_FILTER_TIME_CONSTANT = 15.0; // natural time constant of rendered signal's filter, in units of frame update time
        const float NOISE_RENDER_FILTER_TIME_CONSTANT = 30.0; // natural time constant of rendered signal's filter, in units of frame update time

        enum BinningLogic {MEDIAN, MAXIMUM}; // different options for binning logic
        const BinningLogic BINNING_LOGIC = MAXIMUM;

        double minAnalyzedFrequency = 20;
        double maxAnalyzedFrequency = 24000;
        double maxLoadedFrequency = 22050;

        const sf::Color ACTIVE_SPECTRUM_COLOR = sf::Color(255,255,255,255); // color of analyzed spectral region
        const sf::Color INACTIVE_SPECTRUM_COLOR = sf::Color(192,192,192,255); // color of spectral regions left out of analysis

        const unsigned int ANALYSIS_PERIOD = 0; // how many spectrum loads to skip before doing a new analysis, default 4
        unsigned int analysisPeriodCounter = 0;
        std::vector<double> analyzedSignal;
        std::vector<double> analyzedFrequencies;
        int analysisMaskRadius = 2; // disregard signal at this radius from each found maximum (assumed to belong to the same peak)
        double noiseLevel = 100.0; // will be dynamically updated to approximate the noise ceiling

        static const unsigned int NUM_ANALYZED_NOTES = 4;
        double noteFrequencies[NUM_ANALYZED_NOTES];
        double noteIntensities[NUM_ANALYZED_NOTES];
        const float NOTE_INDICATOR_WIDTH = 2.0;
        const float NOTE_INDICATOR_HANG = 2; // distance between the bottoms of note indicators and the spectrum bottom
        const float NOTE_INDICATOR_HEIGHT = SPECTRUM_HEIGHT+NOTE_INDICATOR_HANG;
        const sf::Color NOTE_INDICATOR_COLOR = sf::Color(63,127,255,255);
        const float NOTE_INDICATOR_FILTER_TIME_CONSTANT = 15.0; // natural time constant for note indicator motion
        double minimumNoteSNR = 30; // minimum signal-to-noise ratio at which notes are registered

        const unsigned int NOISE_QUANTILE = 16; // let this be N, then the noise level will be determined as the first N-quantile of the signal vector...
        double noiseMultiplier = 2.5;      // ...multiplied by this constant

        const float SPECTRUM_CONTAINER_PAD = 8;
        const float CONTAINER_OUTLINE_THICKNESS = 2;
        const sf::Color CONTAINER_FILL_COLOR = sf::Color(0,0,0,255);
        const sf::Color CONTAINER_OUTLINE_COLOR = sf::Color(96,128,160,255);
};


#endif /* BRAIN_SPECTRUM_HPP */
