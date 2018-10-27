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

    private:
        sf::VertexArray spectrumLines;

};


#endif /* BRAIN_SPECTRUM_HPP */
