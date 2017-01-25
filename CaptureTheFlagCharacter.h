// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/Engine/Canvas.h"
#include "FlagWidgetClass.h"
#include "FlagClass.h"
#include "Runtime/Engine/Classes/GameFramework/Controller.h"
#include "CaptureTheFlagCharacter.generated.h"

#define TRACE_WEAPON ECC_GameTraceChannel1

#pragma region structs/enums
UENUM(BlueprintType)
namespace EWeaponType
{
	enum ProjectileType
	{
		Ebullet			UMETA(DisplayName = "Bullet"),
		EProjectile		UMETA(DisplayName = "Projectile"),
	};
}

USTRUCT()
struct FTakeHitInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	float ActualDamage;

	UPROPERTY()
	UClass* DamageTypeClass;

	UPROPERTY()
	TWeakObjectPtr<class ACaptureTheFlagCharacter> PawnInstigator;

	UPROPERTY()
	TWeakObjectPtr<class AActor> DamageCauser;

	UPROPERTY()
	uint8 DamageEventClassID;

	UPROPERTY()
	bool bKilled;

private:

	UPROPERTY()
		uint8 EnsureReplicationByte;

	UPROPERTY()
		FDamageEvent GeneralDamageEvent;

	UPROPERTY()
		FPointDamageEvent PointDamageEvent;

	UPROPERTY()
		FRadialDamageEvent RadialDamageEvent;

public:
	FTakeHitInfo()
		: ActualDamage(0),
		DamageTypeClass(nullptr),
		PawnInstigator(nullptr),
		DamageCauser(nullptr),
		DamageEventClassID(0),
		bKilled(false),
		EnsureReplicationByte(0)
	{}

	FDamageEvent& GetDamageEvent()
	{
		switch (DamageEventClassID)
		{
		case FPointDamageEvent::ClassID:
			if (PointDamageEvent.DamageTypeClass == nullptr)
			{
				PointDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
			}
			return PointDamageEvent;

		case FRadialDamageEvent::ClassID:
			if (RadialDamageEvent.DamageTypeClass == nullptr)
			{
				RadialDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
			}
			return RadialDamageEvent;

		default:
			if (GeneralDamageEvent.DamageTypeClass == nullptr)
			{
				GeneralDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
			}
			return GeneralDamageEvent;
		}
	}


	void SetDamageEvent(const FDamageEvent& DamageEvent)
	{
		DamageEventClassID = DamageEvent.GetTypeID();
		switch (DamageEventClassID)
		{
		case FPointDamageEvent::ClassID:
			PointDamageEvent = *((FPointDamageEvent const*)(&DamageEvent));
			break;
		case FRadialDamageEvent::ClassID:
			RadialDamageEvent = *((FRadialDamageEvent const*)(&DamageEvent));
			break;
		default:
			GeneralDamageEvent = DamageEvent;
		}
	}


	void EnsureReplication()
	{
		EnsureReplicationByte++;
	}
};

USTRUCT()
struct FInstantWeaponData
{
	GENERATED_USTRUCT_BODY()

		/** base weapon spread (degrees) */
		UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		float WeaponSpread;

	/** targeting spread modifier */
	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		float TargetingSpreadMod;

	/** continuous firing: spread increment */
	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		float FiringSpreadIncrement;

	/** continuous firing: max increment */
	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		float FiringSpreadMax;

	/** weapon range */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
		float WeaponRange;

	/** damage amount */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
		int32 HitDamage;

	/** type of damage */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
		TSubclassOf<UDamageType> DamageType;

	/** hit verification: scale for bounding box of hit actor */
	UPROPERTY(EditDefaultsOnly, Category = HitVerification)
		float ClientSideHitLeeway;

	/** hit verification: threshold for dot product between view direction and hit direction */
	UPROPERTY(EditDefaultsOnly, Category = HitVerification)
		float AllowedViewDotHitDir;

