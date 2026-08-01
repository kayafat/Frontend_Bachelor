#include "FollowingRobot.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h" // ✅ Must be included BEFORE you try to use GetPlayerCharacter

AFollowingRobot::AFollowingRobot()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AFollowingRobot::BeginPlay()
{
    Super::BeginPlay();

    if (!TargetActor)
    {
        // Automatically assign player character as target
        TargetActor = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    }
}

void AFollowingRobot::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!TargetActor) return;

    FVector TargetLocation = TargetActor->GetActorLocation();
    FVector CurrentLocation = GetActorLocation();

    FVector Direction = (CurrentLocation - TargetLocation).GetSafeNormal();
    FVector DesiredLocation = TargetLocation + Direction * FollowDistance;

    FVector NewLocation = FMath::VInterpTo(CurrentLocation, DesiredLocation, DeltaTime, MoveSpeed);
    SetActorLocation(NewLocation);

    FRotator LookAtRotation = (TargetLocation - NewLocation).Rotation();
    LookAtRotation.Pitch = 0.0f;
    LookAtRotation.Roll = 0.0f;
    SetActorRotation(LookAtRotation);
}
