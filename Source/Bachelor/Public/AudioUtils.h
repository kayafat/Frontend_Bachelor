#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundWaveProcedural.h"

USoundWaveProcedural* CreateProceduralSoundWaveFromWavData(const TArray<uint8>& RawWaveData);
