#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FollowingRobot.generated.h"

UCLASS()
class BACHELOR_API AFollowingRobot : public AActor

{
    GENERATED_BODY()

public:
    AFollowingRobot();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Follow")
    AActor* TargetActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Follow")
    float FollowDistance = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Follow")
    float MoveSpeed = 500.0f;
};
