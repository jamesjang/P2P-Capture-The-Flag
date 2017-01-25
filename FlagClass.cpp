// Fill out your copyright notice in the Description page of Project Settings.

#include "CaptureTheFlag.h"
#include "FlagClass.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine.h"
#include "CaptureTheFlagGameMode.h"
#include "CaptureTheFlagCharacter.h"
#include "CaptureTheFlagController.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"

AFlagClass::AFlagClass(const FObjectInitializer& ObjectInitializer)
	:Super (ObjectInitializer)
{
	//Makes sure our object is always replicated
	bReplicates = true;

	CollisionComp = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("Collision Comp"));
	CollisionComp->InitSphereRadius(60.f);
	CollisionComp->AttachTo(RootComponent);

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AFlagClass::OnActorBeginOverlap);
	CollisionComp->OnComponentEndOverlap.AddDynamic(this, &AFlagClass::OnActorEndOverlap);

}

void AFlagClass::BeginPlay()
{
	Super::BeginPlay();
	if (TooltipClass)
	{
	//	ACaptureTheFlagController* MyController = Cast<ACaptureTheFlagController>(GetOwner());
		ToolWidget = CreateWidget<UTooltipWidgetClass>(GetWorld()->GetGameInstance(), TooltipClass);
		ToolWidget->AddToViewport();
		ToolWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AFlagClass::OnActorBeginOverlap(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACaptureTheFlagCharacter* character = Cast<ACaptureTheFlagCharacter>(OtherActor);
	if (OtherActor == character)
	{
		if (ToolWidget)
		{
			ToolWidget->SetVisibility(ESlateVisibility::Visible);
		}
	//	SetRotationa(OtherComp);
		isPlayer = true;
	}
}
void AFlagClass::OnActorEndOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		if (ToolWidget)
		{
			ToolWidget->SetVisibility(ESlateVisibility::Hidden);
		}
		isPlayer = false;
	}

}



