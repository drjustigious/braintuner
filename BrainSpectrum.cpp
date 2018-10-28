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
        rect->setFillColor(sf::Color::White);
        rect->setOutlineThickness(0.0);

        this->spectrumShapes.push_back(rect);
    }

    // create the noise level indicator
    this->noiseIndicatorShape.setFillColor( NOISE_INDICATOR_COLOR );
    this->noiseIndicatorShape.setOutlineThickness(0);
    this->noiseIndicatorShape.setSize( sf::Vector2f(SPECTRUM_WIDTH+2*NOISE_LEVEL_OVERSHOOT, 1) );
    this->noiseIndicatorShape.setPosition( SPECTRUM_POSITION+sf::Vector2f(-NOISE_LEVEL_OVERSHOOT, SPECTRUM_HEIGHT*(1-INITIAL_SIGNAL)) );

    // create the analyzed note indicators
    for (unsigned i = 0; i < NUM_ANALYZED_NOTES; i++) {
        sf::RectangleShape* rect = new sf::RectangleShape( sf::Vector2f(NOTE_INDICATOR_WIDTH, NOTE_INDICATOR_HEIGHT+NOTE_INDICATOR_HANG) );
        rect->setPosition( SPECTRUM_POSITION+sf::Vector2f(8+i*8, SPECTRUM_HEIGHT-NOTE_INDICATOR_HEIGHT) );
        rect->setFillColor(NOTE_INDICATOR_COLOR);
        rect->setOutlineThickness(0.0);
        this->noteShapes.push_back(rect);

        this->noteFrequencies[i] = -1.0;
        this->noteIntensities[i] = 0.0;
    }

    // configure the containers shapes
    this->spectrumContainerShape.setSize( sf::Vector2f(SPECTRUM_WIDTH+2*SPECTRUM_CONTAINER_PAD, SPECTRUM_HEIGHT+2*SPECTRUM_CONTAINER_PAD) );
    this->spectrumContainerShape.setPosition( SPECTRUM_POSITION - sf::Vector2f(SPECTRUM_CONTAINER_PAD, SPECTRUM_CONTAINER_PAD) );
    this->spectrumContainerShape.setOutlineThickness( CONTAINER_OUTLINE_THICKNESS );
    this->spectrumContainerShape.setFillColor( CONTAINER_FILL_COLOR );
    this->spectrumContainerShape.setOutlineColor( CONTAINER_OUTLINE_COLOR );

    // configure the zoom rectangle
    this->zoomRectangle.setSize( sf::Vector2f(SPECTRUM_WIDTH/2, SPECTRUM_HEIGHT) );
    this->zoomRectangle.setFillColor( ZOOM_FILL_COLOR );
    this->zoomRectangle.setOutlineThickness(0);
    this->zoomRectangle.setPosition(SPECTRUM_POSITION);

    // configure the spectrum cursor
    this->spectrumCursor.setSize( sf::Vector2f(CURSOR_WIDTH, SPECTRUM_HEIGHT) );
    this->spectrumCursor.setFillColor( CURSOR_COLOR );
    this->spectrumCursor.setOutlineThickness(0);
    this->spectrumCursor.setPosition(SPECTRUM_POSITION);
}


void BrainSpectrum::update(float dt) {
    // Perform UI logic and advance animations

    // Update the drawn spectrum if data is available
    if (binSignals.size() > 0) {
        double maxBinSignal = *std::max_element( this->binSignals.begin(), this->binSignals.end() ); // Scale the drawn signal based on the maximum bin value
        const float rectWidth = SPECTRUM_WIDTH/NUM_SPECTRUM_BINS;
        float oldHeight, newHeight;

        // Spectral lines
        for (unsigned int i = 0; i < NUM_SPECTRUM_BINS; i++) {
            // Apply first-order low pass filter to height
            oldHeight = this->spectrumShapes[i]->getSize().y;
            newHeight = oldHeight + (this->binSignals[i]/maxBinSignal*SPECTRUM_HEIGHT - oldHeight) / SPECTRUM_RENDER_FILTER_TIME_CONSTANT;
            this->spectrumShapes[i]->setSize( sf::Vector2f(rectWidth, newHeight) );

            // Keep spectrum at the bottom of the given window
            this->spectrumShapes[i]->setPosition( SPECTRUM_POSITION+sf::Vector2f(SPECTRUM_WIDTH-(i+1)*rectWidth, SPECTRUM_HEIGHT-newHeight) );
        }

        // Noise level indicator
        oldHeight = this->noiseIndicatorShape.getPosition().y;
        newHeight = oldHeight + (SPECTRUM_POSITION.y + SPECTRUM_HEIGHT-1-this->noiseLevel/maxBinSignal*SPECTRUM_HEIGHT - oldHeight) / NOISE_RENDER_FILTER_TIME_CONSTANT;
        //std::cout << "newHeight = " << newHeight << std::endl;
        this->noiseIndicatorShape.setPosition( sf::Vector2f(SPECTRUM_POSITION.x-NOISE_LEVEL_OVERSHOOT, newHeight) );
    }

    // Note indicators
    double oldX, newX;
    for (size_t i = 0; i < this->noteShapes.size(); i++) {
        oldX = this->noteShapes[i]->getPosition().x;
        newX = SPECTRUM_POSITION.x + this->noteFrequencies[i]/this->maxLoadedFrequency*SPECTRUM_WIDTH-NOTE_INDICATOR_WIDTH/2;
        newX = oldX + (newX-oldX)/NOTE_INDICATOR_FILTER_TIME_CONSTANT;
        this->noteShapes[i]->setPosition( sf::Vector2f(newX, SPECTRUM_POSITION.y+SPECTRUM_HEIGHT-NOTE_INDICATOR_HEIGHT) );
    }
}


