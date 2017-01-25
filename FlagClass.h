// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "FlagWidgetclass.h"
#include "TooltipWidgetClass.h"
#include "FlagClass.generated.h"

UCLASS()
class CAPTURETHEFLAG_API AFlagClass : public AActor
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	
public:	
	// Sets default values for this actor's properties
	AFlagClass(const FObjectInitializer& ObjectInitializer);


	USphereComponent* CollisionComp;

	UPROPERTY(EditInstanceOnly)
	int32 TeamNum;

	UFUNCTION()
	void OnActorBeginOverlap (class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION()
	void OnActorEndOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool isPlayer;


	UPROPERTY(EditAnywhere)
	TSubclassOf<UTooltipWidgetClass> TooltipClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTooltipWidgetClass* ToolWidget;



	
	
};
