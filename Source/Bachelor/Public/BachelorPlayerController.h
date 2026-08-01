#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "UChatWidget.h"
#include "BachelorPlayerController.generated.h"

class UChatWidget;

UCLASS()
class BACHELOR_API ABachelorPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    ABachelorPlayerController(); // ✅ Add this line to declare the constructor


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chat")
    TSubclassOf<UChatWidget> ChatWidgetClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Chat")
    UChatWidget* ChatWid;

    UFUNCTION(BlueprintCallable)
    UChatWidget* GetChatWidgetInstance() const { return ChatWid; }

    UFUNCTION()
    void TryOpenChatForNearbyBot();

    UFUNCTION()
    void DebugPrintTKeyPressed();

   

    UFUNCTION()
    void TriggerVoiceRecordingAPI();

};
