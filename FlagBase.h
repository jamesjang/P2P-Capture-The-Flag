// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "FlagBase.generated.h"

UCLASS()
class CAPTURETHEFLAG_API AFlagBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFlagBase();

	UPROPERTY(EditInstanceOnly)
	int32 TeamNum;

	virtual void NotifyActorBeginOverlap(class AActor* other) override;
	virtual void PickupOnTouch(class ACaptureTheFlagCharacter* Pawn);

	
	
};
