#include <processing.h>

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

FrameDiff abs_frame_difference(AVFrame const *pFrame, AVFrame const *pFramePrev, bool compute_averages){

    if( (pFrame->width==0) || (pFramePrev->width==0) ||
        (pFrame->height==0) || (pFramePrev->height==0)
            ){
        throw FrameDimensionsNotSet();
    }
    if ( (pFrame->width != pFramePrev->width) ||
         (pFrame->height != pFramePrev->height) ) {
        throw FrameDimensionsDiffer();
    }

    auto width = pFrame->width;
    auto height = pFrame->height;

    int c1tot = 0;
    int c2tot = 0;
    int c3tot = 0;
    unsigned int abs_diff = 0;

    // IDEA! Split image in slices and calculate score per-slice.
    // This would allow to detect areas on the image which have stayed
    // the same, and (a) increase score if all areas have changed
    // and (b) decrease score if some areas have changed less (ot not at all).

    #pragma omp parallel for reduction(+:c1tot,c2tot,c3tot,abs_diff)
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            const auto c1 = *(pFrame->data[0] + y * pFrame->linesize[0] + x * 3);
            const auto c2 = *(pFrame->data[0] + y * pFrame->linesize[0] + x * 3 + 1);
            const auto c3 = *(pFrame->data[0] + y * pFrame->linesize[0] + x * 3 + 2);

            const auto c1prev = *(pFramePrev->data[0] + y * pFramePrev->linesize[0] + x * 3);
            const auto c2prev = *(pFramePrev->data[0] + y * pFramePrev->linesize[0] + x * 3 + 1);
            const auto c3prev = *(pFramePrev->data[0] + y * pFramePrev->linesize[0] + x * 3 + 2);

            if(compute_averages){
                c1tot += c1;
                c2tot += c2;
                c3tot += c3;
            }

            abs_diff += abs(c1 - c1prev) + abs(c2 - c2prev) + abs(c3 - c3prev);
        }
    }

    const unsigned int nbpx = (height * width);
    const double abs_norm_diff = static_cast<double>(abs_diff) / nbpx;

    FrameDiff result;
    result.abs_diff = abs_diff;
    result.abs_norm_diff = abs_norm_diff;
    result.nb_pix = nbpx;

    if (compute_averages){
        const double c1avg = static_cast<double>(c1tot) / nbpx;
        const double c2avg = static_cast<double>(c2tot) / nbpx;
        const double c3avg = static_cast<double>(c3tot) / nbpx;
        result.c1avg = c1avg;
        result.c2avg = c2avg;
        result.c3avg = c3avg;
    }else{
        result.c1avg = result.c2avg = result.c3avg = 0;
    }

    return result;
}

}
