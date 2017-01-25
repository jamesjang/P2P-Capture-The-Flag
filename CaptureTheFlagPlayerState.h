// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "CaptureTheFlagPlayerState.generated.h"

UCLASS()
class CAPTURETHEFLAG_API ACaptureTheFlagPlayerState : public APlayerState
{
	GENERATED_UCLASS_BODY()

	/** replicate team colors. Updated the players mesh colors appropriately */
	UFUNCTION()
	void OnRep_TeamColor();

	/*Clears score*/
	virtual void Reset() override;

	/*sets the team, Incontroller = the controller to initialize the state with*/
	virtual void ClientInitialize(class AController* InController) override;

	virtual void UnregisterPlayerWithSession() override;

	void SetTeamNum(int32 NewTeamNumber);

	int32 GetTeamNum() const;

	int32 GetKills() const;

	int32 GetDeaths() const;


	float GetScore() const;

	/** get whether the player quit the match */
	bool IsQuitter() const;

	/** gets truncated player name to fit in death log and scoreboards */
	FString GetShortPlayerName() const;

	/** Sends kill (excluding self) to clients */
	UFUNCTION(Reliable, Client)
	void InformAboutKill(class ACaptureTheFlagPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class ACaptureTheFlagPlayerState* KilledPlayerState);

	/** broadcast death to local clients */
	UFUNCTION(Reliable, NetMulticast)
	void BroadcastDeath(class ACaptureTheFlagPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class ACaptureTheFlagPlayerState* KilledPlayerState);

	/** Set whether the player is a quitter */
	void SetQuitter(bool bInQuitter);

	virtual void CopyProperties(class APlayerState* PlayerState) override;

protected:


	/** whether the user quit the match */
	UPROPERTY()
	uint8 bQuitter : 1;



public:
	/** helper for scoring points */
	void ScorePoints(int32 Points);

	UPROPERTY(Replicated)
	bool bTeamB;

	/** player killed someone */
	void ScoreKill(ACaptureTheFlagPlayerState* Victim, int32 Points);

	/** player died */
	void ScoreDeath(ACaptureTheFlagPlayerState* KilledBy, int32 Points);

/*	void ScoreFlag(int32 Points);*/

	/** team number */
	UPROPERTY(VisibleAnywhere, Transient, ReplicatedUsing = OnRep_TeamColor)
	int32 TeamNumber;

	/** number of kills */
	UPROPERTY(Transient, Replicated)
		int32 NumKills;

	/** number of deaths */
	UPROPERTY(Transient, Replicated)
		int32 NumDeaths;

	
	
	
	
};
