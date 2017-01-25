// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CaptureTheFlag.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "CaptureTheFlagWidget.generated.h"

/**
 * 
 */
UCLASS()
class CAPTURETHEFLAG_API UCaptureTheFlagWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = Information)
	float GetPlayerHealth() const;


	UFUNCTION(BlueprintCallable, Category = Information)
	FText GetCurrentAmmo() const;

	UFUNCTION(BlueprintCallable, Category = Information)
	FText GetMaxAmmo() const;

	UFUNCTION(BlueprintCallable, Category = Information)
	FText GetKills() const;

	UFUNCTION(BlueprintCallable, Category = Information)
	FText GetDeaths() const;

	UFUNCTION(BlueprintCallable, Category = Information)
	bool GetVisibility() const;

	UFUNCTION(BlueprintCallable, Category = Information)
	FText GetTeamRedScore() const;

	UFUNCTION(BlueprintCallable, Category = Information)
	FText GetRoundTime() const;

	UFUNCTION(BlueprintCallable, Category = Information)
	FText GetTeamBlueScore() const;

	UFUNCTION(BlueprintCallable, Category = Information)
	FText GetRedFlagStatus() const;

	UFUNCTION(BlueprintCallable, Category = Information)
	FText GetBlueFlagStatus() const;








	
	
	
};
