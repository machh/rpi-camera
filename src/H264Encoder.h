
#ifndef CODEC_H264ENCODER_H_
#define CODEC_H264ENCODER_H_

#include <cstdint>
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
#include <x264.h>
#ifdef __cplusplus
}
#endif //__cplusplus

namespace rpi {

class H264Encoder {
public:
    typedef struct {
        int iType;
        int iLength;
        uint8_t *pucData;
    } H264Frame;

    H264Encoder();
    ~H264Encoder();

    bool init(int iWidth, int iHeight, int iFps, int iBitRate);
    int inputData(char *yuv[3], int linesize[3], int64_t cts, H264Frame **out_frame);

private:
    x264_t *_pX264Handle = nullptr;
    x264_picture_t *_pPicIn = nullptr;
    x264_picture_t *_pPicOut = nullptr;
    H264Frame _aFrames[10];
};

} /* namespace  */

#endif /* CODEC_H264ENCODER_H_ */
