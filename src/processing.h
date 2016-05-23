#ifndef PROCESSING_H
#define PROCESSING_H

extern "C" {
#   include <libavcodec/avcodec.h>
#   include <libavformat/avformat.h>
}

namespace processing
{

struct YUVTriple {
    double y,u,v;
};

YUVTriple get_yuv_colors(AVFrame const &frame);

}

#endif // PROCESSING_H