void BrainSpectrum::draw(sf::RenderWindow &window) {
    // Draw the spectrum analyzer

    // Containers
    window.draw(this->spectrumContainerShape);

    // Zoom rectangle
    if (this->zoomRectangleVisible) {
        window.draw(this->zoomRectangle);
        window.draw(this->spectrumCursor);
    }

    // Analyzed note indicators
    for (std::size_t i = 0; i < this->noteShapes.size(); i++) {
        if (noteFrequencies[i] >= 0) // negative frequencies mean "note not found/do not draw"
            window.draw( *(this->noteShapes[i]) );
    }

    // Spectral lines
    for (std::size_t i = 0; i < this->spectrumShapes.size(); i++) {
        window.draw( *(this->spectrumShapes[i]) );
    }

    // Noise level indicator
    window.draw(this->noiseIndicatorShape);
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

    // Record the maximum frequency of the dataset (for graphics)
    this->maxLoadedFrequency = *std::max_element(frequencies.begin(), frequencies.end())/2;

    // Binning logic
    for (unsigned int i = 0; i < NUM_SPECTRUM_BINS; i++) {
        binChannels = std::vector<double>(startOfBin, startOfBin+CHANNELS_PER_BIN);
        startOfBin += CHANNELS_PER_BIN;

        switch (BINNING_LOGIC) {
            case MEDIAN: {
                // Take the median of binChannels (slightly obscure but fast implementation)
                std::nth_element( binChannels.begin(), binChannels.begin()+CHANNELS_PER_BIN/2, binChannels.end() );
                this->binSignals.push_back( binChannels[CHANNELS_PER_BIN/2] );
                break;
            }
            case MAXIMUM: {
                // Take the maximum of binChannels
                this->binSignals.push_back( *std::max_element(binChannels.begin(), binChannels.end()) );
                break;
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

        //std::cout << "Max signal " << *std::max_element( this->analyzedSignal.begin(), this->analyzedSignal.end() ) << std::endl;

        // The above guarantees that this->analyzedSignal and this->analyzedFrequencies now have equal sizes
        // Scan for the N strongest peaks in the signal
        int strongestSignalIndex;
        int firstRemovedIndex;
        int lastRemovedIndex;
        double strongestSignal;
        std::vector<double> peakSignals;
        std::vector<double> peakFrequencies;

        double sumWeightedFrequencies;
        double sumSignals;
        double centerFrequency;
        double noteIntensity;

        for (unsigned int i = 0; i < NUM_ANALYZED_NOTES; i++) {
            // Find the current absolute maximum
            strongestSignalIndex = std::max_element( this->analyzedSignal.begin(), this->analyzedSignal.end() ) - this->analyzedSignal.begin(); // subtract begin from iterator to get index
            strongestSignal = this->analyzedSignal[strongestSignalIndex];

            this->noteFrequencies[i] = -1.0; // hide the notes by default
            this->noteIntensities[i] = 0.0;

            if (strongestSignal > this->noiseLevel) {
                // Determine the peak masking range around the maximum, respecting the range of valid vector indices
                firstRemovedIndex = std::max(strongestSignalIndex - this->analysisMaskRadius, 0);
                lastRemovedIndex = std::min( strongestSignalIndex + this->analysisMaskRadius, (int)(this->analyzedSignal.size()) );

                // collect data on the current signal peak, then flatten the signal to avoid reanalyzing the peak on the next pass
                peakSignals.clear();
                peakFrequencies.clear();

                for (int j = firstRemovedIndex; j < lastRemovedIndex; j++) { // intentionally leave last point out - gives more accurate results with discrete frequencies
                    peakSignals.push_back(this->analyzedSignal[j]*this->analyzedSignal[j]); // convert amplitude to intensity
                    peakFrequencies.push_back(this->analyzedFrequencies[j]);

                    //std::cout << analyzedFrequencies[j] << " Hz  " << analyzedSignal[j] << " counts" << std::endl;

                    this->analyzedSignal[j] = this->noiseLevel;
                }

                // refine the center frequency of the peak by taking the signal-weighted average of the frequencies
                sumWeightedFrequencies = 0.0;
                sumSignals = 0.0;

                for (size_t j = 0; j < peakSignals.size(); j++) {
                    sumWeightedFrequencies += peakFrequencies[j]*peakSignals[j];
                    sumSignals += peakSignals[j];
                }

                // calculate the peak intensity in decibels over noise
                noteIntensity = sumSignals/(2*analysisMaskRadius)/this->noiseLevel;
                noteIntensity = 10*log10(noteIntensity); // signals were already converted to intensities

                this->noteIntensities[i] = noteIntensity;
                if (noteIntensity >= minimumNoteSNR) {
                    centerFrequency = sumWeightedFrequencies/sumSignals;
                    this->noteFrequencies[i] = centerFrequency;
                    std::cout << "Strong peak at " << centerFrequency << " Hz, SNR = " << noteIntensity << " dB" << std::endl;
                }
                else {
                    this->noteFrequencies[i] = -1.0;
                }
            }
        }

        // Update the estimated noise level. Note that this scrambles the signal vector and must therefore be the last step of analysis!
        unsigned int noiseQuantileIndex = this->analyzedSignal.size()/NOISE_QUANTILE;
        std::nth_element( this->analyzedSignal.begin(), this->analyzedSignal.begin()+noiseQuantileIndex, this->analyzedSignal.end() );
        this->noiseLevel = this->analyzedSignal[noiseQuantileIndex]*this->noiseMultiplier;

    }
}


void BrainSpectrum::handleScroll(int rawX, int rawY, int windowWidth, int windowHeight, float delta) {
    // Respond to mouse wheel movements

    // correct the mouse coordinates in case the window has been resized
    int x = (float)rawX/windowWidth*BrainConfig::INITIAL_WINDOW_WIDTH;
    int y = (float)rawY/windowHeight*BrainConfig::INITIAL_WINDOW_HEIGHT;

    // Zooming the spectrum?
    if (x >= SPECTRUM_POSITION.x-SPECTRUM_CONTAINER_PAD &&
        x <= SPECTRUM_POSITION.x+SPECTRUM_WIDTH+SPECTRUM_CONTAINER_PAD &&
        y >= SPECTRUM_POSITION.y-SPECTRUM_CONTAINER_PAD &&
        y <= SPECTRUM_POSITION.y+SPECTRUM_HEIGHT+SPECTRUM_CONTAINER_PAD) {

        if (delta > 0) {
            // Zoom in
            this->zoomLevel = std::min(this->zoomLevel*2, MAX_ZOOM_LEVEL);
            std::cout << "Zoom " << this->zoomLevel << "x" << std::endl;
        }
        else if (delta < 0) {
            // Zoom out
            this->zoomLevel = std::max(this->zoomLevel/2, 1);
            std::cout << "Zoom " << this->zoomLevel << "x" << std::endl;
        }
    }
}


void BrainSpectrum::handleMouseMove(int rawX, int rawY, int windowWidth, int windowHeight) {
    // Respond to mouse wheel movements

    // correct the mouse coordinates in case the window has been resized
    int x = (float)rawX/windowWidth*BrainConfig::INITIAL_WINDOW_WIDTH;
    int y = (float)rawY/windowHeight*BrainConfig::INITIAL_WINDOW_HEIGHT;

    std::cout << "Mouse at " << x << ", " << y << std::endl;

    // On the spectrum?
    if (x >= SPECTRUM_POSITION.x-SPECTRUM_CONTAINER_PAD &&
        x <= SPECTRUM_POSITION.x+SPECTRUM_WIDTH+SPECTRUM_CONTAINER_PAD &&
        y >= SPECTRUM_POSITION.y-SPECTRUM_CONTAINER_PAD &&
        y <= SPECTRUM_POSITION.y+SPECTRUM_HEIGHT+SPECTRUM_CONTAINER_PAD) {

        this->zoomRectangleVisible = true;

        // Position the zoom rectangle around the mouse position
        float newX = x-zoomRectangle.getSize().x/2;
        newX = std::max(newX, SPECTRUM_POSITION.x);
        newX = std::min(newX, SPECTRUM_POSITION.x+SPECTRUM_WIDTH/2);
        this->zoomRectangle.setPosition( sf::Vector2f(newX, zoomRectangle.getPosition().y) );

        // Position the spectrum cursor at the mouse position
        newX = x-spectrumCursor.getSize().x/2;
        newX = std::max(newX, SPECTRUM_POSITION.x);
        newX = std::min(newX, SPECTRUM_POSITION.x+SPECTRUM_WIDTH);
        this->spectrumCursor.setPosition( sf::Vector2f(newX, spectrumCursor.getPosition().y) );
    }
    else {
        this->zoomRectangleVisible = false;
    }
}

