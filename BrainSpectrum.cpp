#include "BrainSpectrum.hpp"

BrainSpectrum::BrainSpectrum() {
    // CONSTRUCTOR

    // Set the primitive type for drawing
    //this->spectrumLines.setPrimitiveType(sf::Lines);

    // create rectangle shapes for drawing the spectrum
    const float rectWidth = SPECTRUM_WIDTH/NUM_SPECTRUM_BINS;

    for (unsigned int i = 0; i < NUM_SPECTRUM_BINS; i++) {
        sf::RectangleShape* rect = new sf::RectangleShape( sf::Vector2f(rectWidth, SPECTRUM_HEIGHT*INITIAL_SIGNAL) );

        rect->setPosition( sf::Vector2f(i*rectWidth, 0.0) );
        rect->setFillColor(sf::Color::Red);
        rect->setOutlineThickness(0.0);

        this->spectrumShapes.push_back(rect);
    }
}


void BrainSpectrum::draw(sf::RenderWindow &window) {
    // Draw the spectrum analyzer
    for (std::size_t i = 0; i < this->spectrumShapes.size(); i++) {
        window.draw( *(this->spectrumShapes[i]) );
    }
    //window.draw(this->spectrumLines);
}


void BrainSpectrum::updateSpectrum(std::vector<double> frequencies, std::vector<double> magnitudes) {
    // Takes a zero-symmetric vector of frequencies and corresponding FFT magnitudes and constructs
    // the positive frequency side of the spectrum out of lines. The vectors are assumed to be
    // identical in size.

    std::size_t numPoints = magnitudes.size();
    assert( numPoints == 2*NUM_CHANNELS_ANALYZED );
    assert( frequencies.size() == numPoints );

    /*
    this->spectrumLines.clear();
    unsigned int channelIndex = 0;

    // Only process the data at positive frequencies
    for (std::size_t i = numPoints/2; i < numPoints; i++) {
        //std::cout << frequencies[i] << "  " << magnitudes[i] << std::endl;

        this->spectrumLines.append( sf::Vertex(sf::Vector2f(channelIndex, magnitudes[i]), sf::Color::White) );
        this->spectrumLines.append( sf::Vertex(sf::Vector2f(channelIndex, 0), sf::Color::Red) );

        channelIndex++;
    }

    //std::cout << "Vertices in spectrumLines: " << spectrumLines.getVertexCount() << std::endl;
    */

    std::vector<double> binChannels; // store the subvector of magnitudes corresponding to the current bin
    std::vector<double>::const_iterator startOfBin = magnitudes.begin()+numPoints/2; // Only process the data at positive frequencies

    std::vector<double> binSignals; // stores the signals of processed bins

    // Binning logic
    for (unsigned int i = 0; i < NUM_SPECTRUM_BINS; i++) {
        binChannels = std::vector<double>(startOfBin, startOfBin+CHANNELS_PER_BIN);
        startOfBin += CHANNELS_PER_BIN;

        switch (BINNING_LOGIC) {
            case MEDIAN: {
                // Take the median of binChannels (slightly obscure but fast implementation)
                std::nth_element( binChannels.begin(), binChannels.begin()+CHANNELS_PER_BIN/2, binChannels.end() );
                binSignals.push_back( binChannels[CHANNELS_PER_BIN/2] );
            }
            case MAXIMUM: {
                // Take the maximum of binChannels
                binSignals.push_back( *std::max_element(binChannels.begin(), binChannels.end()) );
            }
        }
    }

    // Update the drawn spectrum
    double maxBinSignal = *std::max_element( binSignals.begin(), binSignals.end() ); // Scale the drawn signal based on the maximum bin value
    const float rectWidth = SPECTRUM_WIDTH/NUM_SPECTRUM_BINS;

    for (unsigned int i = 0; i < NUM_SPECTRUM_BINS; i++) {
        this->spectrumShapes[i]->setSize( sf::Vector2f(rectWidth, binSignals[i]/maxBinSignal*SPECTRUM_HEIGHT) );
    }

    // Analyze the spectrum for strong lines
    // (TODO)
}
