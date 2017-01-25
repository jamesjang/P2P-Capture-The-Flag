// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "CaptureTheFlag.h"
#include "Engine.h"
#include "DrawDebugHelpers.h"
#include "GameStateClass.h"
#include "CaptureTheFlagCharacter.h"
#include "CaptureTheFlagController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine.h"
#include "CaptureTheFlagGameMode.h"
#include "FlagClass.h"
#include "EngineUtils.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Net/UnrealNetwork.h"

//////////////////////////////////////////////////////////////////////////
// ACaptureTheFlagCharacter

ACaptureTheFlagCharacter::ACaptureTheFlagCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bPressedJump = false;
	bReplicateMovement = true;
	m_FlagTimer = 5.f;
	bIsDead = false;
	if (Role == ROLE_Authority)
	{
		m_Health = GetMaxHealth();
	}

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->AttachTo(GetCapsuleComponent());
	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true; //rotates player with mouse axis
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->MaxWalkSpeed = 700.0f;
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
//	CameraBoom->TargetArmLength = 200.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	ShootOffset = 0;

	bCanFire = true;

	m_defaultMaxWalkSpeed = 500.f;

	m_movementModifier = 1.f;
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>VulnerableMat(TEXT("'/Game/FutureSoldiers/materials/FutureSoldier_01_Inst.FutureSoldier_01_Inst'"));
	VulnerableMaterial = VulnerableMat.Object;


	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void ACaptureTheFlagCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultMaterial = GetMesh()->GetMaterial(0);


}

void ACaptureTheFlagCharacter::TestPoints()
{

}

void ACaptureTheFlagCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();


}

void ACaptureTheFlagCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Role == ROLE_Authority)
	{
		m_Health = GetMaxHealth();
	}

	if (Role == ROLE_Authority)
	{
		GetMesh()->SetMaterial(0, VulnerableMaterial);
	}
}

void ACaptureTheFlagCharacter::SetTeamColor()
{
	GetMesh()->SetMaterial(0, VulnerableMaterial);
}

void ACaptureTheFlagCharacter::UpdateTeamColorsAllMIDs()
{

}

void ACaptureTheFlagCharacter::UpdateTeamColors()
{

}

void ACaptureTheFlagCharacter::PossessedBy(class AController* InController)
{
	Super::PossessedBy(InController);

}

void ACaptureTheFlagCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (PlayerState != NULL)
	{
		ACaptureTheFlagPlayerState* PS = Cast<ACaptureTheFlagPlayerState>(this->PlayerState);



		if (PS->GetTeamNum() == 0)
		{
			GetMesh()->SetMaterial(0, VulnerableMaterial);
		//	SetColors();
		}
	}
}

void ACaptureTheFlagCharacter::SetColors()
{
	ACaptureTheFlagPlayerState* PS = Cast<ACaptureTheFlagPlayerState>(this->PlayerState);

	if (PS->GetTeamNum() == 0)
	{
		GetMesh()->SetMaterial(0, VulnerableMaterial);

		if (Role == ROLE_Authority)
		{
			SetColorsServer();
		}			
	}

}

void ACaptureTheFlagCharacter::SetColorsServer_Implementation()
{
	SetColors();
}

bool ACaptureTheFlagCharacter::SetColorsServer_Validate()
{
	return true;
}




void ACaptureTheFlagCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACaptureTheFlagCharacter, m_Health);
	DOREPLIFETIME(ACaptureTheFlagCharacter, MaxClip);
	DOREPLIFETIME(ACaptureTheFlagCharacter, MaxAmmo);
	DOREPLIFETIME(ACaptureTheFlagCharacter, ShotCost);
	DOREPLIFETIME(ACaptureTheFlagCharacter, WeaponSpread);
	DOREPLIFETIME(ACaptureTheFlagCharacter, WeaponRange);
	DOREPLIFETIME(ACaptureTheFlagCharacter, AttackRate);
	DOREPLIFETIME(ACaptureTheFlagCharacter, DamageDealt);
	DOREPLIFETIME(ACaptureTheFlagCharacter, CurrentAmmo);
	DOREPLIFETIME(ACaptureTheFlagCharacter, CurrentClip);
	DOREPLIFETIME(ACaptureTheFlagCharacter, m_Kills);
	DOREPLIFETIME(ACaptureTheFlagCharacter, m_Deaths);
	DOREPLIFETIME(ACaptureTheFlagCharacter, bIsDead);
	DOREPLIFETIME(ACaptureTheFlagCharacter, m_movementModifier);
	DOREPLIFETIME(ACaptureTheFlagCharacter, m_defaultMaxWalkSpeed);
	DOREPLIFETIME(ACaptureTheFlagCharacter, DefaultMaterial);
	DOREPLIFETIME(ACaptureTheFlagCharacter, VulnerableMaterial);


	DOREPLIFETIME(ACaptureTheFlagCharacter, LastTakeHitInfo);

}

void ACaptureTheFlagCharacter::PreReplication(IRepChangedPropertyTracker & ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	// Only replicate this property for a short duration after it changes so join in progress players don't get spammed with fx when joining late
	DOREPLIFETIME_ACTIVE_OVERRIDE(ACaptureTheFlagCharacter, LastTakeHitInfo, GetWorld() && GetWorld()->GetTimeSeconds() < LastTakeHitTimeTimeout);
}


void ACaptureTheFlagCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bHasFlag)
	{
	//	m_FlagTimer -= DeltaSeconds;
	}

	if (bIsGrappling && GetActorLocation().Z > 450.f)
	{
		StopJumping();
	}

	float DistanceMin = (GrappleLocation - GetActorLocation()).Size();

	if (DistanceMin < GrappleDistanceTolerance)
	{
		StopGrapple();
	}

	MoveToGrappleLocation(DeltaSeconds);
}

void ACaptureTheFlagCharacter::ResetFlag()
{
	m_FlagTimer = 5.f;

//	GetWorldSettings()->GetEffectiveTimeDilation(), true

	ACaptureTheFlagController* MyController = Cast<ACaptureTheFlagController>(Controller);
	if (Controller)
	{
		MyController->ToggleMyUIOff();

		MyController->SetIgnoreMoveInput(false);
		MyController->SetIgnoreLookInput(false);
	}

}

void ACaptureTheFlagCharacter::CountDownFlagTimer()
{
	m_FlagTimer = m_FlagTimer - 0.1f;
}

void ACaptureTheFlagCharacter::OnInteract()
{
	if (bGetFlag() && bHasFlag != true)
	{
		bHasFlag = true;

		GetWorld()->GetTimerManager().SetTimer(FlagTimerHandle, this, &ACaptureTheFlagCharacter::CountDownFlagTimer, 0.1f, true);
		


		AGameStateClass* GS = Cast<AGameStateClass>(GetWorld()->GameState);

		ACaptureTheFlagPlayerState* PS = Cast<ACaptureTheFlagPlayerState>(this->PlayerState);

		if (PS->GetTeamNum() == 0)
		{
			GS->bRedTeamFlag = true;

			for (TActorIterator<AFlagClass> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				if (ActorItr->TeamNum == 1)
				{
					AFlagClass *Flag = *ActorItr;
					SetFlagHidden(Flag);

				}
			}
		}
		else
		{
			GS->bBlueTeamFlag = true;

			for (TActorIterator<AFlagClass> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				if (ActorItr->TeamNum == 0)
				{
					AFlagClass *Flag = *ActorItr;
					SetFlagHidden(Flag);

				}
			}
		}

		ACaptureTheFlagController* MyController = Cast<ACaptureTheFlagController>(Controller);
		if (MyController)
		{
			MyController->SetIgnoreMoveInput(true);
			MyController->SetIgnoreLookInput(true);

			MyController->ToggleMyUIOn();

			FTimerHandle UnusedHandle;
			GetWorld()->GetTimerManager().SetTimer(UnusedHandle,this, &ACaptureTheFlagCharacter::ResetFlag, 5.f, false);
		}
	}

}

