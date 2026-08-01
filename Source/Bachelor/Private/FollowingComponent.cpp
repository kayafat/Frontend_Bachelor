#include "FollowingComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Sound/SoundBase.h"

UFollowingComponent::UFollowingComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UFollowingComponent::BeginPlay()
{
    Super::BeginPlay();

    if (FollowSounds.Num() > 0)
    {
        GetWorld()->GetTimerManager().SetTimer(
            SoundTimerHandle,
            this,
            &UFollowingComponent::PlayFollowSound,
            SoundPlayInterval,
            true
        );
    }


    if (!TargetActor)
    {
        TargetActor = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    }
}

void UFollowingComponent::PlayFollowSound()
{
    if (FollowSounds.Num() == 0 || !GetOwner()) return;

    int32 Index = FMath::RandRange(0, FollowSounds.Num() - 1);
    USoundBase* ChosenSound = FollowSounds[Index];

    UGameplayStatics::PlaySoundAtLocation(this, ChosenSound, GetOwner()->GetActorLocation());
}


void UFollowingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!TargetActor) return;

    AActor* Owner = GetOwner();
    if (!Owner) return;

    FVector TargetLocation = TargetActor->GetActorLocation();
    FVector CurrentLocation = Owner->GetActorLocation();

    // Keep the bot at player's head height
    TargetLocation.Z += 5.0f;
    CurrentLocation.Z = TargetLocation.Z;

    float Distance = FVector::Dist(CurrentLocation, TargetLocation);

    // --- Always face the player ---
    FVector LookDirection = (TargetLocation - Owner->GetActorLocation()).GetSafeNormal();
    FRotator LookRotation = LookDirection.Rotation();
    LookRotation.Pitch = 0.0f;
    LookRotation.Roll = 0.0f;
    Owner->SetActorRotation(FMath::RInterpTo(Owner->GetActorRotation(), LookRotation, DeltaTime, 5.0f)); // Smooth turning

    // --- Stop if close enough ---
    if (Distance <= FollowDistance)
    {
        return;
    }

    // --- Smooth movement toward player ---
    FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, MoveSpeed);
    Owner->SetActorLocation(NewLocation);
}

