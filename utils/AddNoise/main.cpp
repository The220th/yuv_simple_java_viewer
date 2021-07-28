#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <string.h>
#include <typeinfo>

#include "utilities.h"

using namespace std;

/*
argv[1] = videoYUVfile
argv[2] = width
argv[3] = height
argv[4] = frames
argv[5] = sigma
*/
int main(int argc, char **argv)
{
	vector<float> img;

    FILE *fp_in  = NULL;
    FILE *fp_out = NULL;

    const unsigned width = atoi(argv[2]);
    const unsigned height = atoi(argv[3]);
    const unsigned frames = atoi(argv[4]);
    const unsigned sigma = atoi(argv[5]);

    int whc = width*height*3;
    int whc_2 = whc / 2;

    if(!(width > 0 || height > 0 || frames > 0))
    {
        cout << "Syntax error" << endl;
        return EXIT_FAILURE;
    }

    fp_in = fopen(argv[1], "rb");
    fp_out = fopen("result.yuv", "wb");

    unsigned char* pic = NULL;
    unsigned char* pic420 = NULL;
    pic = (unsigned char*)malloc(sizeof(unsigned char) * whc);
    pic420 = (unsigned char*)malloc(sizeof(unsigned char) * whc_2);

    img.resize(whc);  

    for(int frame_i = 0; frame_i < frames; ++frame_i)
    {
        if(fread(pic420, sizeof(unsigned char), whc_2, fp_in) != whc_2)
            cout << "fckngsht on in" << endl;

        i420_to_planar(pic420, pic, width, height);

        for(int li = 0; li < whc; ++li)
            img[li] = pic[li];

        color_space_transform(img, width, height, false);

        vector<float> noisedImg;
        noisedImg.resize(whc);
        add_noise(img, noisedImg, sigma);

        color_space_transform(noisedImg, width, height, true);

        for(int li = 0; li < whc; ++li)
            pic[li] = noisedImg[li]+0.5;

        i420_from_planar(pic420, pic, width, height);

        fwrite(pic420, sizeof(unsigned char), whc_2, fp_out);

        cout << "frame " << (frame_i+1) << " noised" << endl;
    }

    free(pic);
    free(pic420);

    if(fp_out != NULL)
        fclose( fp_out );
    if(fclose != NULL)
        fclose( fp_in );

	return EXIT_SUCCESS;
}