void ACaptureTheFlagCharacter::SetFlagHidden(AFlagClass* flag)
{
	flag->SetActorHiddenInGame(true);

	if (Role < ROLE_Authority)
	{
		ServerSetFlagHidden(flag);
	}
}

void ACaptureTheFlagCharacter::ServerSetFlagHidden_Implementation(AFlagClass* flag)
{
	SetFlagHidden(flag);
}

bool ACaptureTheFlagCharacter::ServerSetFlagHidden_Validate(AFlagClass* flag)
{
	return true;
}


void ACaptureTheFlagCharacter::OffInteract()
{
	
}

FVector ACaptureTheFlagCharacter::GetAdjustedAim()
{
	ACaptureTheFlagController* const PlayerController = Instigator ? Cast<ACaptureTheFlagController>(Instigator->Controller) : NULL;
	FVector FinalAim = FVector::ZeroVector;
	// If we have a player controller use it for the aim
	if (PlayerController)
	{
		FVector CamLoc;
		FRotator CamRot;
		PlayerController->GetPlayerViewPoint(CamLoc, CamRot);
		FinalAim = CamRot.Vector();
	}

	return FinalAim;
}

bool ACaptureTheFlagCharacter::bGetFlag()
{
	const FVector AimDir = GetAdjustedAim();

	const FVector StartTrace = GetActorLocation();

	const FVector EndTrace = StartTrace + AimDir * 200.f;

	const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

	AFlagClass* flag = Cast<AFlagClass>(Impact.GetActor());

	DrawDebugLine(this->GetWorld(), StartTrace, Impact.TraceEnd, FColor::Red, true, 1.5f, 10.f);

	ACaptureTheFlagPlayerState* PlayerState = Cast<ACaptureTheFlagPlayerState>(this->PlayerState);
	if (flag)
	{
		if (PlayerState->GetTeamNum() != flag->TeamNum)
		{
			return true;
		}
	}

	return false;
}




//////////////////////////////////////////////////////////////////////////
// Input

void ACaptureTheFlagCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACaptureTheFlagCharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAction("Interact", IE_Pressed, this, &ACaptureTheFlagCharacter::OnInteract);
	InputComponent->BindAction("Interact", IE_Released, this, &ACaptureTheFlagCharacter::OffInteract);

	InputComponent->BindAction("Fire", IE_Pressed, this, &ACaptureTheFlagCharacter::Fire);
	InputComponent->BindAction("Hook", IE_Pressed, this, &ACaptureTheFlagCharacter::GrappleFire);
	InputComponent->BindAction("TakeDamage", IE_Pressed, this, &ACaptureTheFlagCharacter::TestPoints);

	InputComponent->BindAxis("MoveForward", this, &ACaptureTheFlagCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ACaptureTheFlagCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &ACaptureTheFlagCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &ACaptureTheFlagCharacter::LookUpAtRate);
}

#pragma region Movement

void ACaptureTheFlagCharacter::Jump()
{
	if (!bIsGrappling)
	{
		bPressedJump = true;
		JumpKeyHoldTime = 0.0f;
	}
}

void ACaptureTheFlagCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACaptureTheFlagCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ACaptureTheFlagCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f) && CanJump() && !bIsDead)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ACaptureTheFlagCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) && CanJump() && !bIsDead)
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}


#pragma endregion Basic Movement Functions

#pragma region Weapon
void ACaptureTheFlagCharacter::Fire()
{
	if (ProjectileType == EWeaponType::Ebullet)
	{
		if (bCanFire)
		{
			if (CurrentAmmo > 0)
			{
				bCanFire = false;
				GetWorld()->GetTimerManager().SetTimer(FireDelay, this, &ACaptureTheFlagCharacter::ResetFireDelay, 0.5f, false);
				InstantFire();
				CurrentAmmo--;
				AttackRate = 0;
			}
		}
	}
}

void ACaptureTheFlagCharacter::ResetFireDelay()
{
	bCanFire = true;
}


