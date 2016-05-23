#include "processing.h"

namespace processing {

YUVTriple get_yuv_colors(AVFrame const &frame) {
    auto const width = frame.width;
    auto const height = frame.height;
    int y_tot = 0;
    int cb_tot = 0;
    int cr_tot = 0;

    // Parallelize YUV summing over rows
    #pragma omp parallel for reduction(+:y_tot,cb_tot,cr_tot)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            auto const y_current  = frame.data[0][frame.linesize[0] * y + x];  // Y
            auto const cb_current = frame.data[1][frame.linesize[1] * y + x];  // Cb
            auto const cr_current = frame.data[2][frame.linesize[2] * y + x];  // Cr

            y_tot += y_current;
            cb_tot += cb_current;
            cr_tot += cr_current;
        }
    }

    const double nbpix = width * height;
    const double y_avg = y_tot/nbpix;
    const double cb_avg = cb_tot/nbpix;
    const double cr_avg = cr_tot/nbpix;
    return {y_avg, cb_avg, cr_avg};
}

}
