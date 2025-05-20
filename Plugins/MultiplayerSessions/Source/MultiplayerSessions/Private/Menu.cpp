// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch)
{
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;

	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* World = GetWorld();
	if (World) {
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController) {
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->bShowMouseCursor = true;
		}
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance) {
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	if (MultiplayerSessionsSubsystem) {
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	}
	else {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, TEXT("MultiplayerSessionsSubsystem is null"));
		}
	}
}

bool UMenu::Initialize()
{
	if (!Super::Initialize()) return false;
	if (HostButton) {
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}
	if (JoinButton) {
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}
	return false;
}

void UMenu::NativeDestruct()
{
	MenuTearDown();
	// 5.1����UUserWidget�಻�ڰ����麯��OnLevelRemovedFromWorld���������NativeDestruct
	Super::NativeDestruct();
		//InWorld, bIsSimulating
	
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, TEXT("Session Created Successfully"));
		}
		UWorld* World = GetWorld();
		if (World) {
			World->ServerTravel(FString("/Game/ThirdPerson/Maps/Lobby?listen?SeamlessTravel=0"));
		}
	}
	else {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, TEXT("Failed to create session"));
		}
	}
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
}

void UMenu::HostButtonClicked()
{
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, TEXT("Host Button Clicked"));
	}

	if (MultiplayerSessionsSubsystem) {
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
	}
	else {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, TEXT("MultiplayerSessionsSubsystem is null"));
		}
	}
}

void UMenu::JoinButtonClicked()
{
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Orange, TEXT("Join Button Clicked"));
	}
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (World) {
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController) {
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->bShowMouseCursor = false;

		}
	}
}