void ACaptureTheFlagCharacter::InstantFire()
{
	const int32 RandomSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(RandomSeed);

	const FVector AimDir = GetAdjustedAim();

	const FVector StartTrace = GetActorLocation();

	const FVector EndTrace = StartTrace + AimDir * WeaponRange;

	const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

	float CurrentSpread = 5.f;

	ProcessInstantHit(Impact, StartTrace, AimDir, RandomSeed, CurrentSpread);
}



bool ACaptureTheFlagCharacter::ServerNotifyHit_Validate(const FHitResult& Impact, FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread)
{
	return true;
}

void ACaptureTheFlagCharacter::ServerNotifyHit_Implementation(const FHitResult& Impact, FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread)
{
	const float WeaponAngleDot = FMath::Abs(FMath::Sin(ReticleSpread * PI / 180.f));

	// if we have an instigator, calculate dot between the view and the shot
	if (Instigator && (Impact.GetActor() || Impact.bBlockingHit))
	{
		const FVector Origin = GetMesh()->GetSocketLocation("WeaponFire");
		const FVector ViewDir = (Impact.Location - Origin).GetSafeNormal();

		// is the angle between the hit and the view within allowed limits (limit + weapon max angle)
		const float ViewDotHitDir = FVector::DotProduct(Instigator->GetViewRotation().Vector(), ViewDir);
		if (ViewDotHitDir > InstantConfig.AllowedViewDotHitDir - WeaponAngleDot)
		{
				if (Impact.GetActor() == NULL)
				{
					if (Impact.bBlockingHit)
					{
						ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
					}
				}
				// assume it told the truth about static things because the don't move and the hit 
				// usually doesn't have significant gameplay implications
				else if (Impact.GetActor()->IsRootComponentStatic() || Impact.GetActor()->IsRootComponentStationary())
				{
					ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
				}
				else
				{
					// Get the component bounding box
					const FBox HitBox = Impact.GetActor()->GetComponentsBoundingBox();

					// calculate the box extent, and increase by a leeway
					FVector BoxExtent = 0.5 * (HitBox.Max - HitBox.Min);
					BoxExtent *= InstantConfig.ClientSideHitLeeway;

					// avoid precision errors with really thin objects
					BoxExtent.X = FMath::Max(20.0f, BoxExtent.X);
					BoxExtent.Y = FMath::Max(20.0f, BoxExtent.Y);
					BoxExtent.Z = FMath::Max(20.0f, BoxExtent.Z);

					// Get the box center
					const FVector BoxCenter = (HitBox.Min + HitBox.Max) * 0.5;

					// if we are within client tolerance
					if (FMath::Abs(Impact.Location.Z - BoxCenter.Z) < BoxExtent.Z &&
						FMath::Abs(Impact.Location.X - BoxCenter.X) < BoxExtent.X &&
						FMath::Abs(Impact.Location.Y - BoxCenter.Y) < BoxExtent.Y)
					{
						ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
					}
				}
		}
	}
}

void ACaptureTheFlagCharacter::PlayEffect(FVector SpawnLocation, FRotator SpawnRotation)
{
	GetWorld()->SpawnActor<AActor>(Particles, SpawnLocation, SpawnRotation);

	if (Role < ROLE_Authority)
	{
		ServerPlayEffect( SpawnLocation, SpawnRotation);
	}
}

void ACaptureTheFlagCharacter::ServerPlayEffect_Implementation(FVector SpawnLocation, FRotator SpawnRotation)
{
	PlayEffect(SpawnLocation, SpawnRotation);
}

bool ACaptureTheFlagCharacter::ServerPlayEffect_Validate(FVector SpawnLocation, FRotator SpawnRotation)
{
	return true;
}



FHitResult ACaptureTheFlagCharacter::WeaponTrace(const FVector &Start, const FVector &End) const
{
	static FName WeaponFireTag = FName(TEXT("WeaponTrace"));

	FCollisionQueryParams TraceParams(WeaponFireTag, true, Instigator);

	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.AddIgnoredActor(this);

	FHitResult Hit(ForceInit);

	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, TRACE_WEAPON, TraceParams);

	return Hit;

}

