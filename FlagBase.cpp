// Fill out your copyright notice in the Description page of Project Settings.

#include "CaptureTheFlag.h"
#include "CaptureTheFlagCharacter.h"
#include "CaptureTheFlagPlayerState.h"
#include "FlagBase.h"


// Sets default values
AFlagBase::AFlagBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}
void AFlagBase::NotifyActorBeginOverlap(class AActor* Other)
{
	Super::NotifyActorBeginOverlap(Other);

	//PickupOnTouch(Cast<ACaptureTheFlagCharacter>(Other));

}

void AFlagBase::PickupOnTouch(class ACaptureTheFlagCharacter* Pawn)
{


}

