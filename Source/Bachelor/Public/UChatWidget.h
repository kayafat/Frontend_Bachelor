#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UChatWidget.generated.h"

UCLASS()
class BACHELOR_API UChatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Displays the AI response in the text block */
	UFUNCTION(BlueprintCallable, Category = "Chat")
	void DisplayAIResponse(const FString& Response);

	UFUNCTION(BlueprintCallable, Category = "Chat")
	void OnSendMessage();

	UPROPERTY(meta = (BindWidget))
	class UMultiLineEditableTextBox* MessageInput;

	UPROPERTY(meta = (BindWidget))
	class UButton* SendButton;

	virtual void NativeConstruct() override;


protected:
	/** The TextBlock widget that will show the AI's response */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ResponseText;
};
