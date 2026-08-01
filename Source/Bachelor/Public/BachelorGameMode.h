#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BachelorPlayerController.h" // ✅ include this

#include "BachelorGameMode.generated.h"

UCLASS()
class BACHELOR_API ABachelorGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    ABachelorGameMode(); // 👈 constructor

    UPROPERTY(EditDefaultsOnly, Category = "VR")
    TSubclassOf<APawn> VRPawnClass;

    UPROPERTY(EditDefaultsOnly, Category = "VR")
    TSubclassOf<APawn> DevPawnClass;
};
