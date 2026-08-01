#include "BachelorPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "UChatWidget.h"
#include "AChatBotActor.h"
#include "UPDFChatComponent.h"
#include "EngineUtils.h"
#include "UObject/ConstructorHelpers.h" // Needed for FClassFinder
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

#include "VocalChatComponent.h" // For voice chat component

ABachelorPlayerController::ABachelorPlayerController()
{
    // Assign the widget class from asset path /Script/UMGEditor.WidgetBlueprint'/Game/WBP_ChatUI_Cpp.WBP_ChatUI_Cpp'
    static ConstructorHelpers::FClassFinder<UUserWidget> ChatWidgetBPClass(TEXT("/Game/WBP_ChatUI_Cpp"));
    if (ChatWidgetBPClass.Succeeded())
    {
        ChatWidgetClass = ChatWidgetBPClass.Class;
    }
}

void ABachelorPlayerController::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("✅ BachelorPlayerController is now in control!"));

    if (ChatWidgetClass)
    {
        ChatWid = CreateWidget<UChatWidget>(this, ChatWidgetClass);
        if (ChatWid)
        {
            ChatWid->AddToViewport();
            ChatWid->SetVisibility(ESlateVisibility::Hidden);
            UE_LOG(LogTemp, Warning, TEXT("✅ Chat widget created and hidden."));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("❌ Failed to create Chat widget."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ ChatWidgetClass not set in PlayerController."));
    }

    UVocalChatComponent* VoiceComp = NewObject<UVocalChatComponent>(this, UVocalChatComponent::StaticClass());
    if (VoiceComp)
    {
        VoiceComp->RegisterComponent();  // required to tick or work properly
        // VoiceComp->StopRecordingAndSend();  // simulate input
        UE_LOG(LogTemp, Warning, TEXT("🎤 VocalChatComponent created and triggered"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Could not create VocalChatComponent"));
    }
}



void ABachelorPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    InputComponent->BindKey(EKeys::Z, IE_Pressed, this, &ABachelorPlayerController::TriggerVoiceRecordingAPI);
    
    InputComponent->BindAction("OpenChat", IE_Pressed, this, &ABachelorPlayerController::TryOpenChatForNearbyBot);
    InputComponent->BindKey(EKeys::T, IE_Pressed, this, &ABachelorPlayerController::DebugPrintTKeyPressed);
}

void ABachelorPlayerController::DebugPrintTKeyPressed()
{
    UE_LOG(LogTemp, Warning, TEXT("🟢 T key was pressed"));
}

void ABachelorPlayerController::TryOpenChatForNearbyBot()
{
    UE_LOG(LogTemp, Warning, TEXT("🔍 T key pressed - checking for nearby bot"));

    UWorld* World = GetWorld();
    if (!World || !ChatWid)
    {
        return;
    }

    bool bBotFound = false;

    for (TActorIterator<AAChatBotActor> It(World); It; ++It)
    {
        AAChatBotActor* Bot = *It;
        if (Bot && Bot->bPlayerIsNearby)
        {
            bBotFound = true;

            if (UPDFChatComponent* Comp = Bot->FindComponentByClass<UPDFChatComponent>())
            {
                // Optional: set CourseName, etc.
            }

            ChatWid->SetVisibility(ESlateVisibility::Visible);
            break;
        }
    }

    if (!bBotFound)
    {
        // Prevent opening chat outside of bot range
        ChatWid->SetVisibility(ESlateVisibility::Hidden);
        UE_LOG(LogTemp, Warning, TEXT("📭 No nearby bot found. Chat hidden."));
    }
}
void ABachelorPlayerController::TriggerVoiceRecordingAPI()
{
    UE_LOG(LogTemp, Warning, TEXT("🎙️ Z key pressed — Triggering voice record with course"));

    UWorld* World = GetWorld();
    FString CourseToSend = "default_course"; // fallback

    // 🔍 Look for nearby bot
    for (TActorIterator<AAChatBotActor> It(World); It; ++It)
    {
        AAChatBotActor* Bot = *It;
        if (Bot && Bot->bPlayerIsNearby)
        {
            CourseToSend = Bot->AssignedCourse;
            UE_LOG(LogTemp, Warning, TEXT("📘 Found nearby bot with course: %s"), *CourseToSend);

            if (UVocalChatComponent* Vocal = Bot->FindComponentByClass<UVocalChatComponent>())
            {
                Vocal->SetDetectedCourse(CourseToSend);
                Vocal->OwningBot = Bot; // ✅ Make sure it's linked
                Vocal->StopRecordingAndSend(); // ✅ This does the STT + semantic chat + TTS
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("❌ No VocalChatComponent on bot!"));
            }

            return; // Only one bot needed
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("📭 No nearby bot found — skipping voice trigger"));
}

/*
void ABachelorPlayerController::TriggerVoiceRecordingAPI()
{
    UE_LOG(LogTemp, Warning, TEXT("🎙️ Z key pressed — Triggering voice record with course"));

    UWorld* World = GetWorld();
    FString CourseToSend = "default_course"; // fallback

    // 🔍 Look for nearby bot
    for (TActorIterator<AAChatBotActor> It(World); It; ++It)
    {
        AAChatBotActor* Bot = *It;
        if (Bot && Bot->bPlayerIsNearby)
        {
            CourseToSend = Bot->AssignedCourse;
            UE_LOG(LogTemp, Warning, TEXT("📘 Found nearby bot with course: %s"), *CourseToSend);
            break;
        }
    }

    // Build JSON body
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
    JsonObject->SetStringField(TEXT("course"), CourseToSend);

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    // Send request
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL("http://127.0.0.1:3003/api/record-and-process");
    Request->SetVerb("POST");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetContentAsString(OutputString);

    // ✅ Increase timeout to allow backend to finish
    Request->SetTimeout(60.0f);



    Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            if (bWasSuccessful && Response.IsValid())
            {
                FString Result = Response->GetContentAsString();
                UE_LOG(LogTemp, Warning, TEXT("✅ Received Transcript/Response: %s"), *Result);
            }
            else
            {
                FString ErrorMessage = Response.IsValid() ? Response->GetContentAsString() : TEXT("No response received");
                UE_LOG(LogTemp, Error, TEXT("❌ Failed to trigger backend for recording. Status: %d, Response: %s"),
                    Response.IsValid() ? Response->GetResponseCode() : -1,
                    *ErrorMessage);
            }
        });



    Request->ProcessRequest();
}
*/