#include "AudioUtils.h"
#include "Sound/SoundWaveProcedural.h"
#include "Sound/SoundWave.h"
#include "AudioDevice.h"
#include "AudioDecompress.h"


USoundWaveProcedural* CreateProceduralSoundWaveFromWavData(const TArray<uint8>& RawWaveData)
{
    FWaveModInfo WaveInfo;
    if (!WaveInfo.ReadWaveInfo(RawWaveData.GetData(), RawWaveData.Num()))
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Invalid WAV format"));
        return nullptr;
    }

    const int32 SampleRate = *WaveInfo.pSamplesPerSec;
    const int32 NumChannels = *WaveInfo.pChannels;
    const int32 BitsPerSample = *WaveInfo.pBitsPerSample;
    const int32 BytesPerSample = BitsPerSample / 8;

    const uint8* PCMData = WaveInfo.SampleDataStart;
    const int32 PCMDataSize = WaveInfo.SampleDataSize;

    if (!PCMData || PCMDataSize <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ No PCM data in WAV"));
        return nullptr;
    }

    USoundWaveProcedural* SoundWave = NewObject<USoundWaveProcedural>();
    SoundWave->SetSampleRate(SampleRate);
    SoundWave->NumChannels = NumChannels;
    SoundWave->SoundGroup = SOUNDGROUP_Voice;
    SoundWave->Duration = static_cast<float>(PCMDataSize) / (SampleRate * NumChannels * BytesPerSample);

    SoundWave->QueueAudio(PCMData, PCMDataSize);
    UE_LOG(LogTemp, Warning, TEXT("🔎 SampleRate: %d | Channels: %d | Bits: %d | Size: %d"),
        SampleRate, NumChannels, BitsPerSample, PCMDataSize);

    UE_LOG(LogTemp, Warning, TEXT("🎛️ Created SoundWave from WAV — Rate: %d, Channels: %d, Duration: %.2fs"),
        SampleRate, NumChannels, SoundWave->Duration);

    return SoundWave;
}
