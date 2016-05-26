#ifndef PROCESSING_H
#define PROCESSING_H

#include <stdexcept>

extern "C" {
#   include <libavcodec/avcodec.h>
#   include <libavformat/avformat.h>
}

namespace processing
{

class FrameDimensionsDiffer: public std::runtime_error {
public:
    FrameDimensionsDiffer(): std::runtime_error("The dimensions (width and/or height) of the frames are not equal.") {}
};

struct YUVTriple {
    double y,u,v;
};

struct FrameDiff {
    double abs_diff;
    double abs_norm_diff;
    unsigned int nb_pix;
    double c1avg, c2avg, c3avg;
};

YUVTriple get_yuv_colors(AVFrame const &frame);
FrameDiff abs_frame_difference(AVFrame const *pFrame, AVFrame const *pFramePrev, bool compute_averages);

}

#endif // PROCESSING_H