void ACaptureTheFlagCharacter::ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread)
{
	if (this->IsLocallyControlled() && GetNetMode() == NM_Client)
	{
		// if we're a client and we've hit something that is being controlled by the server
		if (Impact.GetActor() && Impact.GetActor()->GetRemoteRole() == ROLE_Authority)
		{
			// notify the server of the hit
			ServerNotifyHit(Impact, ShootDir, RandomSeed, ReticleSpread);
		}
		else if (Impact.GetActor() == NULL)
		{
			if (Impact.bBlockingHit)
			{
				// notify the server of the hit
				ServerNotifyHit(Impact, ShootDir, RandomSeed, ReticleSpread);
			}
		}
	}

	// process a confirmed hit
	ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);


	DrawDebugLine(this->GetWorld(), Origin, Impact.TraceEnd, FColor::Red, true, 1.5f, 10.f);
}

void ACaptureTheFlagCharacter::ProcessInstantHit_Confirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread)
{
	// handle damage
	if (ShouldDealDamage(Impact.GetActor()))
	{
		DealDamage(Impact, ShootDir);
	}

	// play FX on remote clients
	if (Role == ROLE_Authority)
	{
		HitNotify.Origin = Origin;
		HitNotify.RandomSeed = RandomSeed;
		HitNotify.ReticleSpread = ReticleSpread;
	}

	// play FX locally
	if (GetNetMode() != NM_DedicatedServer)
	{
		const FVector EndTrace = Origin + ShootDir * InstantConfig.WeaponRange;
		const FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;

		PlayEffect(Impact.ImpactPoint, FRotator(0, 0, 0));
	}
}

bool ACaptureTheFlagCharacter::ShouldDealDamage(AActor* TestActor) const
{
	// if we're an actor on the server, or the actor's role is authoritative, we should register damage
	if (TestActor)
	{
		if (GetNetMode() != NM_Client ||
			TestActor->Role == ROLE_Authority ||
			TestActor->bTearOff)
		{
			return true;
		}
	}

	return false;
}

void ACaptureTheFlagCharacter::DealDamage(const FHitResult& Impact, const FVector& ShootDir)
{
	FPointDamageEvent PointDmg;
	PointDmg.DamageTypeClass = InstantConfig.DamageType;
	PointDmg.HitInfo = Impact;
	PointDmg.ShotDirection = ShootDir;
	PointDmg.Damage = InstantConfig.HitDamage;

	Impact.GetActor()->TakeDamage(PointDmg.Damage, PointDmg, Controller, this);
}

void ACaptureTheFlagCharacter::OnRep_HitNotify()
{
	//SimulateInstantHit(HitNotify.Origin, HitNotify.RandomSeed, HitNotify.ReticleSpread);
}

#pragma endregion

#pragma region Grapple

void ACaptureTheFlagCharacter::GrappleFire()
{
	GrappleLocation = FVector::ZeroVector;
	StopJumping();
	SpawnHook();
}


void ACaptureTheFlagCharacter::SpawnHook()
{
	// try and fire a projectile
	if (HookProjectile != NULL)
	{


		const FRotator SpawnRotation = GetControlRotation();

	//	const FVector FP_MuzzleLocation = GetMesh()->GetSocketLocation("WeaponFire");
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector SpawnLocation = GetMesh()->GetSocketLocation("WeaponFire");
			//((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			// spawn the projectile at the muzzle
			World->SpawnActor<AActor>(HookProjectile, SpawnLocation, SpawnRotation);
		}
	}

}

