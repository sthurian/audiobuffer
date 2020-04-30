#include <cstring>

#include "audiobuffer.h"
using namespace aal;  // namespace of audio abstraction layer (aal)

int main() {
    short channels = 2;
    short frames = 4;

    // a plain interleaved buffer as it could come from the audio driver
    // Interleaved means (for two channels) e.g.: LRLRLRLR
    // NonInterleaved means (for two channels) e.g.: LLLLRRRR
    float buf[channels * frames] = {0, 1, 0, 1, 0, 1, 0, 1};

    InterleavedAudioBuffer<float> deviceBuffer(frames, channels);
    // you can access the underlying buffer directly by the address-operator &
    memcpy(&deviceBuffer, buf, sizeof(buf));

    // you can copy buffers to different formats (float, int8_t, int16_t and int32_t)
    InterleavedAudioBuffer<int32_t> userBuffer(frames, channels);
    deviceBuffer.copyTo<int32_t>(userBuffer);

    auto& leftChannel = userBuffer[0];   // get the first channel
    auto& rightChannel = userBuffer[1];  // get the second channel

    rightChannel[1] = 1337;  // set second sample of the second channel

    std::cout << "samples of left channel:";
    for (auto i = 0; i < frames; i++) {
        std::cout << leftChannel[i] << " ";  // access sample of channel
    }
    std::cout << std::endl;

    std::cout << "samples of right channel:";
    for (auto i = 0; i < frames; i++) {
        std::cout << rightChannel[i] << " ";  // access sample of channel
    }
    std::cout << std::endl;
    return 0;
}