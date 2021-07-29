#ifndef BM3D_H_INCLUDED
#define BM3D_H_INCLUDED

#include <fftw3.h>
#include <vector>

/*

% BM3D image denoising.

# ABOUT

* Author    : Marc Lebrun <marc.lebrun@cmla.ens-cachan.fr>
* Copyright : (C) 2011 IPOL Image Processing On Line http://www.ipol.im/
* Licence   : GPL v3+, see GPLv3.txt

# OVERVIEW

This source code provides an implementation of the BM3D image denoising.

# UNIX/LINUX/MAC USER GUIDE

The code is compilable on Unix/Linux and Mac OS. 

- Compilation. 
Automated compilation requires the make program.

- Library. 
This code requires the libpng library and the fftw library.

- Image format. 
Only the PNG format is supported. 
 
-------------------------------------------------------------------------
Usage:
1. Download the code package and extract it. Go to that directory. 

2. Compile the source code (on Unix/Linux/Mac OS). 
There are two ways to compile the code. 
(1) RECOMMENDED, with Open Multi-Processing multithread parallelization 
(http://openmp.org/). Roughly speaking, it accelerates the program using the 
multiple processors in the computer. Run
make OMP=1

OR
(2) If the complier does not support OpenMp, run 
make

3. Run BM3D image denoising.
./BM3Ddenoising
The generic way to run the code is:

./BM3Ddenoising cinput.png sigma add_noise ImNoisy.png ImBasic.png ImDenoised.png ImDiff.png ImBias.png
ImDiffBias.png computeBias 2DtransformStep1 useSD1 2DtransformStep2 useSD2 ColorSpace

with :
- cinput.png is a noise-free image;
- sigma is the value of the noise which will be added to cinput.png;
- add_noise is the option to add noise or not to the input image;
- ImNoisy.png will contain the noisy image;
- ImBasic.png will contain the result of the first step of the algorithm;
- ImDenoised.png will contain the final result of the algorithm;
- ImDiff.png will contain the difference between cinput.png and ImDenoised.png;
- ImBias.png will contain the result of the algorithm applied on cinput.png;
- ImDiffBias.png will contain the difference between cinput.png and ImBias.png;
- computeBias : see (3);
- 2DtransformStep1: choice of the 2D transform which will be applied in the first step of the
algorithm. See (4);
- useSD1 : see (1) below;
- 2DtransformStep2: choice of the 2D transform which will be applied in the second step of the
algorithm. See (4);
- useSD2 : see (2);
- ColorSpace : choice of the color space on which the image will be applied. See (5).

There are multiple ways to run the code:
(1) for the first step, users can choose if they prefer to use
standard variation for the weighted aggregation (useSD1 = 1)
(2) for the second step, users can choose if they prefer to use
standard variation for the weighted aggregation (useSD2 = 1)
(3) you can moreover want to compute the bias (algorithm applied to the original
image). To do this, use computeBias = 1.
(4) you can choose the DCT transform or the Bior1.5 transform for the 2D transform
in the step 1 (tau_2D_hard = dct or bior) and/or the step 2. (tau_2d_wien = dct or
bior).
(5) you can choose the colorspace for both steps between : rgb, yuv, ycbcr and opp.
 
Example, run
./BM3Ddenoising cinput.png 10 1 ImNoisy.png ImBasic.png ImDenoised.png ImDiff.png ImBias.png
ImDiffBias.png 1 bior 0 dct 1 opp


# ABOUT THIS FILE

Copyright 2011 IPOL Image Processing On Line http://www.ipol.im/

Copying and distribution of this file, with or without modification,
are permitted in any medium without royalty provided the copyright
notice and this notice are preserved.  This file is offered as-is,
without any warranty.

*/


/** ------------------ **/
/** - Main functions - **/
/** ------------------ **/
//! Main function
int run_bm3d(
    const float sigma
,   std::vector<float> &img_noisy
,   std::vector<float> &img_basic
,   std::vector<float> &img_denoised
,   const unsigned width
,   const unsigned height
,   const unsigned chnls
,   const bool useSD_h
,   const bool useSD_w
,   const unsigned tau_2D_hard
,   const unsigned tau_2D_wien
,   const unsigned color_space
);

