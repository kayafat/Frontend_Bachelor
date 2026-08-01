#include "UChatWidget.h"
#include "Components/TextBlock.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Components/Button.h"
#include "UPDFChatComponent.h" // Adjust this include if your path is different
#include "AChatBotActor.h"              // ✅ Needed for AAChatBotActor
#include "EngineUtils.h"                // ✅ Nee
#include "GameFramework/PlayerController.h"

void UChatWidget::DisplayAIResponse(const FString& Response)
{
	if (ResponseText)
	{
		ResponseText->SetText(FText::FromString(Response));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("❌ ResponseText is not bound in the widget!"));
	}
}


void UChatWidget::OnSendMessage()
{
	if (MessageInput)
	{
		const FString Message = MessageInput->GetText().ToString(); // ✅ now declared
		UE_LOG(LogTemp, Display, TEXT("📤 User message: %s"), *Message);

		// Optionally clear input
		MessageInput->SetText(FText::GetEmpty());

		if (!Message.IsEmpty())
		{
			if (UWorld* World = GetWorld())
			{
				for (TActorIterator<AAChatBotActor> It(World); It; ++It)
				{
					AAChatBotActor* Bot = *It;
					if (Bot->bPlayerIsNearby)
					{
						if (UPDFChatComponent* Comp = Bot->FindComponentByClass<UPDFChatComponent>())
						{
							UE_LOG(LogTemp, Warning, TEXT("📤 Sending message to bot: %s"), *Bot->GetName());
							Comp->SendMessageToBackend(Message);
							UE_LOG(LogTemp, Warning, TEXT("🧪 User typed message: %s"), *Message);

						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("❌ No UPDFChatComponent on bot: %s"), *Bot->GetName());
						}
						return;
					}
				}

				UE_LOG(LogTemp, Warning, TEXT("❌ No nearby bot to send message to"));
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("❌ MessageInput not bound in widget!"));
	}
}


void UChatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SendButton)
	{
		SendButton->OnClicked.AddDynamic(this, &UChatWidget::OnSendMessage);
	}
}