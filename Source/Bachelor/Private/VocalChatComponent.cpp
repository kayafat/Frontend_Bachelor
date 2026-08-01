#include "VocalChatComponent.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWaveProcedural.h"
#include "EngineUtils.h"
// For debug logging
#include "Engine/Engine.h"
#include "Sound/SoundWaveProcedural.h"
#include "AChatBotActor.h"
#include "Kismet/GameplayStatics.h"
#include "AudioUtils.h"

#include "Misc/FileHelper.h"
#include "Misc/Paths.h"



UVocalChatComponent::UVocalChatComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}



void UVocalChatComponent::StartRecording()
{
    UE_LOG(LogTemp, Warning, TEXT("🎙️ StartRecording called — (placeholder)"));
}

void UVocalChatComponent::StopRecordingAndSend()
{
    UE_LOG(LogTemp, Warning, TEXT("🎙️ Triggering full voice pipeline via /api/record-and-process"));

    // JSON payload: just the course name
    FString JsonBody;
    TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    JsonObject->SetStringField("course", DetectedCourse);
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonBody);
    FJsonSerializer::Serialize(JsonObject, Writer);

    TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL("http://localhost:3003/api/record-and-process");
    Request->SetVerb("POST");
    Request->SetHeader("Content-Type", "application/json");
    Request->SetContentAsString(JsonBody);

    Request->OnProcessRequestComplete().BindUObject(this, &UVocalChatComponent::OnAIResponseReceived);  // Same handler works
    Request->ProcessRequest();
    //StopRecording();
}

void UVocalChatComponent::StopRecording()
{
    // Simulate a voice file path (already saved externally via Python)
    RecordedFilePath = FPaths::ProjectDir() + TEXT("Test/voice.wav");

    UE_LOG(LogTemp, Warning, TEXT("🧾 Using DetectedCourse: %s"), *DetectedCourse);
    SendAudioToBackend();
}

void UVocalChatComponent::SetDetectedCourse(const FString& Course)
{
    DetectedCourse = Course;
    UE_LOG(LogTemp, Warning, TEXT("✅ DetectedCourse set to: %s"), *DetectedCourse);
}

void UVocalChatComponent::SendAudioToBackend()
{
    UE_LOG(LogTemp, Display, TEXT("📤 Sending audio file to STT backend: %s"), *RecordedFilePath);

    TArray<uint8> FileData;
    if (!FFileHelper::LoadFileToArray(FileData, *RecordedFilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Failed to load audio file into memory"));
        return;
    }

    FString Boundary = "------------------------boundaryUE5";
    FString BeginBoundary = "--" + Boundary + "\r\n";
    FString ContentDisposition = "Content-Disposition: form-data; name=\"audio\"; filename=\"voice.wav\"\r\n";
    FString ContentType = "Content-Type: audio/wav\r\n\r\n";
    FString EndBoundary = "\r\n--" + Boundary + "--\r\n";

    TArray<uint8> Payload;
    Payload.Append((uint8*)TCHAR_TO_ANSI(*BeginBoundary), BeginBoundary.Len());
    Payload.Append((uint8*)TCHAR_TO_ANSI(*ContentDisposition), ContentDisposition.Len());
    Payload.Append((uint8*)TCHAR_TO_ANSI(*ContentType), ContentType.Len());
    Payload.Append(FileData);
    Payload.Append((uint8*)TCHAR_TO_ANSI(*EndBoundary), EndBoundary.Len());

    TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL("http://localhost:3003/api/stt");
    Request->SetVerb("POST");
    Request->SetHeader("Content-Type", "multipart/form-data; boundary=" + Boundary);
    Request->SetContent(Payload);

    Request->OnProcessRequestComplete().BindUObject(this, &UVocalChatComponent::OnSTTResponseReceived);
    Request->ProcessRequest();
}

void UVocalChatComponent::OnSTTResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful || !Response.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("❌ STT HTTP request failed"));
        return;
    }

    const FString ResponseStr = Response->GetContentAsString();
    UE_LOG(LogTemp, Display, TEXT("✅ STT Response: %s"), *ResponseStr);

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);
    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Failed to parse STT JSON"));
        return;
    }

    FString Transcript;
    if (!JsonObject->TryGetStringField(TEXT("transcript"), Transcript))
    {
        UE_LOG(LogTemp, Error, TEXT("❌ 'transcript' missing from STT response"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("🗣️ Transcribed Text: %s"), *Transcript);

    // Send to semantic-chat
    TSharedRef<IHttpRequest> AIRequest = FHttpModule::Get().CreateRequest();
    AIRequest->SetURL("http://localhost:3003/api/semantic-chat");
    AIRequest->SetVerb("POST");
    AIRequest->SetHeader("Content-Type", "application/json");

    TSharedPtr<FJsonObject> OutJson = MakeShareable(new FJsonObject);
    OutJson->SetStringField("message", Transcript);
    OutJson->SetStringField("course", DetectedCourse);

    //test
    OutJson->SetStringField("sessionTitle", "PDF Chat Session"); // 🔧 Add this line


    FString Payload;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Payload);
    FJsonSerializer::Serialize(OutJson.ToSharedRef(), Writer);
    AIRequest->SetContentAsString(Payload);

    AIRequest->OnProcessRequestComplete().BindUObject(this, &UVocalChatComponent::OnAIResponseReceived);
    AIRequest->ProcessRequest();
}

