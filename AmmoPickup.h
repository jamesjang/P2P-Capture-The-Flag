// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "AmmoPickup.generated.h"

UCLASS()
class CAPTURETHEFLAG_API AAmmoPickup : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAmmoPickup();

	virtual void NotifyActorBeginOverlap(class AActor* other) override;

	virtual void PickupOnTouch(class ACaptureTheFlagCharacter* Pawn);

	UCapsuleComponent* CollisionComp;

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerDestroy();

	
	
};