void ACaptureTheFlagCharacter::GrappleEvent(FVector ImpactPoint)
{
	//GrappleLocation = FVector::ZeroVector;

	if (!bIsGrappling )
	{
		//GrappleLocation = FVector::ZeroVector;
		GrappleLocation = ImpactPoint;
		
		float DistanceMin = (GrappleLocation - GetActorLocation()).Size();

		if (DistanceMin > GrappleDistanceTolerance)
		{
			StopJumping();
			bCanMove = true; //We turn off movement so that way we dont go off cource mid air.
			bIsGrappling = true;

			//Play a camera shake to give the illsuion of sudden movement...
			if (CameraShakeClass)
				UGameplayStatics::PlayWorldCameraShake(this, CameraShakeClass, GetActorLocation(), 0.f, 500.f);

			//If our grapple location is higher than we are currently at...
			if (GrappleLocation.Z > GetActorLocation().Z)
			{
				LaunchCharacter(FVector(0.f, 0.f, 400.f), false, false);
				GetCharacterMovement()->GravityScale = 0.f;
			}
		}
	}
}

void ACaptureTheFlagCharacter::UpdateCameraFOV(float DeltaSeconds)
{
	const float CurrentFOV = FollowCamera->FieldOfView;
	const float TargetFOV = (bIsGrappling ? GrappleFOV : NonGrappleFOV);

	//FInterp to our target FOV, then set the camera FOV
	const float NewFOV = UKismetMathLibrary::FInterpTo(CurrentFOV, TargetFOV, DeltaSeconds, GrappleFOVTransitionSpeed);

	FollowCamera->SetFieldOfView(NewFOV);
}

void ACaptureTheFlagCharacter::MoveToGrappleLocation(float DeltaSeconds)
{
	if (bIsGrappling)
	{
		const FVector CurrentLocation = GetActorLocation();
		const FVector NewLocation = UKismetMathLibrary::VInterpTo(CurrentLocation, GrappleLocation, DeltaSeconds, GrappleSpeed);

		FHitResult Hit;

		//If we hit something while grappling
		if (SetActorLocation(NewLocation, true, &Hit))
		{
			//If the actor we hit is a valid actor
			if (Hit.GetActor())
			{
				//if the actor we hit was a world static object (walls, floors, etc) stop grappling
				if (Hit.GetComponent()->GetCollisionObjectType() == ECC_WorldStatic)
				{
					StopGrapple();
				}
			}
		}

		//If we are nearly close to our grapple location, stop grappling.
		else if (UKismetMathLibrary::EqualEqual_VectorVector(CurrentLocation, GrappleLocation, GrappleErrorTolerance))
		{
			StopGrapple();
		}
	}
}

void ACaptureTheFlagCharacter::StopGrapple()
{
	bIsGrappling = false;
	bCanMove = true;
	StopJumping();
	GetCharacterMovement()->GravityScale = 1.f;
}


#pragma endregion Grapple Events



int32 ACaptureTheFlagCharacter::GetMaxHealth()
{
	return 100;
}

#pragma region new Death and damage events

float ACaptureTheFlagCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	if (m_Health <= 0.f)
	{
		return 0.f;
	}

	//modify damage
	ACaptureTheFlagGameMode* const Game = GetWorld()->GetAuthGameMode<ACaptureTheFlagGameMode>();
	Damage = Game ? Game->ModifyDamage(Damage, this, DamageEvent, EventInstigator, DamageCauser) : 0.f;

	//if game is valid, modify damage, if it isnt valid, damage is 0

	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage > 0.f)
	{
		m_Health -= ActualDamage;
		if (m_Health <= 0)
		{
			bool bCanDie = true;

			if (bCanDie)
			{
				Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
			}
			else
			{
				m_Health = 1.0f;
			}
		}
	}

	return ActualDamage;
}

bool ACaptureTheFlagCharacter::CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const
{
	if (bIsDying ||
		IsPendingKill() ||
		Role != ROLE_Authority ||
		GetWorld()->GetAuthGameMode() == NULL ||
		GetWorld()->GetAuthGameMode()->GetMatchState() == MatchState::LeavingMap)
	{
		return false;
	}

	return true;
}

