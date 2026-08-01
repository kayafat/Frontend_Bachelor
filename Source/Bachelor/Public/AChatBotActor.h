#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UPDFChatComponent.h"            // For chat logic
#include "Components/SphereComponent.h"   // For proximity detection
#include "Components/WidgetComponent.h"
#include "Components/AudioComponent.h"

#include "UObject/NoExportTypes.h"
#include "AChatBotActor.generated.h"       // ✅ This must be last

UCLASS()
class BACHELOR_API AAChatBotActor : public ACharacter
{
	GENERATED_BODY()

public:

	// Constructor
	AAChatBotActor();
	bool bHasTriggeredInteraction = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* VoiceAudioComponent;


	UFUNCTION(BlueprintCallable)
	void PlayVoiceFromURL(const FString& AudioURL);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
	USkeletalMeshComponent* VisualMesh;

	/** Floating prompt shown when player is nearby */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* TalkPromptWidget;


	/** Chat handler component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UPDFChatComponent* ChatComponent;

	/** Sphere for detecting player proximity */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	USphereComponent* InteractionSphere;

	/** The name of the course (PDF) this bot is assigned to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chat", meta = (ExposeOnSpawn = true))
	FString AssignedCourse;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PlayGestureByName(const FString& GestureName);


	/** Tracks whether the player is in interaction range */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool bPlayerIsNearby = false;

	//NEU AB HIER

	UPROPERTY(BlueprintReadWrite, Category = "Animation")
	float CurrentGestureDuration = 0.0f;

	UPROPERTY(BlueprintReadWrite, Category="Audio2Face")
	FString CurrentA2FWavPath;

	UFUNCTION(BlueprintImplementableEvent, Category="Audio2Face")
	void PlayAudio2FaceCurrentWav();
	//BIS HIER


	//XXXXX
	UPROPERTY()
	AActor* TrackedPlayer = nullptr;
	//XXXXX
protected:
	// Called when the game starts or the actor is spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Input bindings, unused for now but can be extended
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Overlap events for interaction detection
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
