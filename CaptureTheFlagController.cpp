// Fill out your copyright notice in the Description page of Project Settings.

#include "CaptureTheFlag.h"
#include "CaptureTheFlagCharacter.h"
#include "CaptureTheFlagGameMode.h"
#include "CaptureTheFlagController.h"
#include "FlagWidgetClass.h"
#include "FlagClass.h"
#include "MyHUD.h"
#include "Engine.h"
#include "Net/UnrealNetwork.h"

ACaptureTheFlagController::ACaptureTheFlagController()
{
	bRespawnImmediately = true;
}

void ACaptureTheFlagController::UnFreeze()
{
	ServerRestartPlayer();
//	bisPlayerInGame = true;
}

void ACaptureTheFlagController::Respawn()
{
	//bisPlayerInGame = true;
}



void ACaptureTheFlagController::BeginPlay()
{
	Super::BeginPlay();
	FInputModeGameOnly GameMode;
	SetInputMode(GameMode);


}

void ACaptureTheFlagController::SetPlayer(UPlayer* InPlayer)
{
	Super::SetPlayer(InPlayer);
}

/** Starts the online game using the session name in the PlayerState */
void ACaptureTheFlagController::ClientStartOnlineGame_Implementation()
{
	ACaptureTheFlagPlayerState* PS = Cast<ACaptureTheFlagPlayerState>(PlayerState);
	if (PS)
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub)
		{
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid())
			{
				UE_LOG(LogOnline, Log, TEXT("Starting session %s on client"), *PS->SessionName.ToString());
				Sessions->StartSession(PS->SessionName);
			}
		}
	}
	else
	{
		// Keep retrying until player state is replicated
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ClientStartOnlineGame, this, &ACaptureTheFlagController::ClientStartOnlineGame_Implementation, 0.2f, false);
	}
}

/** Ends the online game using the session name in the PlayerState */
void ACaptureTheFlagController::ClientEndOnlineGame_Implementation()
{
	ACaptureTheFlagPlayerState* PS = Cast<ACaptureTheFlagPlayerState>(PlayerState);
	if (PS)
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub)
		{
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid())
			{
				UE_LOG(LogOnline, Log, TEXT("Ending session %s on client"), *PS->SessionName.ToString());
				Sessions->EndSession(PS->SessionName);
			}
		}
	}
}

void ACaptureTheFlagController::ClientGameEnded_Implementation(class AActor* EndGameFocus, bool bIsWinner)
{
	Super::ClientGameEnded_Implementation(EndGameFocus, bIsWinner);

	// Disable controls now the game has ended
	SetIgnoreMoveInput(true);

}

void ACaptureTheFlagController::Suicide()
{
	ServerSuicide();
}

bool ACaptureTheFlagController::ServerSuicide_Validate()
{
	return true;
}

void ACaptureTheFlagController::ServerSuicide_Implementation()
{
	if ((GetPawn() != NULL) && ((GetWorld()->TimeSeconds - GetPawn()->CreationTime > 1)))
	{
		ACaptureTheFlagCharacter* MyPawn = Cast<ACaptureTheFlagCharacter>(GetPawn());
		if (MyPawn)
		{
			MyPawn->Suicide();
		}
	}
}

AMyHUD* ACaptureTheFlagController::GetHud() const
{
	return Cast<AMyHUD>(GetHUD());
}

void ACaptureTheFlagController::ToggleMyUIOn()
{
	GetHud()->ToggleFlagWidgetOn();
}
void ACaptureTheFlagController::ToggleMyUIOff()
{
	GetHud()->ToggleFlagWidgetOff();
}

void ACaptureTheFlagController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Toggle", IE_Pressed, this, &ACaptureTheFlagController::ToggleServerOn);
}

void ACaptureTheFlagController::ToggleServerOn()
{
	GetHud()->ToggleServerWidgetOn();
}

