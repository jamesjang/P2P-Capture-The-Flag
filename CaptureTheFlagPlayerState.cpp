// Fill out your copyright notice in the Description page of Project Settings.

#include "CaptureTheFlag.h"
#include "CaptureTheFlagPlayerState.h"
#include "Engine.h"
#include "CaptureTheFlagController.h"
#include "CaptureTheFlagCharacter.h"
#include "GameStateClass.h"
#include "Net/UnrealNetwork.h"

ACaptureTheFlagPlayerState::ACaptureTheFlagPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	TeamNumber = 0;
	NumKills = 0;
	NumDeaths = 0;
	bQuitter = false;
};

void ACaptureTheFlagPlayerState::Reset()
{
	Super::Reset();

	NumKills = 0;
	NumDeaths = 0;
	bQuitter = false;
}

void ACaptureTheFlagPlayerState::UnregisterPlayerWithSession()
{
	Super::UnregisterPlayerWithSession();
}

void ACaptureTheFlagPlayerState::ClientInitialize(AController* InController)
{
	Super::ClientInitialize(InController);

//	UpdateTeamColors();

}

void ACaptureTheFlagPlayerState::SetTeamNum(int32 NewTeamNumber)
{
	TeamNumber = NewTeamNumber;

	//UpdateTeamColors();
}

void ACaptureTheFlagPlayerState::OnRep_TeamColor()
{
//	UpdateTeamColors();
}


void ACaptureTheFlagPlayerState::SetQuitter(bool bInQuitter)
{
	bQuitter = bInQuitter;
}

void ACaptureTheFlagPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	ACaptureTheFlagPlayerState* ShooterPlayer = Cast<ACaptureTheFlagPlayerState>(PlayerState);
	if (ShooterPlayer)
	{
		ShooterPlayer->TeamNumber = TeamNumber;
	}
}

int32 ACaptureTheFlagPlayerState::GetTeamNum() const
{
	return TeamNumber;
}

int32 ACaptureTheFlagPlayerState::GetKills() const
{
	return NumKills;
}

int32 ACaptureTheFlagPlayerState::GetDeaths() const
{
	return NumDeaths;
}

float ACaptureTheFlagPlayerState::GetScore() const
{
	return Score;
}

bool ACaptureTheFlagPlayerState::IsQuitter() const
{
	return bQuitter;
}


void ACaptureTheFlagPlayerState::InformAboutKill_Implementation(class ACaptureTheFlagPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class ACaptureTheFlagPlayerState* KilledPlayerState)
{
	//id can be null for bots
	if (KillerPlayerState->UniqueId.IsValid())
	{
		//search for the actual killer before calling OnKill()	
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			ACaptureTheFlagController* TestPC = Cast<ACaptureTheFlagController>(*It);
			if (TestPC && TestPC->IsLocalController())
			{
				// a local player might not have an ID if it was created with CreateDebugPlayer.
				ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(TestPC->Player);
				TSharedPtr<const FUniqueNetId> LocalID = LocalPlayer->GetCachedUniqueNetId();
				if (LocalID.IsValid() && *LocalPlayer->GetCachedUniqueNetId() == *KillerPlayerState->UniqueId)
				{
					//TestPC->OnKill();
				}
			}
		}
	}
}

void ACaptureTheFlagPlayerState::BroadcastDeath_Implementation(class ACaptureTheFlagPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class ACaptureTheFlagPlayerState* KilledPlayerState)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		// all local players get death messages so they can update their huds.
		ACaptureTheFlagController* TestPC = Cast<ACaptureTheFlagController>(*It);
		if (TestPC && TestPC->IsLocalController())
		{
		//	TestPC->OnDeathMessage(KillerPlayerState, this, KillerDamageType);
		}
	}
}

void ACaptureTheFlagPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACaptureTheFlagPlayerState, TeamNumber);
	DOREPLIFETIME(ACaptureTheFlagPlayerState, NumKills);
	DOREPLIFETIME(ACaptureTheFlagPlayerState, NumDeaths);
	DOREPLIFETIME(ACaptureTheFlagPlayerState, bTeamB);
}

FString ACaptureTheFlagPlayerState::GetShortPlayerName() const
{
	if (PlayerName.Len() > 10)
	{
		return PlayerName.Left(10) + "...";
	}
	return PlayerName;
}


void ACaptureTheFlagPlayerState::ScoreKill(ACaptureTheFlagPlayerState* Victim, int32 Points)
{
	NumKills++;
	ScorePoints(Points);
}

void ACaptureTheFlagPlayerState::ScoreDeath(ACaptureTheFlagPlayerState* KilledBy, int32 Points)
{
	NumDeaths++;
	ScorePoints(Points);
}


void ACaptureTheFlagPlayerState::ScorePoints(int32 Points)
{
	AGameStateClass* const MyGameState = Cast<AGameStateClass>(GetWorld()->GameState);
	if (MyGameState && TeamNumber >= 0)
	{
		if (TeamNumber >= MyGameState->TeamScores.Num())
		{
			MyGameState->TeamScores.AddZeroed(TeamNumber - MyGameState->TeamScores.Num() + 1);
		}

		MyGameState->TeamScores[TeamNumber] += Points;
	}

	Score += Points;
}







