#include <vector>

typedef unsigned char byte;

void add_noise(
    const std::vector<float> &img
,   std::vector<float> &img_noisy
,   const float sigma
);

void color_space_transform(
    std::vector<float> &img
,   const unsigned width
,   const unsigned height
,   const bool rgb2yuv
);

void i420_to_planar(const byte *frame, byte *planar, int width, int height);
void i420_from_planar(byte *frame, const byte *planar, int width, int height);
