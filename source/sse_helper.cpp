#include "Channel.h"

#include "sse_helper.h"


namespace sse
{




void calcAmplitude(const std::complex<double>* pdFourier, std::vector<double>& aDest)
{
    for (unsigned int j=0, k=1; j<aDest.size(); j += 2, k += 2)
    {
        Double2 fReal(pdFourier[j].real(), pdFourier[k].real());
        Double2 fImag(pdFourier[j].imag(), pdFourier[k].imag());
        Double2 fAbs = (fReal * fReal + fImag * fImag);
        fAbs.sqrt();
        fAbs >> &aDest[j];
    }
}

void convertBufferFloat128bit(size_t aStartPos, size_t aSize, std::uint32_t aStartChannel, std::uint32_t aChannels, const Channel* aCP, const void *aInputBuffer, std::vector< std::vector<double> >&aOutputBuffer)
{
    std::uint32_t fItems = aSize*aChannels;
    std::uint32_t fStopChannel = aStartChannel + aChannels;
    const float *fBuffer = static_cast<const float*>(aInputBuffer);

    Float4 fMultiplier;
    Float4 fAdder;
    Float4 fResult;
    const std::uint32_t fMaxIndex = sizeof(Float4)/sizeof(float);

    for (std::uint32_t fIndex= 0, fC = aStartChannel; fIndex<fMaxIndex; ++fIndex, ++fC)
    {
        if (fC == fStopChannel) fC = aStartChannel;
        fMultiplier[fIndex] = static_cast<float>(aCP[fC].getAudioInputScale() * aCP[fC].getAudioScale());
        fAdder[fIndex]      = aCP[fC].getAudioInputOffset();
    }

    for (std::uint32_t i=0; i<fItems; ++aStartPos, i+=fMaxIndex)
    {
        Float4 *fInput = (Float4 *)&fBuffer[i];
        fResult = (*fInput + fAdder) * fMultiplier;
        for (std::uint32_t fIndex= 0, fC = aStartChannel; fIndex<fMaxIndex; ++fIndex, ++fC)
        {
            if (fC == fStopChannel)
            {
                ++aStartPos;
                fC = aStartChannel;
            }
            aOutputBuffer[fC][aStartPos] = fResult[fIndex];
        }
    }
}
#ifdef __AVX__
void convertBufferFloat256bit(size_t aStartPos, size_t aSize, std::uint32_t aStartChannel, std::uint32_t aChannels, const Channel* aCP, const void *aInputBuffer, std::vector< std::vector<double> >&aOutputBuffer)
{
    std::uint32_t fItems = aSize*aChannels;
    std::uint32_t fStopChannel = aStartChannel + aChannels;
    const float *fBuffer = static_cast<const float*>(aInputBuffer);

    Float8 fMultiplier;
    Float8 fAdder;
    Float8 fResult;
    const std::uint32_t fMaxIndex = sizeof(Float8)/sizeof(float);

    for (std::uint32_t fIndex= 0, fC = aStartChannel; fIndex<fMaxIndex; ++fIndex, ++fC)
    {
        if (fC == fStopChannel) fC = aStartChannel;
        fMultiplier[fIndex] = static_cast<float>(aCP[fC].getAudioInputScale() * aCP[fC].getAudioScale());
        fAdder[fIndex]      = aCP[fC].getAudioInputOffset();
    }

    for (std::uint32_t i=0; i<fItems; ++aStartPos, i+=fMaxIndex)
    {
        Float8 *fInput = (Float8 *)&fBuffer[i];
        fResult = (*fInput + fAdder) * fMultiplier;
        for (std::uint32_t fIndex= 0, fC = aStartChannel; fIndex<fMaxIndex; ++fIndex, ++fC)
        {
            if (fC == fStopChannel)
            {
                ++aStartPos;
                fC = aStartChannel;
            }
            aOutputBuffer[fC][aStartPos] = fResult[fIndex];
        }
    }
}
#endif
} // sse
