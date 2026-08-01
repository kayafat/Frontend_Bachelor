#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/IHttpRequest.h"
#include "UPDFChatComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BACHELOR_API UPDFChatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPDFChatComponent();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "PDF Chat")
	void SendMessageToBackend(const FString& UserMessage);

	// Editable in Unreal Editor per NPC
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PDF Chat")
	FString CourseName = TEXT("default_course");

private:
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
