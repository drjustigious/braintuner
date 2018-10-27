#include "FFT.hpp"
/*
Forward and inverse fast discrete Fourier transforms.
Based on <https://rosettacode.org/wiki/Fast_Fourier_transform>
*/

// Forward Cooley-Tukey FFT (in-place, breadth-first, decimation-in-frequency)
void fft(std::vector<Complex> &x)
{
	// DFT
	unsigned int N = x.size(), k = N, n;
	double thetaT = 3.14159265358979323846264338328L / N;
	Complex phiT = Complex(cos(thetaT), -sin(thetaT)), T;
	while (k > 1)
	{
		n = k;
		k >>= 1;
		phiT = phiT * phiT;
		T = 1.0L;
		for (unsigned int l = 0; l < k; l++)
		{
			for (unsigned int a = l; a < N; a += n)
			{
				unsigned int b = a + k;
				Complex t = x[a] - x[b];
				x[a] += x[b];
				x[b] = t * T;
			}
			T *= phiT;
		}
	}
	// Decimate
	unsigned int m = (unsigned int)log2(N);
	for (unsigned int a = 0; a < N; a++)
	{
		unsigned int b = a;
		// Reverse bits
		b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
		b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
		b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
		b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
		b = ((b >> 16) | (b << 16)) >> (32 - m);
		if (b > a)
		{
			Complex t = x[a];
			x[a] = x[b];
			x[b] = t;
		}
	}
	//// Normalize (This section make it not working correctly)
	//Complex f = 1.0 / sqrt(N);
	//for (unsigned int i = 0; i < N; i++)
	//	x[i] *= f;
}



// Inverse fft (in-place)
void ifft(std::vector<Complex> &x)
{
    size_t transformSize = x.size();

    // conjugate the complex numbers
    for (std::size_t i = 0; i < transformSize; i++) {
        x[i] = std::conj(x[i]);
    }

    // forward fft
    fft( x );

    // conjugate again and scale back
    for (std::size_t i = 0; i < transformSize; i++) {
        x[i] = std::conj(x[i]) / (double)transformSize;
    }
}
