#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VocalChatComponent.h"
#include "BP_VRCharacter.generated.h"

UCLASS()
class BACHELOR_API ABP_VRCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ABP_VRCharacter();

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Voice Chat")
    UVocalChatComponent* VocalChat;

 

};
