#ifndef AAL_BUFFERS_H
#define AAL_BUFFERS_H

#include <stdint.h>

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>
namespace aal {

template <typename T>
class Buffer {
   protected:
    T* buffer;
    uint32_t size;

   public:
    Buffer(uint32_t size) : size(size) { this->buffer = (T*)malloc(sizeof(T) * size); };
    virtual ~Buffer() { free(this->buffer); };
    T* operator&() const { return this->buffer; }

    // tmp
    void printBuffer() {
        for (uint32_t i = 0; i < size; i++) std::cout << (float)buffer[i] << ", ";
        std::cout << std::endl;
    }
};

template <typename T>
class AudioBuffer;

template <typename T>
class NonInterleavedAudioBuffer;
template <typename T>
class InterleavedAudioBuffer;

template <typename T>
class Channel {
    friend AudioBuffer<T>;

   protected:
    const AudioBuffer<T>* buffer;
    uint16_t index;

   public:
    Channel(const AudioBuffer<T>* buffer, uint16_t index) : buffer(buffer), index(index){};
    virtual ~Channel(){};
    virtual const T* operator[](unsigned int i) const = 0;
    virtual T& operator[](unsigned int i) = 0;
};

template <typename T>
class NonInterleavedChannel : public Channel<T> {
   public:
    NonInterleavedChannel(const AudioBuffer<T>* buffer, uint16_t index)
        : Channel<T>(buffer, index){};
    const T* operator[](unsigned int i) const {
        return &*this->buffer + this->index * this->buffer->getFrames() + i;
    }
    T& operator[](unsigned int i) {
        return *(&*this->buffer + this->index * this->buffer->getFrames() + i);
    }
};

template <typename T>
class InterleavedChannel : public Channel<T> {
   public:
    InterleavedChannel(const AudioBuffer<T>* buffer, uint16_t index) : Channel<T>(buffer, index){};
    const T* operator[](unsigned int i) const {
        uint16_t k = (i + this->index * this->buffer->getFrames());

        return &*this->buffer + ((k % this->buffer->getFrames()) * this->buffer->getChannels() +
                                 (uint16_t)std::floor(k / this->buffer->getFrames()));
    }
    T& operator[](unsigned int i) {
        uint16_t k = (i + this->index * this->buffer->getFrames());
        return *(&*this->buffer + ((k % this->buffer->getFrames()) * this->buffer->getChannels() +
                                   (uint16_t)std::floor(k / this->buffer->getFrames())));
    }
};

template <typename T>
class AudioBuffer : public Buffer<T> {
   protected:
    uint16_t frames;
    uint16_t channels;
    std::vector<std::unique_ptr<Channel<T>>> _channels;
    template <typename outFormat>
    void _copyTo(AudioBuffer<outFormat>& targetBuffer){};

    template <typename outFormat>
    bool copyTo(AudioBuffer<outFormat>& targetBuffer) {
        if (targetBuffer.getChannels() != this->getChannels()) {
            return false;
        }
        if (targetBuffer.getFrames() != this->getFrames()) {
            return false;
        }
        _copyTo<outFormat>(targetBuffer);
        return true;
    };

    bool copyTo(AudioBuffer<T>& targetBuffer) {
        if (targetBuffer.getChannels() != this->getChannels()) {
            return false;
        }
        if (targetBuffer.getFrames() != this->getFrames()) {
            return false;
        }
        for (uint32_t i = 0; i < this->getChannels() * this->getFrames(); i++) {
            (&targetBuffer)[i] = (&*this)[i];
        };
        return true;
    };

   public:
    AudioBuffer(uint16_t frames, uint16_t channels)
        : Buffer<T>(frames * channels), frames(frames), channels(channels){};

