#include "BrainSpectrum.hpp"

BrainSpectrum::BrainSpectrum() {
    // CONSTRUCTOR
    // Set the primitive type for drawing
    this->spectrumLines.setPrimitiveType(sf::Lines);
}


void BrainSpectrum::draw(sf::RenderWindow &window) {
    // Draw the spectrum analyzer
    window.draw(this->spectrumLines);
}


void BrainSpectrum::updateSpectrum(std::vector<double> frequencies, std::vector<double> magnitudes) {
    // Takes a zero-symmetric vector of frequencies and corresponding FFT magnitudes and constructs
    // the positive frequency side of the spectrum out of lines. The vectors are assumed to be
    // identical in size.

    std::size_t numPoints = magnitudes.size();
    assert( frequencies.size() == numPoints );

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
}
