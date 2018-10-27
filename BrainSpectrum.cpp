#include "BrainSpectrum.hpp"

BrainSpectrum::BrainSpectrum() {
    // CONSTRUCTOR

    // Set the primitive type for drawing
    //this->spectrumLines.setPrimitiveType(sf::Lines);

    // create rectangle shapes for drawing the spectrum
    const float rectWidth = SPECTRUM_WIDTH/NUM_SPECTRUM_BINS;

    for (unsigned int i = 0; i < NUM_SPECTRUM_BINS; i++) {
        sf::RectangleShape* rect = new sf::RectangleShape( sf::Vector2f(rectWidth, SPECTRUM_HEIGHT*INITIAL_SIGNAL) );

        rect->setPosition( SPECTRUM_POSITION+sf::Vector2f(SPECTRUM_WIDTH-(i+1)*rectWidth, SPECTRUM_HEIGHT*(1-INITIAL_SIGNAL)) );
        rect->setFillColor(sf::Color::Red);
        rect->setOutlineThickness(0.0);

        this->spectrumShapes.push_back(rect);
    }
}


void BrainSpectrum::update(float dt) {
    // Perform UI logic and advance animations

    // Update the drawn spectrum if data is available
    if (binSignals.size() > 0) {
        double maxBinSignal = *std::max_element( this->binSignals.begin(), this->binSignals.end() ); // Scale the drawn signal based on the maximum bin value
        const float rectWidth = SPECTRUM_WIDTH/NUM_SPECTRUM_BINS;
        float oldHeight, newHeight;

        for (unsigned int i = 0; i < NUM_SPECTRUM_BINS; i++) {
            // Apply first-order low pass filter to height
            oldHeight = this->spectrumShapes[i]->getSize().y;
            newHeight = oldHeight + (this->binSignals[i]/maxBinSignal*SPECTRUM_HEIGHT - oldHeight) / RENDER_FILTER_TIME_CONSTANT;
            this->spectrumShapes[i]->setSize( sf::Vector2f(rectWidth, newHeight) );

            // Keep spectrum at the bottom of the given window
            this->spectrumShapes[i]->setPosition( SPECTRUM_POSITION+sf::Vector2f(SPECTRUM_WIDTH-(i+1)*rectWidth, (SPECTRUM_HEIGHT-newHeight)) );
        }
    }
}


void BrainSpectrum::draw(sf::RenderWindow &window) {
    // Draw the spectrum analyzer
    for (std::size_t i = 0; i < this->spectrumShapes.size(); i++) {
        window.draw( *(this->spectrumShapes[i]) );
    }
    //window.draw(this->spectrumLines);
}


void BrainSpectrum::loadNewSpectrum(std::vector<double> frequencies, std::vector<double> magnitudes) {
    // Takes a zero-symmetric vector of frequencies and corresponding FFT magnitudes and constructs
    // the positive frequency side of the spectrum out of lines. The vectors are assumed to be
    // identical in size.

    std::size_t numPoints = magnitudes.size();
    assert( numPoints == 2*NUM_CHANNELS_ANALYZED );
    assert( frequencies.size() == numPoints );

    std::vector<double> binChannels; // store the subvector of magnitudes corresponding to the current bin
    std::vector<double>::const_iterator startOfBin = magnitudes.begin()+numPoints/2; // Only process the data at positive frequencies

    double binBottomFrequency;
    double binTopFrequency;

    this->binSignals.clear();

    // Binning logic
    for (unsigned int i = 0; i < NUM_SPECTRUM_BINS; i++) {
        binChannels = std::vector<double>(startOfBin, startOfBin+CHANNELS_PER_BIN);
        startOfBin += CHANNELS_PER_BIN;

        switch (BINNING_LOGIC) {
            case MEDIAN: {
                // Take the median of binChannels (slightly obscure but fast implementation)
                std::nth_element( binChannels.begin(), binChannels.begin()+CHANNELS_PER_BIN/2, binChannels.end() );
                this->binSignals.push_back( binChannels[CHANNELS_PER_BIN/2] );
            }
            case MAXIMUM: {
                // Take the maximum of binChannels
                this->binSignals.push_back( *std::max_element(binChannels.begin(), binChannels.end()) );
            }
        }

        // Update spectrum line colors to indicate the active analysis region
        binBottomFrequency = frequencies[numPoints/2 + i*CHANNELS_PER_BIN];
        binTopFrequency = frequencies[numPoints/2 + (i+1)*CHANNELS_PER_BIN-1];

        if (binTopFrequency >= minAnalyzedFrequency && binBottomFrequency <= maxAnalyzedFrequency)
            this->spectrumShapes[i]->setFillColor(ACTIVE_SPECTRUM_COLOR);
        else
            this->spectrumShapes[i]->setFillColor(INACTIVE_SPECTRUM_COLOR);
    }


    // Analyze the spectrum for strong lines
    this->analysisPeriodCounter++;
    if (this->analysisPeriodCounter > ANALYSIS_PERIOD) {
        this->analysisPeriodCounter = 0;

        std::cout << "Analyzing..." << std::endl;

        // Collect the actual signal portion to be analyzed
        this->analyzedSignal.clear();
        this->analyzedFrequencies.clear();
        for (size_t i = numPoints/2; i < numPoints; i++) {
            if (frequencies[i] >= minAnalyzedFrequency && frequencies[i] <= maxAnalyzedFrequency)
                this->analyzedSignal.push_back(magnitudes[i]);
                this->analyzedFrequencies.push_back(frequencies[i]);
        }

        // The above guarantees that this->analyzedSignal and this->analyzedFrequencies nowhave equal sizes
        // Scan for the N strongest peaks in the signal
        std::vector<double>::iterator strongestSignalElement;
        std::vector<double>::iterator firstRemovedElement;
        std::vector<double>::iterator lastRemovedElement;
        std::size_t firstRemovedIndex;
        std::size_t lastRemovedIndex;

        for (unsigned int i = 0; i < numAnalyzedNotes; i++) {
            // Find the current absolute maximum
            strongestSignalElement = std::max_element( this->analyzedSignal.begin(), this->analyzedSignal.end() );

            unsigned int strongestElementIndex = strongestSignalElement-this->analyzedSignal.begin();
            std::cout << "Strong index " << strongestElementIndex << std::endl;
            std::cout << "Strong signal at " << this->analyzedFrequencies[strongestElementIndex] << " Hz" << std::endl;

            // Disregard data close to the found maximum (may be from same signal peak)
            firstRemovedElement = strongestSignalElement-this->analysisMaskRadius;
            if (firstRemovedElement < this->analyzedSignal.begin())
                firstRemovedElement = this->analyzedSignal.begin();
                firstRemovedIndex = firstRemovedElement-this->analyzedSignal.begin();

            lastRemovedElement = strongestSignalElement+this->analysisMaskRadius;
            if (lastRemovedElement > this->analyzedSignal.end())
                lastRemovedElement = this->analyzedSignal.end();
                lastRemovedIndex = lastRemovedElement-this->analyzedSignal.begin();

            this->analyzedSignal.erase(firstRemovedElement, lastRemovedElement);
            this->analyzedFrequencies.erase(this->analyzedFrequencies.begin()+firstRemovedIndex, this->analyzedFrequencies.begin()+lastRemovedIndex);
        }
    }
}
