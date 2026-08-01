/* 28.05.25
#pragma once
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "VocalChatComponent.generated.h"  // ✅ MUST be last include

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BACHELOR_API UVocalChatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UVocalChatComponent();

    
UFUNCTION(BlueprintCallable, Category = "Voice Chat")
void StartRecording();


UFUNCTION(BlueprintCallable, Category = "Voice Chat")
void StopRecordingAndSend();

FString DetectedCourse;

UFUNCTION(BlueprintCallable)
void SetDetectedCourse(const FString& Course);


private:
    FString RecordedFilePath;
    FString DetectedCourse;
    FTimerHandle AudioSegmentTimerHandle;

    TArray<FString> PendingAudioSegments;
    int32 CurrentAudioSegmentIndex = 0;

    void PlayNextAudioSegment();

    void SendAudioToBackend();
    void OnSTTResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    void OnAIResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    void DownloadAndPlayAudio(const FString& AudioURL);
    void OnAudioDownloaded(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};


*/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
class AAChatBotActor;

#include "VocalChatComponent.generated.h" // ✅ Must be last include

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BACHELOR_API UVocalChatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UVocalChatComponent();
    UPROPERTY()
    AAChatBotActor* OwningBot;

    // 📌 These are now just placeholders if you're triggering externally
    UFUNCTION(BlueprintCallable, Category = "Voice Chat")
    void StartRecording();

    UFUNCTION(BlueprintCallable, Category = "Voice Chat")
    void StopRecording();

    UFUNCTION(BlueprintCallable, Category = "Voice Chat")
    void StopRecordingAndSend();

    UFUNCTION(BlueprintCallable, Category = "Voice Chat")
    void SetDetectedCourse(const FString& Course);

private:

    FString RecordedFilePath;
    FString DetectedCourse;

    // SEGMENT AUDIO SYSTEM
    UPROPERTY()
    TArray<FString> PendingAudioSegments;

    UPROPERTY()
    TArray<FString> PendingGestureSegments;

    int32 CurrentAudioSegmentIndex = 0;

    FTimerHandle AudioSegmentTimerHandle;

    UFUNCTION()
    void PlayNextAudioSegment();

    void SendAudioToBackend();
    void OnSTTResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    void OnAIResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    void DownloadAndPlayAudio(const FString& AudioURL);
    void OnAudioDownloaded(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
