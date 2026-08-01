#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FollowingComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BACHELOR_API UFollowingComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UFollowingComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Follow")
    AActor* TargetActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Follow")
    float FollowDistance = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Follow")
    float MoveSpeed = 3.0f;


        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    TArray<USoundBase*> FollowSounds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float SoundPlayInterval = 5.0f; // every 5 seconds

    FTimerHandle SoundTimerHandle;
    UFUNCTION()
    void PlayFollowSound();

};
