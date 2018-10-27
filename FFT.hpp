#ifndef FFT_HPP
#define FFT_HPP

#include <complex>
#include <vector>
#include <algorithm>

typedef std::complex<double> Complex;


void fft(std::vector<Complex> &x);
void ifft(std::vector<Complex> &x);

#endif /* FFT_HPP */
