// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "CaptureTheFlag.h"
#include "CaptureTheFlagGameMode.h"
#include "MyHUD.h"
#include "CaptureTheFlagController.h"
#include "GameStateClass.h"
#include "CaptureTheFlagCharacter.h"
#include "CaptureTheFlagPlayerState.h"
#include "Engine.h"
#include "MyPlayerStart.h"

ACaptureTheFlagGameMode::ACaptureTheFlagGameMode()
{
	//sets our default character to our blueprint class
	static ConstructorHelpers::FObjectFinder<UClass> bpClassFinder(TEXT("Class'/Game/Blueprints/ThirdPersonCharacter.ThirdPersonCharacter_C'"));
	if (bpClassFinder.Object)
	{
		UClass* bpClass = bpClassFinder.Object;

	}

	//PlayerControllerClass = ACaptureTheFlagController::StaticClass();
	//assigns hud 
	HUDClass = AMyHUD::StaticClass();
	PlayerControllerClass = ACaptureTheFlagController::StaticClass();
	GameStateClass = AGameStateClass::StaticClass();
	PlayerStateClass = ACaptureTheFlagPlayerState::StaticClass();

	NumTeams = 2;
	KillScore = 1;

}
void ACaptureTheFlagGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	GetWorldTimerManager().SetTimer(TimerHandle_DefaultTimer, this, &ACaptureTheFlagGameMode::DefaultTimer, GetWorldSettings()->GetEffectiveTimeDilation(), true);
}


bool ACaptureTheFlagGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	return false;
}



void ACaptureTheFlagGameMode::AddScore(int32 amount)
{
	GetGameState<AGameStateClass>()->Score += amount;

}

void ACaptureTheFlagGameMode::PostLogin(APlayerController* NewPlayer)
{
	// Place player on a team before Super (VoIP team based init, findplayerstart, etc)
	ACaptureTheFlagPlayerState* NewPlayerState = CastChecked<ACaptureTheFlagPlayerState>(NewPlayer->PlayerState);
	const int32 TeamNum = ChooseTeam(NewPlayerState);
	NewPlayerState->SetTeamNum(TeamNum);

	Super::PostLogin(NewPlayer);
	
}

int32 ACaptureTheFlagGameMode::ChooseTeam(ACaptureTheFlagPlayerState* ForPlayerState) const
{
	TArray<int32> TeamBalance;
	TeamBalance.AddZeroed(NumTeams);

	// get current team balance
	for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		ACaptureTheFlagPlayerState const* const TestPlayerState = Cast<ACaptureTheFlagPlayerState>(GameState->PlayerArray[i]);
		if (TestPlayerState && TestPlayerState != ForPlayerState && TeamBalance.IsValidIndex(TestPlayerState->GetTeamNum()))
		{
			TeamBalance[TestPlayerState->GetTeamNum()]++;
		}
	}

	// find least populated one
	int32 BestTeamScore = TeamBalance[0];
	for (int32 i = 1; i < TeamBalance.Num(); i++)
	{
		if (BestTeamScore > TeamBalance[i])
		{
			BestTeamScore = TeamBalance[i];
		}
	}

	// there could be more than one...
	TArray<int32> BestTeams;
	for (int32 i = 0; i < TeamBalance.Num(); i++)
	{
		if (TeamBalance[i] == BestTeamScore)
		{
			BestTeams.Add(i);
		}
	}

	// get random from best list
	const int32 RandomBestTeam = BestTeams[FMath::RandHelper(BestTeams.Num())];
	return RandomBestTeam;
}
void ACaptureTheFlagGameMode::InitGameState()
{
	Super::InitGameState();

	AGameStateClass* const MyGameState = Cast<AGameStateClass>(GameState);
	if (MyGameState)
	{
		MyGameState->NumTeams = NumTeams;
	}
}


AActor* ACaptureTheFlagGameMode::ChoosePlayerStart_Implementation(AController* Player)
{

	TArray<APlayerStart*> PreferredSpawns;
	TArray<APlayerStart*> FallbackSpawns;

	APlayerStart* BestStart = NULL;
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
 		APlayerStart* TestSpawn = *It;
			if (IsSpawnpointAllowed(TestSpawn, Player))
			{
				if (IsSpawnpointPreferred(TestSpawn, Player))
				{
					PreferredSpawns.Add(TestSpawn);
				}
				else
				{
					FallbackSpawns.Add(TestSpawn);
				}
			}
		}

	if (PreferredSpawns.Num() > 0)
	{
		BestStart = PreferredSpawns[FMath::RandHelper(PreferredSpawns.Num())];
	}
	else if (FallbackSpawns.Num() > 0)
	{
		BestStart = FallbackSpawns[FMath::RandHelper(FallbackSpawns.Num())];
	}

	return BestStart ? BestStart : Super::ChoosePlayerStart_Implementation(Player);
}