	/** defaults */
	FInstantWeaponData()
	{
		WeaponSpread = 5.0f;
		TargetingSpreadMod = 0.25f;
		FiringSpreadIncrement = 1.0f;
		FiringSpreadMax = 10.0f;
		WeaponRange = 10000.0f;
		HitDamage = 10;
		DamageType = UDamageType::StaticClass();
		ClientSideHitLeeway = 200.0f;
		AllowedViewDotHitDir = 0.8f;
	}
};
USTRUCT()
struct FInstantHitInfo
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY()
		FVector Origin;

	UPROPERTY()
		float ReticleSpread;

	UPROPERTY()
		int32 RandomSeed;
};

#pragma endregion

UCLASS(config=Game)
class ACaptureTheFlagCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

	FTimerHandle FireDelay;

	void TestPoints();

	bool bCanFire;

	void ResetFireDelay();

	/** Called on the actor right before replication occurs */
	virtual void PreReplication(IRepChangedPropertyTracker & ChangedPropertyTracker) override;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	virtual void PossessedBy(AController* C) override;


	virtual void OnRep_PlayerState() override;

	FVector GetAdjustedAim();


public:
	ACaptureTheFlagCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	/** shoot offset */
	UPROPERTY(EditAnywhere, Category = Offset)
	float ShootOffset;

	UPROPERTY(EditAnywhere)
	bool bGettingFlag;

protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);


	virtual void Jump();



protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }


public:
	void Fire();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TEnumAsByte<EWeaponType::ProjectileType> ProjectileType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, Replicated)
	int32 MaxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, Replicated)
	float MaxClip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, Replicated)
	float ShotCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, Replicated)
	float WeaponSpread;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, Replicated)
	float WeaponRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, Replicated)
	float Cooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, Replicated)
	float AttackRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, Replicated)
	float DamageDealt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, Replicated)
	int32 CurrentAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, Replicated)
	int32 CurrentClip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, Replicated)
	float m_movementModifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, Replicated)
	float m_defaultMaxWalkSpeed;

	bool bGetFlag();

public:

	void GrappleFire();

protected:
	//Can this pawn move? We turn this to false if we are grappling, so that way we dont go off course.
	bool bCanMove;

	//Is this pawn grappling?
	bool bIsGrappling;

	//Where did our hook hit?
	FVector GrappleLocation;

	//When we are not grappling, what is our FOV?
	UPROPERTY(EditDefaultsOnly, Category = "Camera Config")
	float NonGrappleFOV;

	//When we are grappling, what is our FOV?
	UPROPERTY(EditDefaultsOnly, Category = "Camera Config")
	float GrappleFOV;

	//How fast is the transition speed between FOV's?
	UPROPERTY(EditDefaultsOnly, Category = "Camera Config")
	float GrappleFOVTransitionSpeed;

	//Camera shake, we shake the caemra when we launch into grapple. For polish purposes
	UPROPERTY(EditDefaultsOnly, Category = "Camera Config")
	TSubclassOf<class UCameraShake> CameraShakeClass;

	/** Hook class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Grapple Config")
	TSubclassOf<class AActor> HookProjectile;

	//Error Tolerance: How close does the pawn have to be to the grapple location in order for him to be considered not grappling anymore
	UPROPERTY(EditDefaultsOnly, Category = "Grapple Config")
	float GrappleErrorTolerance;

	UPROPERTY(EditDefaultsOnly, Category = "Grapple Config")
	float GrappleDistanceTolerance;

	//How fast do we grapple?
	UPROPERTY(EditDefaultsOnly, Category = "Grapple Config")
	float GrappleSpeed;

public:

	//When our hook hits something, call this. Is called via cast
	UFUNCTION(BlueprintCallable, Category = "Grapplehook Event")
	void GrappleEvent(FVector ImpactPoint);

protected:
	//Spawn the hook
	void SpawnHook();

	//Updated the camera FOV, called every frame
	void UpdateCameraFOV(float DeltaSeconds);

	//Move to the grapple hook location, called every frame. 
	void MoveToGrappleLocation(float DeltaSeconds);

	//Stop grappling and reset grapple variables.
	void StopGrapple();

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AActor> Particles;

	/*Server sided*/
