#include "UPDFChatComponent.h"
#include "HttpModule.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "Engine/Engine.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UChatWidget.h"
#include "BachelorPlayerController.h"
#include "EngineUtils.h"  // <-- for TActorIterator

#include "AChatBotActor.h"


UPDFChatComponent::UPDFChatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPDFChatComponent::BeginPlay()
{
	Super::BeginPlay();
}





void UPDFChatComponent::SendMessageToBackend(const FString& UserMessage)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

	Request->SetURL(TEXT("http://localhost:3003/api/semantic-chat"));
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// JSON payload with message AND dynamic course name
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("message", UserMessage);
	JsonObject->SetStringField("course", CourseName);  // ✅ Dynamic

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->SetContentAsString(RequestBody);
	Request->OnProcessRequestComplete().BindUObject(this, &UPDFChatComponent::OnResponseReceived);
	Request->ProcessRequest();
}

void UPDFChatComponent::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful || !Response.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("❌ HTTP request failed."));
		return;
	}

	const FString ResponseStr = Response->GetContentAsString();
	UE_LOG(LogTemp, Display, TEXT("✅ Raw Response: %s"), *ResponseStr);

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);
	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Failed to deserialize JSON."));
		return;
	}

	FString AIReply;
	if (!JsonObject->TryGetStringField(TEXT("response"), AIReply))
	{
		UE_LOG(LogTemp, Error, TEXT("❌ 'response' field not found in JSON."));
		return;
	}

	// Get the player controller and display in widget
	if (UWorld* World = GetWorld())
	{
		if (ABachelorPlayerController* PC = Cast<ABachelorPlayerController>(World->GetFirstPlayerController()))
		{
			if (UChatWidget* Widget = PC->GetChatWidgetInstance())
			{
				Widget->DisplayAIResponse(AIReply);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("⚠️ Chat widget is null."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("⚠️ PlayerController is not of expected type ABachelorPlayerController."));
		}
	}

	// Optional debug display
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Cyan, FString::Printf(TEXT("AI says: %s"), *AIReply));
	}
}
