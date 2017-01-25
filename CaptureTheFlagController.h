#include "GameFramework/PlayerController.h"
#include "CaptureTheFlagCharacter.h"
#include "CaptureTheFlagWidget.h"
#include "FlagWidgetClass.h"
#include "CaptureTheFlagController.generated.h"


class AMyHUD;
UCLASS()
class CAPTURETHEFLAG_API ACaptureTheFlagController : public APlayerController
{
	GENERATED_BODY()

	ACaptureTheFlagController();


	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bisPlayerInGame;
	
	void Respawn();

	void ToggleServerOn();


	/* Flag to respawn or start spectating upon death */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	bool bRespawnImmediately;

	/* Respawn or start spectating after dying */
	virtual void UnFreeze() override;


	/** Starts the online game using the session name in the PlayerState */
	UFUNCTION(reliable, client)
	void ClientStartOnlineGame();

	/** Ends the online game using the session name in the PlayerState */
	UFUNCTION(reliable, client)
	void ClientEndOnlineGame();

	/** notify player about finished match */
	virtual void ClientGameEnded_Implementation(class AActor* EndGameFocus, bool bIsWinner);

	/** Returns a pointer to the shooter game hud. May return NULL. */
	AMyHUD* GetHud() const;

	/** Associate a new UPlayer with this PlayerController. */
	virtual void SetPlayer(UPlayer* Player);

	/** Causes the player to commit suicide */
	UFUNCTION(exec)
	virtual void Suicide();

	/** Notifies the server that the client has suicided */
	UFUNCTION(reliable, server, WithValidation)
	void ServerSuicide();

	/** Handle for efficient management of ClientStartOnlineGame timer */
	FTimerHandle TimerHandle_ClientStartOnlineGame;

	/** after all game elements are created */
//	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	//virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;

	/** sets up input */
	virtual void SetupInputComponent() override;

	void ToggleMyUIOn();

	void ToggleMyUIOff();






};