public:



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, Replicated)
	float m_Health;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, Replicated)
	UPROPERTY(EditAnywhere)
	bool bHasFlag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, Replicated)
	bool bFinishedFlagTimer;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, Replicated)
	UPROPERTY(EditAnywhere)
	float m_FlagTimer;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, Replicated)
	int32 m_Kills;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, Replicated)
	int32 m_Deaths;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, Replicated)
	bool bWantsToInteract;

	void OnInteract();
	void OffInteract();
	

	int32 GetMaxHealth();
	int32 GetHealth();

	void PlayEffect(FVector SpawnLocation, FRotator SpawnRotation);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerPlayEffect(FVector SpawnLocation, FRotator SpawnRotation);


	void ResetFlag();

	void SetFlagHidden(AFlagClass* flag);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSetFlagHidden(AFlagClass* flag);






	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, Replicated)
	bool bIsDead;

	/* Take damage & handle death */
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	virtual bool CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const;

	virtual bool Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser);

	virtual void OnDeath(float KillingDamage, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser);


	virtual void PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled);

	void ReplicateHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled);

	/* Holds hit data to replicate hits and death to clients */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_LastTakeHitInfo)
	struct FTakeHitInfo LastTakeHitInfo;

	UFUNCTION()
	void OnRep_LastTakeHitInfo();

	bool bIsDying;

	void Suicide();

	void KilledBy(class APawn* EventInstigator);

	virtual void Destroyed() override;

	void TornOff();

	/** Time at which point the last take hit info for the actor times out and won't be replicated; Used to stop join-in-progress effects all over the screen */
	float LastTakeHitTimeTimeout;

	//////////////////////////////*Weapon*////////////////////////////////////

	/** server notified of hit from client to verify */
	UFUNCTION(reliable, server, WithValidation)
	void ServerNotifyHit(const FHitResult& Impact, FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread);


	/** server notified of miss to show trail FX */
//	UFUNCTION(unreliable, server, WithValidation)
//	void ServerNotifyMiss(FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread);


	/** continue processing the instant hit, as if it has been confirmed by the server */
	void ProcessInstantHit_Confirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread);

	/** check if weapon should deal damage to actor */
	bool ShouldDealDamage(AActor* TestActor) const;

	/** handle damage */
	void DealDamage(const FHitResult& Impact, const FVector& ShootDir);

	/** weapon config */
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FInstantWeaponData InstantConfig;

	/** instant hit notify for replication */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_HitNotify)

	FInstantHitInfo HitNotify;

 	UFUNCTION()
 	void OnRep_HitNotify();

	protected:
	FHitResult WeaponTrace(const FVector &Start, const FVector &End) const;

	void ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread);

public:
	void InstantFire();

	void DisplayScore();
	void ScoreAdd();

	UPROPERTY(Transient, EditDefaultsOnly)
	TArray<UMaterialInstanceDynamic*> MeshMIDs;

	/** Update the team color of all player meshes. */
	void UpdateTeamColorsAllMIDs();

	/** handle mesh colors on specified material instance */
	void UpdateTeamColors();

	/** update mesh for first person view */
	virtual void PawnClientRestart() override;

	UPROPERTY(Replicated)
	class UMaterialInterface* DefaultMaterial;
	UPROPERTY(Replicated)
	class UMaterialInterface* VulnerableMaterial;

	UFUNCTION(Reliable, Server, WithValidation)
	void SetColorsServer();

	void SetColors();


	void SetTeamColor();

	FTimerHandle FlagTimerHandle;

	void CountDownFlagTimer();






// 	/** [local] weapon specific fire implementation */
// 	virtual void FireWeapon() override;
// 
// 	/** [local + server] update spread on firing */
// 	virtual void OnBurstFinished() override;


};