bool ACaptureTheFlagCharacter::Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser)
{
	if (!CanDie(KillingDamage, DamageEvent, Killer, DamageCauser))
	{
		return false;
	}

	m_Health = FMath::Min(0.0f, m_Health);

	/* Fallback to default DamageType if none is specified */
	UDamageType const* const DamageType = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
	Killer = GetDamageInstigator(Killer, *DamageType);

	/* Notify the gamemode we got killed for scoring and game over state */
	AController* KilledPlayer = Controller ? Controller : Cast<AController>(GetOwner());

	GetWorld()->GetAuthGameMode<ACaptureTheFlagGameMode>()->Killed(Killer, KilledPlayer, this, DamageType);

	GetCharacterMovement()->ForceReplicationUpdate();

	OnDeath(KillingDamage, DamageEvent, Killer ? Killer->GetPawn() : NULL, DamageCauser);
	return true;
}


void ACaptureTheFlagCharacter::OnDeath(float KillingDamage, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser)
{
	if (bIsDying)
	{
		return;
	}

	bReplicateMovement = false;
	bTearOff = true;
	bIsDying = true;

	if (Role == ROLE_Authority)
	{
		ReplicateHit(KillingDamage, DamageEvent, PawnInstigator, DamageCauser, true);
	}

	DetachFromControllerPendingDestroy();

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);

	SetActorEnableCollision(true);

	// hide and set short lifespan
	TurnOff();
	SetActorHiddenInGame(true);
	SetLifeSpan(3.0f);
}


void ACaptureTheFlagCharacter::PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled)
{
	if (Role == ROLE_Authority)
	{
		ReplicateHit(DamageTaken, DamageEvent, PawnInstigator, DamageCauser, bKilled);
	}
}

void ACaptureTheFlagCharacter::ReplicateHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled)
{
	const float TimeoutTime = GetWorld()->GetTimeSeconds() + 0.5f;

	FDamageEvent const& LastDamageEvent = LastTakeHitInfo.GetDamageEvent();
	if (PawnInstigator == LastTakeHitInfo.PawnInstigator.Get() && LastDamageEvent.DamageTypeClass == LastTakeHitInfo.DamageTypeClass)
	{
		// Same frame damage
		if (bKilled && LastTakeHitInfo.bKilled)
		{
			// Redundant death take hit, ignore it
			return;
		}

		DamageTaken += LastTakeHitInfo.ActualDamage;
	}

	LastTakeHitInfo.ActualDamage = DamageTaken;
	LastTakeHitInfo.PawnInstigator = Cast<ACaptureTheFlagCharacter>(PawnInstigator);
	LastTakeHitInfo.DamageCauser = DamageCauser;
	LastTakeHitInfo.SetDamageEvent(DamageEvent);
	LastTakeHitInfo.bKilled = bKilled;
	LastTakeHitInfo.EnsureReplication();

	LastTakeHitTimeTimeout = TimeoutTime;
}

void ACaptureTheFlagCharacter::OnRep_LastTakeHitInfo()
{
	if (LastTakeHitInfo.bKilled)
	{
		OnDeath(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get());
	}
	else
	{
		PlayHit(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get(), LastTakeHitInfo.bKilled);
	}
}

void ACaptureTheFlagCharacter::Suicide()
{
	KilledBy(this);
}


void ACaptureTheFlagCharacter::KilledBy(class APawn* EventInstigator)
{
	if (Role == ROLE_Authority && !bIsDying)
	{
		AController* Killer = nullptr;
		if (EventInstigator != nullptr)
		{
			Killer = EventInstigator->Controller;
			LastHitBy = nullptr;
		}

		Die(m_Health, FDamageEvent(UDamageType::StaticClass()), Killer, nullptr);
	}
}

void ACaptureTheFlagCharacter::Destroyed()
{
	Super::Destroyed();
}

void ACaptureTheFlagCharacter::TornOff()
{
	SetLifeSpan(25.f);
}

#pragma  endregion Death and damage events

void ACaptureTheFlagCharacter::DisplayScore()
{
	AGameStateClass* MyGameState = Cast<AGameStateClass>(GetWorld()->GameState);
	if (MyGameState->TeamScores.Num() > 0)
	{

	}
}



