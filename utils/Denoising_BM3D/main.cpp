#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <string.h>
#include <typeinfo>

#include "bm3d.h"

#define YUV       0
#define YCBCR     1
#define OPP       2
#define RGB       3
#define DCT       4
#define BIOR      5
#define HADAMARD  6
#define NONE      7

using namespace std;

typedef unsigned char byte;

int bm3dAdapter(unsigned char *pic, unsigned width, unsigned height, float fSigma, int dct1, int dct2, int SD1, int SD2);
static void i420_to_planar(const byte *frame, byte *planar, int width, int height);
static void i420_from_planar(byte *frame, const byte *planar, int width, int height);
void byteyuv2yuv(float *img, unsigned width, unsigned height);
void yuv2byteyuv(float *img, unsigned width, unsigned height);

/*
argv[1] = videoYUVfile
argv[2] = width
argv[3] = height
argv[4] = frames
argv[5] = sigma
*/
int main(int argc, char **argv)
{
    const unsigned width = atoi(argv[2]);
    const unsigned height = atoi(argv[3]);
    const unsigned maxFrames = atoi(argv[4]);
    const unsigned sigma = atoi(argv[5]);
    const char* videoYUVpath = argv[1];

    const int whc = width*height*3;
    const int whc_2 = whc/2;

    FILE *fp_in  = NULL;
    FILE *fp_out = NULL;
    fp_in = fopen(videoYUVpath, "rb");
    fp_out = fopen("result.yuv", "wb");

    unsigned char* pic = NULL;
    unsigned char* pic420 = NULL;
    pic = (unsigned char*)malloc( sizeof(unsigned char) * whc );
    pic420 = (unsigned char*)malloc( sizeof(unsigned char) * whc_2 );

    for(int frame_i = 0; frame_i < maxFrames; ++frame_i)
    {
        if( fread(pic420, sizeof(unsigned char), whc_2, fp_in) != whc_2 )
            cout << "fckngsht on in" << endl;

        i420_to_planar(pic420, pic, width, height);

        cout << "frame " << (frame_i+1) << flush;
        bm3dAdapter(pic, width, height, sigma, 1, 1, 0, 0);

        i420_from_planar(pic420, pic, width, height);

        fwrite(pic420, sizeof(unsigned char), whc_2, fp_out );

        cout << " done. " << endl;
    }

    free(pic);
    free(pic420);

    if(fp_out != NULL)
        fclose( fp_out );
    if(fclose != NULL)
        fclose( fp_in );
}

/**
 * @param pic yuv изображение
 * @param width его ширина
 * @param height его высота
 * @param fSigma насколько сильно зашумлено (чем выше, тем исходное изображение "шумнее"). Значение от 0 до 75. Чем выше значение, тем больше может быть "размытия", и тем дольше работает
 * @param dct1 Использовать DCT или Bior1.5 на шаге 1. if dct1==0, то Bior1.5, иначе DCT
 * @param dct2 Использовать DCT или Bior1.5 на шаге 2. if dct2==0, то Bior1.5, иначе DCT
 * @param SD1 Использовать ли стандартное изменение для взвешенного агрегирования на шаге 1
 * @param SD2 Использовать ли стандартное изменение для взвешенного агрегирования на шаге 2
 * 
 */
int bm3dAdapter(unsigned char *pic, unsigned width, unsigned height, float fSigma, int dct1, int dct2, int SD1, int SD2)
{
	//! Declarations
	vector<float> img, img_noisy, img_basic, img_denoised, img_bias, img_diff;
	vector<float> img_basic_bias;
	vector<float> img_diff_bias;
    unsigned chnls = 3;

	const unsigned tau_2D_hard = dct1?DCT:BIOR;
                                  
	const unsigned tau_2D_wien = dct2?DCT:BIOR;

	const unsigned color_space  = YUV;

	unsigned wh = (unsigned) width * height;
	unsigned whc = (unsigned) wh * chnls;

    float *buffPic = NULL;
	buffPic = (float*)malloc(sizeof(float) * whc);
	for(int li = 0; li < whc; ++li)
        buffPic[li] = pic[li];

	byteyuv2yuv(buffPic, width, height);
	
	img.resize(whc);
    for(int li = 0; li < whc; ++li)
        img[li] = buffPic[li];

	//! Variables initialization
	int useSD_1 = SD1;
	int useSD_2 = SD2;

	img_noisy.resize(whc);
	img_diff.resize(whc);
			
	img_noisy = img;

    //! Denoising
    if (run_bm3d(fSigma, img_noisy, img_basic, img_denoised, width, height, chnls,
                 useSD_1, useSD_2, tau_2D_hard, tau_2D_wien, color_space)
        != EXIT_SUCCESS)
        return EXIT_FAILURE;

    for(int li = 0; li < whc; ++li)
        buffPic[li] = img_denoised[li];

	yuv2byteyuv(buffPic, width, height);

    for(int i = 0; i < whc; ++i)
        pic[i] = buffPic[i]+0.5f;
	free(buffPic);

	return EXIT_SUCCESS;
}

