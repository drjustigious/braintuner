#ifndef BRAIN_CONFIG_HPP
#define BRAIN_CONFIG_HPP

namespace BrainConfig {

const unsigned int FRAME_RATE = 60;

const unsigned int INITIAL_WINDOW_WIDTH = 640;
const unsigned int INITIAL_WINDOW_HEIGHT = INITIAL_WINDOW_WIDTH*3/4; // force 4:3 aspect ratio

const unsigned int MINIMUM_WINDOW_WIDTH = INITIAL_WINDOW_WIDTH;
const unsigned int MINIMUM_WINDOW_HEIGHT = INITIAL_WINDOW_HEIGHT;

} // namespace BrainConfig


#endif /* BRAIN_CONFIG_HPP */
