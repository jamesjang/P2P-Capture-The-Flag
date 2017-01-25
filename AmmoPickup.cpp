// Fill out your copyright notice in the Description page of Project Settings.

#include "CaptureTheFlag.h"
#include "CaptureTheFlagCharacter.h"
#include "CaptureTheFlagGameMode.h"
#include "Engine.h"
#include "Net/UnrealNetwork.h"
#include "CaptureTheFlagController.h"
#include "AmmoPickup.h"


// Sets default values
AAmmoPickup::AAmmoPickup(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CollisionComp = ObjectInitializer.CreateDefaultSubobject<UCapsuleComponent>(this, TEXT("CollisionComp"));
	CollisionComp->InitCapsuleSize(40.f, 50.f);
	CollisionComp->AttachTo(RootComponent);


	bReplicates = true;
}


void AAmmoPickup::NotifyActorBeginOverlap(class AActor* Other)
{
	Super::NotifyActorBeginOverlap(Other);

	PickupOnTouch(Cast<ACaptureTheFlagCharacter>(Other));

}

void AAmmoPickup::PickupOnTouch(class ACaptureTheFlagCharacter* Pawn)
{
	if (Pawn)
	{
		ACaptureTheFlagController* MyController = Cast<ACaptureTheFlagController>(Pawn->Controller);
		if (MyController)
		{

			Pawn->CurrentAmmo += 10.f;


			Destroy();

			if (Role < ROLE_Authority)
			{
				ServerDestroy();
			}
		}
	}

}

void AAmmoPickup::ServerDestroy_Implementation()
{
	Destroy();
}

bool AAmmoPickup::ServerDestroy_Validate()
{
	return true;
}