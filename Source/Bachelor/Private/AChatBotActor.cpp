#include "AChatBotActor.h"
#include "UPDFChatComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "BP_VRCharacter.h" // Or your actual class name
#include "Kismet/GameplayStatics.h"
#include "AudioUtils.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundAttenuation.h"
#include "Sound/AudioSettings.h" // ✅ Required for FAttenuationSettings

#include "VocalChatComponent.h"  // ✅ Add this!


// Sets default values
AAChatBotActor::AAChatBotActor()
{
    PrimaryActorTick.bCanEverTick = true;

    // 🟢 Add a RootComponent so other components have a base
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    // 🧠 Chat logic component
    ChatComponent = CreateDefaultSubobject<UPDFChatComponent>(TEXT("UPDFChatComponent"));

    // 🟢 Proximity detection sphere
    InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
    InteractionSphere->InitSphereRadius(200.0f);
    InteractionSphere->SetupAttachment(RootComponent);
    InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    InteractionSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    InteractionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    InteractionSphere->SetGenerateOverlapEvents(true);

    // 💬 Talk prompt UI widget
    TalkPromptWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("TalkPromptWidget"));
    TalkPromptWidget->SetupAttachment(RootComponent);
    TalkPromptWidget->SetWidgetSpace(EWidgetSpace::Screen);
    TalkPromptWidget->SetDrawSize(FVector2D(200.0f, 50.0f));
    TalkPromptWidget->SetVisibility(false);

    // 👤 Mesh for visual appearance
    VisualMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VisualMesh"));
    VisualMesh->SetupAttachment(RootComponent);

    VoiceAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("VoiceAudioComponent"));
    VoiceAudioComponent->SetupAttachment(RootComponent);
    VoiceAudioComponent->bAutoActivate = false; // We control when it plays
    VoiceAudioComponent->bAllowSpatialization = true;
    VoiceAudioComponent->bOverrideAttenuation = true;
    VoiceAudioComponent->SetVolumeMultiplier(1.0f);
    UE_LOG(LogTemp, Warning, TEXT("🎚️ VoiceAudioComponent volume set to 1.0f"));

    FSoundAttenuationSettings AttenuationSettings;
    AttenuationSettings.bSpatialize = true;
    AttenuationSettings.AttenuationShape = EAttenuationShape::Sphere;
    AttenuationSettings.AttenuationShapeExtents = FVector(400.0f);  // Radius
    AttenuationSettings.FalloffDistance = 800.0f;

    VoiceAudioComponent->AttenuationOverrides = AttenuationSettings;


}

// Called when the game starts or when spawned
void AAChatBotActor::BeginPlay()
{
    Super::BeginPlay();

    if (!VoiceAudioComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ VoiceAudioComponent is NULL!"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("🔊 VoiceAudioComponent state: IsPlaying: %s"),
        VoiceAudioComponent->IsPlaying() ? TEXT("true") : TEXT("false"));

   // PlayVoiceFromURL(TEXT("http://localhost:3003/audio/reply.wav"));
    /* if (VoiceAudioComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("🎧 VoiceAudioComponent is valid and ready"));
    }
    FTimerHandle DummyTimer;
    GetWorldTimerManager().SetTimer(DummyTimer, [this]()
        {
            this->PlayVoiceFromURL(TEXT("http://localhost:3003/audio/police-sirens-316719.mp3"));
        }, 5.0f, false); // Plays after 5 seconds
*/

    
   /*FString FullPath = FPaths::ProjectDir() + TEXT("generated/Applaus.wav");

    TArray<uint8> RawData;
    if (FFileHelper::LoadFileToArray(RawData, *FullPath))
    {
        USoundWaveProcedural* Sound = CreateProceduralSoundWaveFromWavData(RawData);
        if (Sound && VoiceAudioComponent)
        {
            VoiceAudioComponent->SetSound(Sound);
            VoiceAudioComponent->Play();
            UE_LOG(LogTemp, Warning, TEXT("✅ Played local WAV file from: %s"), *FullPath);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("❌ Failed to create or assign SoundWave"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Failed to load WAV file from: %s"), *FullPath);
    }*/ 
  


    InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &AAChatBotActor::OnOverlapBegin);
    InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &AAChatBotActor::OnOverlapEnd);

    if (TalkPromptWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("🛠️ TalkPromptWidget initialized"));

        if (TalkPromptWidget->GetWidgetClass())
        {
            UE_LOG(LogTemp, Warning, TEXT("🛠️ Widget Class: %s"),
                *TalkPromptWidget->GetWidgetClass()->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("❌ WidgetClass is NULL on TalkPromptWidget"));
        }

        if (TalkPromptWidget->GetWidget())
        {
            UE_LOG(LogTemp, Warning, TEXT("✅ Widget instance is constructed!"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("❌ Widget instance is NULL — probably no class assigned"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ TalkPromptWidget is NULL in BeginPlay!"));
    }

}

// Called every frame
void AAChatBotActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Optional: Draw debug sphere to see interaction area
    DrawDebugSphere(GetWorld(), GetActorLocation(), InteractionSphere->GetScaledSphereRadius(), 16, FColor::Green, false, 0.1f);

    if (TalkPromptWidget)
    {
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC)
        {
            FVector CamLoc;
            FRotator CamRot;
            PC->GetPlayerViewPoint(CamLoc, CamRot);

            FVector ToCam = CamLoc - TalkPromptWidget->GetComponentLocation();
            FRotator LookAt = FRotationMatrix::MakeFromX(ToCam).Rotator();
            TalkPromptWidget->SetWorldRotation(LookAt);
        }
    }

    if (TrackedPlayer)
    {
        FVector TargetLocation = TrackedPlayer->GetActorLocation();
        FVector CurrentLocation = GetActorLocation();

        FVector LookDirection = (TargetLocation - CurrentLocation).GetSafeNormal();
        FRotator LookRotation = LookDirection.Rotation();
        LookRotation.Pitch = 0.0f;
        LookRotation.Roll = 0.0f;

        SetActorRotation(FMath::RInterpTo(GetActorRotation(), LookRotation, DeltaTime, 5.0f));
    }



}

void AAChatBotActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this && !bHasTriggeredInteraction)
    {
        if (ACharacter* OverlappingCharacter = Cast<ACharacter>(OtherActor))
        {
            bHasTriggeredInteraction = true; // ✅ Prevent multiple triggers

            bPlayerIsNearby = true;
            UE_LOG(LogTemp, Warning, TEXT("✅ Valid player character nearby"));
            TrackedPlayer = OverlappingCharacter;  // 🔍 Save reference

            if (UVocalChatComponent* Vocal = OverlappingCharacter->FindComponentByClass<UVocalChatComponent>())
            {
                if (AssignedCourse.IsEmpty())
                {
                    UE_LOG(LogTemp, Error, TEXT("❌ Bot's AssignedCourse is EMPTY! Please set it in editor."));
                }
                else
                {
                    Vocal->SetDetectedCourse(AssignedCourse);
                    Vocal->OwningBot = this;  // ✅ Link the bot here
                    UE_LOG(LogTemp, Warning, TEXT("📘 Course passed to VocalChat: %s"), *AssignedCourse);
                }

                // Optional: Start voice input
                // Vocal->StopRecordingAndSend();
            }


            if (TalkPromptWidget)
            {
                TalkPromptWidget->SetVisibility(true);
            }

            if (ChatComponent)
            {
                ChatComponent->CourseName = AssignedCourse;
                UE_LOG(LogTemp, Warning, TEXT("📘 ChatComponent CourseName set to: %s"), *AssignedCourse);
            }
        }
    }
}



/*
 if (UVocalChatComponent* Vocal = OverlappingCharacter->FindComponentByClass<UVocalChatComponent>())
            {
                if (AssignedCourse.IsEmpty())
                {
                    UE_LOG(LogTemp, Error, TEXT("❌ Bot's AssignedCourse is EMPTY! Please set it in editor."));
                }
                else
                {
                    Vocal->SetDetectedCourse(AssignedCourse);
                    UE_LOG(LogTemp, Warning, TEXT("📘 Course passed to VocalChat: %s"), *AssignedCourse);


                }

                //Vocal->StopRecordingAndSend();  // ✅ Only called once per overlap now
            }*/

void AAChatBotActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor != this)
    {
        if (ACharacter* OverlappingCharacter = Cast<ACharacter>(OtherActor))
        {
            bPlayerIsNearby = false;
            bHasTriggeredInteraction = false; // ✅ Allow re-interaction later
            UE_LOG(LogTemp, Warning, TEXT("🚪 Player left interaction range"));
            if (TrackedPlayer == OverlappingCharacter)
            {
                TrackedPlayer = nullptr;
            }
            if (TalkPromptWidget)
            {
                TalkPromptWidget->SetVisibility(false);
            }
        }
    }
}
void AAChatBotActor::PlayVoiceFromURL(const FString& AudioURL)
{
    UE_LOG(LogTemp, Warning, TEXT("🔊 PlayVoiceFromURL CALLED with URL: %s"), *AudioURL);

    TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(AudioURL);
    Request->SetVerb("GET");

    Request->OnProcessRequestComplete().BindLambda([this, AudioURL](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            if (!bWasSuccessful || !Response.IsValid())
            {
                UE_LOG(LogTemp, Error, TEXT("❌ HTTP failed for URL: %s"), *AudioURL);
                return;
            }

            const TArray<uint8>& AudioData = Response->GetContent();
            UE_LOG(LogTemp, Warning, TEXT("📦 Downloaded audio size: %d bytes"), AudioData.Num());

            USoundWaveProcedural* Sound = CreateProceduralSoundWaveFromWavData(AudioData);
            if (!Sound)
            {
                UE_LOG(LogTemp, Error, TEXT("❌ CreateProceduralSoundWaveFromWavData() returned NULL"));
                return;
            }

            if (Sound->Duration <= 0.0f)
            {
                UE_LOG(LogTemp, Error, TEXT("❌ SoundWave duration is 0. Probably no valid audio data."));
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("✅ SoundWave duration: %.2f seconds"), Sound->Duration);
            }

            if (!VoiceAudioComponent)
            {
                UE_LOG(LogTemp, Error, TEXT("❌ VoiceAudioComponent is NULL"));
                return;
            }

            VoiceAudioComponent->Stop();
            VoiceAudioComponent->SetSound(Sound);

            // 🔊 Ensure volume/spatialization
            VoiceAudioComponent->SetVolumeMultiplier(1.0f);
            VoiceAudioComponent->bAllowSpatialization = false;
            VoiceAudioComponent->bOverrideAttenuation = false;

            UE_LOG(LogTemp, Warning, TEXT("🎧 VoiceAudioComponent settings: Volume = %f, Spatialized = %s"),
                VoiceAudioComponent->VolumeMultiplier,
                VoiceAudioComponent->bAllowSpatialization ? TEXT("true") : TEXT("false"));

            VoiceAudioComponent->Play();

            UE_LOG(LogTemp, Warning, TEXT("✅ Playing sound from downloaded data (%s)"), *AudioURL);
        });

    Request->ProcessRequest();
}







// Input setup (optional for later use)
void AAChatBotActor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}
