#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <cstring>
#include <random>

#include "mt19937ar.h"
#include "utilities.h"

#define YUV       0
#define YCBCR     1
#define OPP       2
#define RGB       3

using namespace std;

/**
 * @brief add noise to img
 *
 * @param img : original noise-free image
 * @param img_noisy = img + noise
 * @param sigma : standard deviation of the noise
 *
 * @return none.
 * 
 * @author Marc Lebrun <marc.lebrun@cmla.ens-cachan.fr>
 **/
void add_noise(
    const vector<float> &img
,   vector<float> &img_noisy
,   const float sigma
){
    const unsigned size = img.size();
    if (img_noisy.size() != size)
        img_noisy.resize(size);

    mt_init_genrand((unsigned long int) time (NULL) + (unsigned long int) getpid());

    for (unsigned k = 0; k < size; k++)
    {
        double a = mt_genrand_res53();
        double b = mt_genrand_res53();
        double z = (double)(sigma) * sqrt(-2.0 * log(a)) * cos(2.0 * M_PI * b);

        img_noisy[k] =  img[k] + (float) z;
    }
}

/*
void add_noise(
    const vector<float> &img
,   vector<float> &img_noisy
,   const float sigma
){
    const unsigned size = img.size();
    if (img_noisy.size() != size)
        img_noisy.resize(size);

    const double mean = 0.0;
    const double stddev = 1.1;
    std::default_random_engine generator;
    std::normal_distribution<double> dist(mean, stddev);

    for(unsigned k = 0; k < size; ++k)
        img_noisy[k] = img[k] + dist(generator);
}*/

void color_space_transform(
    vector<float> &img
,   const unsigned width
,   const unsigned height
,   const bool rgb2yuv
){
    //! Declarations
    vector<float> tmp;
    tmp.resize(3 * width * height);
    const unsigned red   = 0;
    const unsigned green = width * height;
    const unsigned blue  = width * height * 2;

    if (rgb2yuv)
    {
        for (unsigned k = 0; k < width * height; k++)
        {
            //! Y
            tmp[k + red  ] =  0.299f   * img[k + red] + 0.587f   * img[k + green] + 0.114f   * img[k + blue];
            //! U
            tmp[k + green] = -0.1687f * img[k + red] - 0.3313f * img[k + green] + 0.5f   * img[k + blue] + 128;
            //! V
            tmp[k + blue ] =  0.5f   * img[k + red] - 0.4187f * img[k + green] - 0.0813f * img[k + blue]+128;

            if(tmp[k + red] > 255 || tmp[k + green] > 255 || tmp[k + blue] > 255
            || tmp[k + red] < 0 || tmp[k + green] < 0 || tmp[k + blue] < 0)
            {
                tmp[k + red] = tmp[k + red]>255?255:tmp[k + red];
                tmp[k + green] = tmp[k + green]>255?255:tmp[k + green];
                tmp[k + blue] = tmp[k + blue]>255?255:tmp[k + blue];

                tmp[k + red] = tmp[k + red]<0?0:tmp[k + red];
                tmp[k + green] = tmp[k + green]<0?0:tmp[k + green];
                tmp[k + blue] = tmp[k + blue]<0?0:tmp[k + blue];
            }
        }
    }
    else
    {
        for (unsigned k = 0; k < width * height; k++)
        {
            //! R
            tmp[k + red  ] = img[k + red] + 1.402f * (img[k + blue]-128);
            //! G
            tmp[k + green] = img[k + red] - 0.3441f * (img[k + green]-128) - 0.7141f * (img[k + blue]-128);
            //! B
            tmp[k + blue ] = img[k + red] + 1.772f * (img[k + green]-128);

            if(tmp[k + red] > 255 || tmp[k + green] > 255 || tmp[k + blue] > 255
            || tmp[k + red] < 0 || tmp[k + green] < 0 || tmp[k + blue] < 0)
            {
                tmp[k + red] = tmp[k + red]>255?255:tmp[k + red];
                tmp[k + green] = tmp[k + green]>255?255:tmp[k + green];
                tmp[k + blue] = tmp[k + blue]>255?255:tmp[k + blue];

                tmp[k + red] = tmp[k + red]<0?0:tmp[k + red];
                tmp[k + green] = tmp[k + green]<0?0:tmp[k + green];
                tmp[k + blue] = tmp[k + blue]<0?0:tmp[k + blue];
            }
        }
    }

    for (unsigned k = 0; k < width * height * 3; k++)
        img[k] = tmp[k];
}

void i420_to_planar( const byte *frame, byte *planar, int width, int height )
{
	int wh = width * height;
	const byte *in = frame;
	byte *out = planar;
	int n, k, f;

	for( n = 0 ; n < wh ; n ++ )
		( *out ++ ) = ( *in ++ );

	for( f = 0 ; f < 2 ; f ++ )
	{
		for( k = 0 ; k < height ; k ++ )
		{
			const byte *line = in;

			for( n = 0 ; n < width ; n += 2 )
			{
				( *out ++ ) = *line;
				( *out ++ ) = ( *line ++ );
			}

			if( k % 2 )
				in += width / 2;
		}
	}
}

void i420_from_planar( byte *frame, const byte *planar, int width, int height )
{
	int wh = width * height;
	const byte *in = planar;
	byte *out = frame;
	int n, k, f;

	memset( frame, 0x80, wh * 3 / 2);

	for( n = 0 ; n < wh ; n ++ )
		( *out ++ ) = ( *in ++ );

	for( f = 0 ; f < 2 ; f ++ )
	{
		for( k = 0 ; k < height ; k += 2 )
		{
			for( n = 0 ; n < width ; n += 2 )
			{
				( *out ++ ) = ( *in ++ );
				in ++;
			}

			in += width;
		}
	}
}