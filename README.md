# audiobuffer.h

Single header C++ library for easy access to audiobuffers.
This library is part of a bigger project i am working on, so it lives in the namespace **aal**, which stands for **audio abstraction layer**

## Features

- same interface for interleaved and non-interleaved buffers
- supports int32_t, int16_t, int8_t and float buffers
- converts one buffer format to another

## Example
```c++
#include <cstring>
#include <iostream>

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
```

## API
### Classes
#### AudioBuffer\<T\>
Generic base class that provides the common interface.
|method|description|
|---|---|
|[*x*]-operator|returns the *x*th channel as Channel\<T>|
|&-operator|returns the underlying buffer for read/write access as T*|
### InterleavedAudioBuffer\<T>
Subclass of AudioBuffer\<T>.

AudioBuffer where the underlying buffer (e.g. from the audio driver) is interleaved (e.g. for stereo: LRLRLRLR)
|method|description|
|---|---|
|copyTo\<*format*>(InterleavedAudioBuffer\<*format*>& target)|copies and converts the buffer to the target buffer with *format*|

### NonInterleavedAudioBuffer\<T>
Subclass of AudioBuffer\<T>.

AudioBuffer where the underlying buffer (e.g. from the audio driver) is non-interleaved (e.g. for stereo: LLLLRRRR)
|method|description|
|---|---|
|copyTo\<*format*>(NonInterleavedAudioBuffer\<*format*>& target)|copies and converts the buffer to the target buffer with *format*|
### Channel\<T>
Provides access to the samples of a channel by using the []-operator

|method|description|
|---|---|
|[*x*]-operator|returns the *x*th sample of this channel for read/write access|

## TODOS
- write tests
- support for 24bit integers
- support for double/float64
- ???