bool ACaptureTheFlagGameMode::IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const
{
	if (Player)
	{
		AMyPlayerStart* TeamStart = Cast<AMyPlayerStart>(SpawnPoint);
		ACaptureTheFlagPlayerState* PlayerState = Cast<ACaptureTheFlagPlayerState>(Player->PlayerState);

		if (PlayerState && TeamStart && TeamStart->SpawnTeam != PlayerState->GetTeamNum())
		{
			return false;

		}
	}
	return true;
}

bool ACaptureTheFlagGameMode::IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Player) const
{
	ACharacter* MyPawn = Player ? Cast<ACharacter>(Player->GetPawn()) : NULL;
	if (MyPawn)
	{
		const FVector SpawnLocation = SpawnPoint->GetActorLocation();
		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
		{
			ACharacter* OtherPawn = Cast<ACharacter>(*It);
			if (OtherPawn && OtherPawn != MyPawn)
			{
				const float CombinedHeight = (MyPawn->CapsuleComponent->GetScaledCapsuleHalfHeight() + OtherPawn->CapsuleComponent->GetScaledCapsuleHalfHeight()) * 2.0f;
				const float CombinedRadius = MyPawn->CapsuleComponent->GetScaledCapsuleRadius() + OtherPawn->CapsuleComponent->GetScaledCapsuleRadius();
				const FVector OtherLocation = OtherPawn->GetActorLocation();

				// check if player start overlaps this pawn
				if (FMath::Abs(SpawnLocation.Z - OtherLocation.Z) < CombinedHeight && (SpawnLocation - OtherLocation).Size2D() < CombinedRadius)
				{
					return false;
				}
			}
		}
	}

	return true;
}

void ACaptureTheFlagGameMode::Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType)
{
	ACaptureTheFlagPlayerState* KillerPlayerState = Killer ? Cast<ACaptureTheFlagPlayerState>(Killer->PlayerState) : NULL;
	ACaptureTheFlagPlayerState* VictimPlayerState = KilledPlayer ? Cast<ACaptureTheFlagPlayerState>(KilledPlayer->PlayerState) : NULL;

	if (KillerPlayerState && KillerPlayerState != VictimPlayerState)
	{
		KillerPlayerState->ScoreKill(VictimPlayerState, KillScore);
	}

	if (VictimPlayerState)
	{
		VictimPlayerState->ScoreDeath(KillerPlayerState, DeathScore);
	}
}

float ACaptureTheFlagGameMode::ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
	float ActualDamage = Damage;

	ACaptureTheFlagCharacter* DamagedPawn = Cast<ACaptureTheFlagCharacter>(DamagedActor);
	if (DamagedPawn && EventInstigator)
	{
		ACaptureTheFlagPlayerState* DamagedPlayerState = Cast<ACaptureTheFlagPlayerState>(DamagedPawn->PlayerState);
		ACaptureTheFlagPlayerState* InstigatorPlayerState = Cast<ACaptureTheFlagPlayerState>(EventInstigator->PlayerState);

		// disable friendly fire
		if (!CanDealDamage(InstigatorPlayerState, DamagedPlayerState))
		{
			ActualDamage = 0.0f;
		}

		// scale self instigated damage
		if (InstigatorPlayerState == DamagedPlayerState)
		{
			ActualDamage *= 0;
		}
	}

	return ActualDamage;
}

bool ACaptureTheFlagGameMode::CanDealDamage(class ACaptureTheFlagPlayerState* DamageInstigator, class ACaptureTheFlagPlayerState* DamagedPlayer) const
{
	return DamageInstigator && DamagedPlayer && (DamagedPlayer == DamageInstigator || DamagedPlayer->GetTeamNum() != DamageInstigator->GetTeamNum());
}

void ACaptureTheFlagGameMode::DefaultTimer()
{
	// don't update timers for Play In Editor mode, it's not real match
	if (GetWorld()->IsPlayInEditor())
	{
		// start match if necessary.
		if (GetMatchState() == MatchState::WaitingToStart)
		{
			StartMatch();
		}
		return;
	}

	AGameStateClass* const MyGameState = Cast<AGameStateClass>(GameState);
	if (MyGameState && MyGameState->RemainingTime > 0)
	{
		MyGameState->RemainingTime--;

		if (MyGameState->RemainingTime <= 0)
		{
			MyGameState->RemainingTime = 0;
		}
	}
}

void ACaptureTheFlagGameMode::HandleMatchIsWaitingToStart()
{
		// start warmup if needed
		AGameStateClass* const MyGameState = Cast<AGameStateClass>(GameState);
		if (MyGameState && MyGameState->RemainingTime == 0)
		{
			const bool bWantsMatchWarmup = !GetWorld()->IsPlayInEditor();
			if (bWantsMatchWarmup)
			{
				MyGameState->RemainingTime = 50.f;
			}
			else
			{
				MyGameState->RemainingTime = 0.0f;
			}
		}
}

void ACaptureTheFlagGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	AGameStateClass* const MyGameState = Cast<AGameStateClass>(GameState);
	MyGameState->RemainingTime = 300.f;


}
