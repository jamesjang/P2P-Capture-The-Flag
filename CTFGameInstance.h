// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "OnlineIdentityInterface.h"
#include "OnlineSessionInterface.h"
#include "Engine/GameInstance.h"
#include "CTFGameInstance.generated.h"

class ACTFSession;


UCLASS()
class CAPTURETHEFLAG_API UCTFGameInstance : public UGameInstance
{
	GENERATED_UCLASS_BODY()

public:
	ACTFSession* GetGameSession() const;

	virtual void Init() override;
// 	virtual void Shutdown() override;
// 	virtual void StartGameInstance() override;

// 
// 	bool HostGame(ULocalPlayer* LocalPlayer, const FString& GameType, const FString& InTravelURL);
// 	bool JoinSession(ULocalPlayer* LocalPlayer, int32 SessionIndexInSearchResults);
// 	bool JoinSession(ULocalPlayer* LocalPlayer, const FOnlineSessionSearchResult& SearchResult);

	/** Last connection status that was passed into the HandleNetworkConnectionStatusChanged hander */
	EOnlineServerConnectionStatus::Type	CurrentConnectionStatus;






};
