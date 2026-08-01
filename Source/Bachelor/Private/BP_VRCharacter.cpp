#include "BP_VRCharacter.h"
#include "VocalChatComponent.h"

ABP_VRCharacter::ABP_VRCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    VocalChat = CreateDefaultSubobject<UVocalChatComponent>(TEXT("VocalChat"));
}
