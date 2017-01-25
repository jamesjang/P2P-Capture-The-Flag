// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "CaptureTheFlagPlayerState.h"
#include "MyPlayerStart.h"
#include "CaptureTheFlagGameMode.generated.h"

UCLASS(minimalapi)
class ACaptureTheFlagGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ACaptureTheFlagGameMode();

	FTimerHandle TimerHandle_DefaultTimer;

	void AddScore(int32 amount);

	void PostLogin(APlayerController* NewPlayer) override;

	virtual void PreInitializeComponents() override;

	/** prevents friendly fire */
	virtual float ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const;

	/** can players damage each other? */
	virtual bool CanDealDamage(ACaptureTheFlagPlayerState* DamageInstigator, ACaptureTheFlagPlayerState* DamagedPlayer) const;

	/** initialize replicated game data */
	virtual void InitGameState() override;

	//AActor* ChoosePlayerStart(AController* Player) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	/** number of teams */
	int32 NumTeams;

	/** best team */
	int32 WinnerTeam;

	/** pick team with least players in or random when it's equal */
	int32 ChooseTeam(ACaptureTheFlagPlayerState* ForPlayerState) const;

	/** check team constraints */
	virtual bool IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Player) const;

	virtual bool IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const;

	/** always pick new random spawn */
	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;

	/** notify about kills */
	virtual void Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType);

	/** score for kill */
	UPROPERTY(config)
	int32 KillScore;

	/** score for death */
	UPROPERTY(config)
	int32 DeathScore;

	void DefaultTimer();

	/** called before startmatch */
	virtual void HandleMatchIsWaitingToStart() override;

	/** starts new match */
	virtual void HandleMatchHasStarted() override;

};