//! 1st step of BM3D
void bm3d_1st_step(
    const float sigma
,   std::vector<float> const& img_noisy
,   std::vector<float> &img_basic
,   const unsigned width
,   const unsigned height
,   const unsigned chnls
,   const unsigned nHard
,   const unsigned kHard
,   const unsigned NHard
,   const unsigned pHard
,   const bool     useSD
,   const unsigned color_space
,   const unsigned tau_2D
,   fftwf_plan *  plan_2d_for_1
,   fftwf_plan *  plan_2d_for_2
,   fftwf_plan *  plan_2d_inv
);

//! 2nd step of BM3D
void bm3d_2nd_step(
    const float sigma
,   std::vector<float> const& img_noisy
,   std::vector<float> const& img_basic
,   std::vector<float> &img_denoised
,   const unsigned width
,   const unsigned height
,   const unsigned chnls
,   const unsigned nWien
,   const unsigned kWien
,   const unsigned NWien
,   const unsigned pWien
,   const bool     useSD
,   const unsigned color_space
,   const unsigned tau_2D
,   fftwf_plan *  plan_2d_for_1
,   fftwf_plan *  plan_2d_for_2
,   fftwf_plan *  plan_2d_inv
);

//! Process 2D dct of a group of patches
void dct_2d_process(
    std::vector<float> &DCT_table_2D
,   std::vector<float> const& img
,   fftwf_plan * plan_1
,   fftwf_plan * plan_2
,   const unsigned nHW
,   const unsigned width
,   const unsigned height
,   const unsigned chnls
,   const unsigned kHW
,   const unsigned i_r
,   const unsigned step
,   std::vector<float> const& coef_norm
,   const unsigned i_min
,   const unsigned i_max
);

//! Process 2D bior1.5 transform of a group of patches
void bior_2d_process(
    std::vector<float> &bior_table_2D
,   std::vector<float> const& img
,   const unsigned nHW
,   const unsigned width
,   const unsigned height
,   const unsigned chnls
,   const unsigned kHW
,   const unsigned i_r
,   const unsigned step
,   const unsigned i_min
,   const unsigned i_max
,   std::vector<float> &lpd
,   std::vector<float> &hpd
);

void dct_2d_inverse(
    std::vector<float> &group_3D_table
,   const unsigned kHW
,   const unsigned N
,   std::vector<float> const& coef_norm_inv
,   fftwf_plan * plan
);

void bior_2d_inverse(
    std::vector<float> &group_3D_table
,   const unsigned kHW
,   std::vector<float> const& lpr
,   std::vector<float> const& hpr
);

//! HT filtering using Welsh-Hadamard transform (do only
//! third dimension transform, Hard Thresholding
//! and inverse Hadamard transform)
void ht_filtering_hadamard(
    std::vector<float> &group_3D
,   std::vector<float> &tmp
,   const unsigned nSx_r
,   const unsigned kHard
,   const unsigned chnls
,   std::vector<float> const& sigma_table
,   const float lambdaThr3D
,   std::vector<float> &weight_table
,   const bool doWeight
);

//! Wiener filtering using Welsh-Hadamard transform
void wiener_filtering_hadamard(
    std::vector<float> &group_3D_img
,   std::vector<float> &group_3D_est
,   std::vector<float> &tmp
,   const unsigned nSx_r
,   const unsigned kWien
,   const unsigned chnls
,   std::vector<float> const& sigma_table
,   std::vector<float> &weight_table
,   const bool doWeight
);

//! Compute weighting using Standard Deviation
void sd_weighting(
    std::vector<float> const& group_3D
,   const unsigned nSx_r
,   const unsigned kHW
,   const unsigned chnls
,   std::vector<float> &weight_table
);

//! Apply a bior1.5 spline wavelet on a vector of size N x N.
void bior1_5_transform(
    std::vector<float> const& input
,   std::vector<float> &output
,   const unsigned N
,   std::vector<float> const& bior_table
,   const unsigned d_i
,   const unsigned d_o
,   const unsigned N_i
,   const unsigned N_o
);

/** ---------------------------------- **/
/** - Preprocessing / Postprocessing - **/
/** ---------------------------------- **/
//! Preprocess coefficients of the Kaiser window and normalization coef for the DCT
void preProcess(
    std::vector<float> &kaiserWindow
,   std::vector<float> &coef_norm
,   std::vector<float> &coef_norm_inv
,   const unsigned kHW
);

void precompute_BM(
    std::vector<std::vector<unsigned> > &patch_table
,   const std::vector<float> &img
,   const unsigned width
,   const unsigned height
,   const unsigned kHW
,   const unsigned NHW
,   const unsigned n
,   const unsigned pHW
,   const float    tauMatch
);

#endif // BM3D_H_INCLUDED
