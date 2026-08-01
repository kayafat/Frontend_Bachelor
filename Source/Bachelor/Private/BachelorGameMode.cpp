
#include "BachelorGameMode.h"
#include "IXRTrackingSystem.h"
#include "Engine/Engine.h"
#include "BachelorPlayerController.h" // ✅ again



void ABachelorGameMode::BeginPlay()
{
    Super::BeginPlay();

    if (GEngine && GEngine->XRSystem.IsValid() && GEngine->XRSystem->IsHeadTrackingAllowed())
    {
        UE_LOG(LogTemp, Warning, TEXT("🎧 VR is active"));
        DefaultPawnClass = VRPawnClass;
   }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("🖥️ VR not detected, using dev pawn"));
        DefaultPawnClass = DevPawnClass;
    }
}


ABachelorGameMode::ABachelorGameMode()
{
    PlayerControllerClass = ABachelorPlayerController::StaticClass(); // ✅ SET IT HERE
}