static void i420_to_planar( const byte *frame, byte *planar, int width, int height )
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

static void i420_from_planar( byte *frame, const byte *planar, int width, int height )
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

void yuv2byteyuv(float *img, unsigned width, unsigned height)
{
	unsigned wh = width*height;
	unsigned whc = wh*3;
	float *tmp = NULL;
	tmp = (float*)malloc(sizeof(float) * whc);

	unsigned red = 0;
	unsigned green = width * height;
	unsigned blue = width * height * 2;

	//rgb first
	for (unsigned k = 0; k < wh; k++)
	{
		//! Red   channel
		tmp[k + red  ] = img[k + red] + 1.13983f * img[k + blue];
		//! Green channel
		tmp[k + green] = img[k + red] - 0.39465f * img[k + green] - 0.5806f * img[k + blue];
		//! Blue  channel
		tmp[k + blue ] = img[k + red] + 2.03211f * img[k + green];
	}

	//byte yuv second
	for(int k = 0; k < wh; ++k)
	{
		//! Y
		img[k + red] = 0.299f   * tmp[k + red] + 0.587f   * tmp[k + green] + 0.114f   * tmp[k + blue];
		//! U
		img[k + green] = -0.1687f * tmp[k + red] - 0.3313f * tmp[k + green] + 0.5f   * tmp[k + blue] + 128;
		//! V
		img[k + blue] = 0.5f   * tmp[k + red] - 0.4187f * tmp[k + green] - 0.0813f * tmp[k + blue]+128;

		img[k + red] = img[k + red]>255?255:img[k + red];
		img[k + green] = img[k + green]>255?255:img[k + green];
		img[k + blue] = img[k + blue]>255?255:img[k + blue];

		img[k + red] = img[k + red]<0?0:img[k + red];
		img[k + green] = img[k + green]<0?0:img[k + green];
		img[k + blue] = img[k + blue]<0?0:img[k + blue];
	}
	free(tmp);
}

void byteyuv2yuv(float *img, unsigned width, unsigned height)
{
	unsigned wh = width*height;
	unsigned whc = wh*3;
	float *tmp = NULL;
	tmp = (float*)malloc(sizeof(float) * whc);

	unsigned red = 0;
	unsigned green = width * height;
	unsigned blue = width * height * 2;
	
	//rgb first
	for(int k = 0; k < wh; ++k)
	{
		//! R
		tmp[k + red] = img[k + red] + 1.402f * (img[k + blue]-128);
		//! G
		tmp[k + green] = img[k + red] - 0.3441f * (img[k + green]-128) - 0.7141f * (img[k + blue]-128);
		//! B
		tmp[k + blue] = img[k + red] + 1.772f * (img[k + green]-128);

		tmp[k + red] = tmp[k + red]>255?255:tmp[k + red];
		tmp[k + green] = tmp[k + green]>255?255:tmp[k + green];
		tmp[k + blue] = tmp[k + blue]>255?255:tmp[k + blue];

		tmp[k + red] = tmp[k + red]<0?0:tmp[k + red];
		tmp[k + green] = tmp[k + green]<0?0:tmp[k + green];
		tmp[k + blue] = tmp[k + blue]<0?0:tmp[k + blue];
	}

	//float yuv second
	for(int k = 0; k < wh; ++k)
	{
		//! Y
		img[k + red  ] =  0.299f   * tmp[k + red] + 0.587f   * tmp[k + green] + 0.114f   * tmp[k + blue];
		//! U
		img[k + green] = -0.14713f * tmp[k + red] - 0.28886f * tmp[k + green] + 0.436f   * tmp[k + blue];
		//! V
        img[k + blue ] =  0.615f   * tmp[k + red] - 0.51498f * tmp[k + green] - 0.10001f * tmp[k + blue];
	}
	free(tmp);
}