    const uint16_t getFrames() const { return frames; };
    const uint16_t getChannels() const { return channels; };
    virtual Channel<T>& operator[](unsigned int i) const = 0;
};

// _copyTo specializations

// one to ones in case user specifies same outFormat
template <>
template <>
void AudioBuffer<float>::_copyTo(AudioBuffer<float>& targetBuffer) {
    for (uint32_t i = 0; i < this->getChannels() * this->getFrames(); i++) {
        (&targetBuffer)[i] = (&*this)[i];
    };
};

template <>
template <>
void AudioBuffer<int32_t>::_copyTo(AudioBuffer<int32_t>& targetBuffer) {
    for (uint32_t i = 0; i < this->getChannels() * this->getFrames(); i++) {
        (&targetBuffer)[i] = (&*this)[i];
    };
};

template <>
template <>
void AudioBuffer<int16_t>::_copyTo(AudioBuffer<int16_t>& targetBuffer) {
    for (uint32_t i = 0; i < this->getChannels() * this->getFrames(); i++) {
        (&targetBuffer)[i] = (&*this)[i];
    };
};

template <>
template <>
void AudioBuffer<int8_t>::_copyTo(AudioBuffer<int8_t>& targetBuffer) {
    for (uint32_t i = 0; i < this->getChannels() * this->getFrames(); i++) {
        (&targetBuffer)[i] = (&*this)[i];
    };
};
// from ints to float

template <>
template <>
void AudioBuffer<int32_t>::_copyTo(AudioBuffer<float>& targetBuffer) {
    for (uint32_t i = 0; i < this->getChannels() * this->getFrames(); i++) {
        (&targetBuffer)[i] = (float)((&*this)[i] / 2147483648.0);
    };
};

// no 24 bit conversion. use int32_t for that

template <>
template <>
void AudioBuffer<int16_t>::_copyTo(AudioBuffer<float>& targetBuffer) {
    for (uint32_t i = 0; i < this->getChannels() * this->getFrames(); i++) {
        (&targetBuffer)[i] = (float)((&*this)[i] / 32768.0);
    };
};

template <>
template <>
void AudioBuffer<int8_t>::_copyTo(AudioBuffer<float>& targetBuffer) {
    for (uint32_t i = 0; i < this->getChannels() * this->getFrames(); i++) {
        (&targetBuffer)[i] = (float)((&*this)[i] / 128.0);
    };
};

// from float to ints
template <>
template <>
void AudioBuffer<float>::_copyTo(AudioBuffer<int32_t>& targetBuffer) {
    for (uint32_t i = 0; i < this->getChannels() * this->getFrames(); i++) {
        (&targetBuffer)[i] =
            (int32_t)std::min(std::lround((&*this)[i] * 2147483648.0), 2147483647L);
    };
};

template <>
template <>
void AudioBuffer<float>::_copyTo(AudioBuffer<int16_t>& targetBuffer) {
    for (uint32_t i = 0; i < this->getChannels() * this->getFrames(); i++) {
        (&targetBuffer)[i] = (int16_t)std::min(std::lround((&*this)[i] * 32768.0), 32767L);
    };
};

template <>
template <>
void AudioBuffer<float>::_copyTo(AudioBuffer<int8_t>& targetBuffer) {
    for (uint32_t i = 0; i < this->getChannels() * this->getFrames(); i++) {
        (&targetBuffer)[i] = (int8_t)std::min(std::lround((&*this)[i] * 128.0), 127L);
    };
};

// from int32 to ints

template <>
template <>
void AudioBuffer<int32_t>::_copyTo(AudioBuffer<int16_t>& targetBuffer) {
    for (uint32_t i = 0; i < this->getChannels() * this->getFrames(); i++) {
        (&targetBuffer)[i] = (int16_t)(((&*this)[i] >> 16) & 0x0000ffff);
    };
};

template <>
template <>
void AudioBuffer<int32_t>::_copyTo(AudioBuffer<int8_t>& targetBuffer) {
    for (uint32_t i = 0; i < this->getChannels() * this->getFrames(); i++) {
        (&targetBuffer)[i] = (int8_t)(((&*this)[i] >> 24) & 0x000000ff);
    };
};

// from int16 to ints

template <>
template <>
void AudioBuffer<int16_t>::_copyTo(AudioBuffer<int32_t>& targetBuffer) {
    for (uint32_t i = 0; i < this->getChannels() * this->getFrames(); i++) {
        (&targetBuffer)[i] = ((int32_t)(&*this)[i]) << 16;
    };
};

template <>
template <>
void AudioBuffer<int16_t>::_copyTo(AudioBuffer<int8_t>& targetBuffer) {
    for (uint32_t i = 0; i < this->getChannels() * this->getFrames(); i++) {
        (&targetBuffer)[i] = (int8_t)(((&*this)[i] >> 8) & 0x00ff);
    };
};

// from int8 to ints

template <>
template <>
void AudioBuffer<int8_t>::_copyTo(AudioBuffer<int32_t>& targetBuffer) {
    for (uint32_t i = 0; i < this->getChannels() * this->getFrames(); i++) {
        (&targetBuffer)[i] = ((int32_t)(&*this)[i]) << 24;
    };
};

template <>
template <>
void AudioBuffer<int8_t>::_copyTo(AudioBuffer<int16_t>& targetBuffer) {
    for (uint32_t i = 0; i < this->getChannels() * this->getFrames(); i++) {
        (&targetBuffer)[i] = ((int16_t)(&*this)[i]) << 8;
    };
};

template <typename T>
class InterleavedAudioBuffer : public AudioBuffer<T> {
   public:
    InterleavedAudioBuffer(uint16_t frames, uint16_t channels) : AudioBuffer<T>(frames, channels) {
        for (auto i = 0; i < channels; i++)
            this->_channels.push_back(std::make_unique<InterleavedChannel<T>>(this, i));
    };
    template <typename outFormat>
    bool copyTo(InterleavedAudioBuffer<outFormat>& targetBuffer) {
        return AudioBuffer<T>::template copyTo<outFormat>(targetBuffer);
    };
    Channel<T>& operator[](unsigned int i) const { return *this->_channels.at(i); };
};

template <typename T>
class NonInterleavedAudioBuffer : public AudioBuffer<T> {
   private:
    std::vector<std::unique_ptr<NonInterleavedChannel<T>>> _channels;

   public:
    NonInterleavedAudioBuffer(uint16_t frames, uint16_t channels)
        : AudioBuffer<T>(frames, channels) {
        for (auto i = 0; i < channels; i++)
            this->_channels.push_back(std::make_unique<NonInterleavedChannel<T>>(this, i));
    };
    template <typename outFormat>
    bool copyTo(NonInterleavedAudioBuffer<outFormat>& targetBuffer) {
        return AudioBuffer<T>::template copyTo<outFormat>(targetBuffer);
    };
    Channel<T>& operator[](unsigned int i) const { return *this->_channels.at(i); };
};

}  // namespace aal

#endif