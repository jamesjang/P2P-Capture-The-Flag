// Fill out your copyright notice in the Description page of Project Settings.

#include "CaptureTheFlag.h"
#include "CaptureTheFlagCharacter.h"
#include "CaptureTheFlagController.h"
#include "CaptureTheFlagGameMode.h"
#include "CaptureTheFlagPlayerState.h"
#include "GameStateClass.h"
#include "Engine.h"
#include "Net/UnrealNetwork.h"
#include "CaptureTheFlagWidget.h"


float UCaptureTheFlagWidget::GetPlayerHealth() const
{
	ACaptureTheFlagCharacter* Character = Cast<ACaptureTheFlagCharacter>(GetOwningPlayer()->GetPawn());
	if (Character)
	{
		return Character->m_Health/100;
	}

	ACaptureTheFlagCharacter* LocalCharacter = Cast<ACaptureTheFlagCharacter>(GetWorld()->GetFirstPlayerController());
	if (LocalCharacter)
	{
		return Character->m_Health/100;
	}

	return 100;


}

FText UCaptureTheFlagWidget::GetCurrentAmmo() const
{
	ACaptureTheFlagCharacter* Character = Cast<ACaptureTheFlagCharacter>(GetOwningPlayer()->GetPawn());
	if (Character)
	{
		return FText::AsNumber(Character->CurrentAmmo);
	}

	ACaptureTheFlagCharacter* LocalCharacter = Cast<ACaptureTheFlagCharacter>(GetWorld()->GetFirstPlayerController());
	if (LocalCharacter)
	{
		return FText::AsNumber(Character->CurrentAmmo);
	}

	return FText::AsNumber(10);
}

FText UCaptureTheFlagWidget::GetMaxAmmo() const
{
	ACaptureTheFlagCharacter* Character = Cast<ACaptureTheFlagCharacter>(GetOwningPlayer()->GetPawn());
	if (Character)
	{
		return FText::AsNumber(Character->MaxAmmo);
	}

	ACaptureTheFlagCharacter* LocalCharacter = Cast<ACaptureTheFlagCharacter>(GetWorld()->GetFirstPlayerController());
	if (LocalCharacter)
	{
		return FText::AsNumber(Character->MaxAmmo);
	}
	return FText::AsNumber(10);
}

FText UCaptureTheFlagWidget::GetKills() const
{
	ACaptureTheFlagCharacter* Character = Cast<ACaptureTheFlagCharacter>(GetOwningPlayer()->GetPawn());
	if (Character)
	{
		return FText::AsNumber(Character->m_Kills);
	}

	ACaptureTheFlagCharacter* LocalCharacter = Cast<ACaptureTheFlagCharacter>(GetWorld()->GetFirstPlayerController());
	if (LocalCharacter)
	{
		return FText::AsNumber(Character->m_Kills);
	}
	return FText::AsNumber(0);
}

bool UCaptureTheFlagWidget::GetVisibility() const
{
	ACaptureTheFlagCharacter* Character = Cast<ACaptureTheFlagCharacter>(GetOwningPlayer()->GetPawn());
	if (Character)
	{
		return Character->bHasFlag;
	}

	ACaptureTheFlagCharacter* LocalCharacter = Cast<ACaptureTheFlagCharacter>(GetWorld()->GetFirstPlayerController());
	if (LocalCharacter)
	{
		return Character->bHasFlag;
	}

	return false;
}

FText UCaptureTheFlagWidget::GetDeaths() const
{
	ACaptureTheFlagCharacter* Character = Cast<ACaptureTheFlagCharacter>(GetOwningPlayer()->GetPawn());
	if (Character)
	{
		return FText::AsNumber(Character->m_Deaths);
	}

	ACaptureTheFlagCharacter* LocalCharacter = Cast<ACaptureTheFlagCharacter>(GetWorld()->GetFirstPlayerController());
	if (LocalCharacter)
	{
		return FText::AsNumber(Character->m_Deaths);
	}
	return FText::AsNumber(0);
}

FText UCaptureTheFlagWidget::GetTeamRedScore() const
{
	AGameStateClass* const MyGameState = Cast<AGameStateClass>(GetWorld()->GameState);

	ACaptureTheFlagPlayerState* PS = Cast<ACaptureTheFlagPlayerState>(GetWorld()->GetFirstPlayerController()->PlayerState);
	if (PS)
	{
		if (MyGameState->TeamScores.Num() > 0)
		{
			return FText::AsNumber(MyGameState->TeamScores[1]);
		}
	}

	return FText::AsNumber(0);
}


FText UCaptureTheFlagWidget::GetTeamBlueScore() const
{
	AGameStateClass* const MyGameState = Cast<AGameStateClass>(GetWorld()->GameState);
	ACaptureTheFlagPlayerState* PS = Cast<ACaptureTheFlagPlayerState>(GetWorld()->GetFirstPlayerController()->PlayerState);

	if (MyGameState->TeamScores.Num() > 0)
	{
		return FText::AsNumber(MyGameState->TeamScores[0]);
	}

	return FText::AsNumber(0);
}

FText UCaptureTheFlagWidget::GetRoundTime() const
{
	AGameStateClass* const MyGameState = Cast<AGameStateClass>(GetWorld()->GameState);
	ACaptureTheFlagPlayerState* PS = Cast<ACaptureTheFlagPlayerState>(GetWorld()->GetFirstPlayerController()->PlayerState);
	ACaptureTheFlagGameMode* GM = Cast<ACaptureTheFlagGameMode>(GetWorld()->GetAuthGameMode());

	//AShooterGameState* const GameState = Cast<AShooterGameState>(PCOwner->GetWorld()->GameState);
	if (MyGameState )
	{
// 		if (GM->GetMatchState() == MatchState::InProgress)
// 		{
			return FText::AsNumber(MyGameState->RemainingTime);
//		}
	}

	return FText::AsNumber(0);
}

FText UCaptureTheFlagWidget::GetRedFlagStatus() const
{
	AGameStateClass* GS = Cast<AGameStateClass>(GetWorld()->GameState);
	if (GS)
	{
		if (GS->bRedTeamFlag == true)
		{
			FString Status = FString(TEXT("Red Team has the flag"));
			return FText::FromString(Status);

		}
	}
	FString Status = FString(TEXT(""));
	return FText::FromString(Status);
}

FText UCaptureTheFlagWidget::GetBlueFlagStatus() const
{
	AGameStateClass* GS = Cast<AGameStateClass>(GetWorld()->GameState);
	if (GS)
	{
		if (GS->bBlueTeamFlag == true)
		{
			FString Status = FString(TEXT("Blue Team has the flag"));
			return FText::FromString(Status);

		}
	}
	FString Status = FString(TEXT(""));
	return FText::FromString(Status);
}