void UVocalChatComponent::OnAIResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful || !Response.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("❌ AI HTTP request failed"));
        return;
    }

    const FString ResponseStr = Response->GetContentAsString();
    UE_LOG(LogTemp, Display, TEXT("🤖 AI Response: %s"), *ResponseStr);

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);
    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Failed to parse AI JSON"));
        return;
    }

    FString TextResponse;

    JsonObject->TryGetStringField("response", TextResponse);

    PendingAudioSegments.Empty();
    PendingGestureSegments.Empty();
    CurrentAudioSegmentIndex = 0;

    const TArray<TSharedPtr<FJsonValue>>* SegmentsArray;
    if (JsonObject->TryGetArrayField(TEXT("segments"), SegmentsArray))
    {
        for (const TSharedPtr<FJsonValue>& SegmentValue : *SegmentsArray)
        {
            TSharedPtr<FJsonObject> SegmentObject = SegmentValue->AsObject();
            if (SegmentObject.IsValid())
            {
                FString SegmentWavPath;
                FString SegmentGesture = TEXT("talk_pose");

                SegmentObject->TryGetStringField(TEXT("wav_path"), SegmentWavPath);
                SegmentObject->TryGetStringField(TEXT("gesture"), SegmentGesture);

                if (!SegmentWavPath.IsEmpty())
                {
                    PendingAudioSegments.Add(SegmentWavPath);
                    PendingGestureSegments.Add(SegmentGesture);

                    UE_LOG(LogTemp, Warning, TEXT("🎭 Segment gesture added: %s"), *SegmentGesture);
                }
            }
        }
    }
    else
    {
        FString AudioURL;
        JsonObject->TryGetStringField(TEXT("audio_url"), AudioURL);

        FString GestureName = TEXT("talk_pose");
        JsonObject->TryGetStringField(TEXT("gesture"), GestureName);

        if (!AudioURL.IsEmpty())
        {
            PendingAudioSegments.Add(AudioURL);
            PendingGestureSegments.Add(GestureName);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("🗨️ Text: %s"), *TextResponse);

    UE_LOG(LogTemp, Warning, TEXT("🔊 Audio segments found: %d"), PendingAudioSegments.Num());
    UE_LOG(LogTemp, Warning, TEXT("📦 OwningBot is %s"), OwningBot ? TEXT("VALID") : TEXT("NULL"));

    if (PendingAudioSegments.Num() > 0)
    {
        PlayNextAudioSegment();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ No audio segments found — cannot play."));
    }

}

void UVocalChatComponent::PlayNextAudioSegment()
{
    if (CurrentAudioSegmentIndex >= PendingAudioSegments.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("✅ All audio segments played."));
        return;
    }

    const int32 SegmentIndex = CurrentAudioSegmentIndex;

    const FString NextWavPath = PendingAudioSegments[SegmentIndex];

    FString GestureName = TEXT("talk_pose");
    if (PendingGestureSegments.IsValidIndex(SegmentIndex))
    {
        GestureName = PendingGestureSegments[SegmentIndex];
    }

    CurrentAudioSegmentIndex++;

    UE_LOG(LogTemp, Warning, TEXT("▶️ Playing audio segment %d/%d: %s"),
        CurrentAudioSegmentIndex,
        PendingAudioSegments.Num(),
        *NextWavPath
    );

    UE_LOG(LogTemp, Warning, TEXT("🎭 Playing gesture for segment: %s"), *GestureName);

    if (OwningBot)
    {
        OwningBot->CurrentGestureDuration = 0.0f;
        OwningBot->PlayGestureByName(GestureName);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ OwningBot is NULL — cannot play gesture."));
    }

    if (OwningBot)
    {
        const float GestureDuration = OwningBot->CurrentGestureDuration;

        UE_LOG(
            LogTemp,
            Warning,
            TEXT("🎭 Gesture duration: %.2f seconds"),
            GestureDuration
        );

        OwningBot->CurrentA2FWavPath = NextWavPath;
        OwningBot->PlayAudio2FaceCurrentWav();

        float AudioDuration = 4.0f;

        if (FPaths::FileExists(NextWavPath))
        {
            TArray<uint8> AudioData;
            if (FFileHelper::LoadFileToArray(AudioData, *NextWavPath))
            {
                USoundWaveProcedural* SoundWave = CreateProceduralSoundWaveFromWavData(AudioData);
                if (SoundWave)
                {
                    AudioDuration = SoundWave->Duration;
                }
            }
        }

        const float SegmentDuration = FMath::Max(AudioDuration, GestureDuration);
        const float Delay = FMath::Max(SegmentDuration + 0.4f, 0.5f);

        UE_LOG(
            LogTemp,
            Warning,
            TEXT(
                "⏱️ Audio duration: %.2fs | "
                "Gesture duration: %.2fs | "
                "Next segment in: %.2fs"
            ),
            AudioDuration,
            GestureDuration,
            Delay
        );


        GetWorld()->GetTimerManager().SetTimer(
            AudioSegmentTimerHandle,
            this,
            &UVocalChatComponent::PlayNextAudioSegment,
            Delay,
            false
        );
    }

}