// Copyright Epic Games, Inc. All Rights Reserved.


#include "Variant_Shooter/AI/ShooterNPC.h"
#include "ShooterWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "ShooterGameMode.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Components/TextRenderComponent.h"
#include "Components/WidgetComponent.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

void AShooterNPC::BeginPlay()
{
	Super::BeginPlay();

	HideDebugTextComponents();

	if (!HasAuthority())
	{
		return;
	}

	if (!WeaponClass)
	{
		WeaponClass = LoadClass<AShooterWeapon>(nullptr, TEXT("/Game/Variant_Shooter/Blueprints/Pickups/Weapons/BP_ShooterWeapon_Pistol.BP_ShooterWeapon_Pistol_C"));
	}

	// spawn the weapon
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	Weapon = GetWorld()->SpawnActor<AShooterWeapon>(WeaponClass, GetActorTransform(), SpawnParams);
	if (Weapon)
	{
		Weapon->SetInfiniteAmmo(true);
	}
}

void AShooterNPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HideDebugTextComponents();

	if (!HasAuthority() || bIsDead || !Weapon)
	{
		return;
	}

	AActor* Target = FindFallbackTarget();
	if (!Target)
	{
		ResetCombatTiming(nullptr);
		StopShooting();
		return;
	}

	if (CombatTarget != Target)
	{
		ResetCombatTiming(Target);
	}

	const FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
	const float DistanceToTarget = ToTarget.Size();
	const FRotator TargetRotation = FRotator(0.0f, ToTarget.Rotation().Yaw, 0.0f);
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 7.0f));

	const FVector DesiredLocation = GetFallbackDesiredLocation(Target);
	FVector MoveDirection = DesiredLocation - GetActorLocation();
	MoveDirection.Z = 0.0f;

	const FVector SeparationDirection = GetSeparationDirection();
	if (!SeparationDirection.IsNearlyZero())
	{
		MoveDirection += SeparationDirection * SeparationStrength * SeparationRadius;
	}

	if (DistanceToTarget < CloseCombatDistance)
	{
		MoveDirection -= ToTarget.GetSafeNormal2D() * PreferredCombatDistance;
	}

	if (!bAttackBurstActive && TargetDetectionStartTime >= 0.0f)
	{
		FVector StrafeDirection = FVector::CrossProduct(FVector::UpVector, ToTarget.GetSafeNormal2D()).GetSafeNormal();
		if (GetUniqueID() % 2 == 0)
		{
			StrafeDirection *= -1.0f;
		}
		MoveDirection += StrafeDirection * RepositionStrafeStrength;
	}

	if (MoveDirection.SizeSquared2D() > FMath::Square(MoveAcceptanceRadius))
	{
		const FVector MoveTarget = GetActorLocation() + MoveDirection.GetSafeNormal2D() * FMath::Min(MoveDirection.Size2D(), 650.0f);
		if (AAIController* AIController = Cast<AAIController>(GetController()))
		{
			const float CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
			if (CurrentTime - LastMoveRequestTime >= MoveRequestInterval)
			{
				AIController->MoveToLocation(MoveTarget, MoveAcceptanceRadius, true);
				LastMoveRequestTime = CurrentTime;
			}
		}
		AddMovementInput(MoveDirection.GetSafeNormal2D(), FallbackMoveScale);
	}
	else if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
	}

	const float CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	if (DistanceToTarget > FireDistance || !HasLineOfSightToFallbackTarget(Target))
	{
		TargetDetectionStartTime = -1.0f;
		bAttackBurstActive = false;
		NextCombatStateChangeTime = CurrentTime + 0.25f;
		StopShooting();
		return;
	}

	if (TargetDetectionStartTime < 0.0f)
	{
		TargetDetectionStartTime = CurrentTime;
		NextCombatStateChangeTime = CurrentTime + DetectionDelay;
		StopShooting();
		return;
	}

	if (CurrentTime - TargetDetectionStartTime < DetectionDelay)
	{
		StopShooting();
		return;
	}

	UpdateCombatTiming(CurrentTime);

	if (bAttackBurstActive)
	{
		StartShooting(Target);
	}
	else
	{
		StopShooting();
	}
}

void AShooterNPC::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// clear the death timer
	GetWorld()->GetTimerManager().ClearTimer(DeathTimer);
}

float AShooterNPC::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!HasAuthority() || bIsDead)
	{
		return 0.0f;
	}

	CurrentHP -= Damage;

	if (CurrentHP <= 0.0f)
	{
		Die(EventInstigator);
	}

	return Damage;
}

void AShooterNPC::AttachWeaponMeshes(AShooterWeapon* WeaponToAttach)
{
	const FAttachmentTransformRules AttachmentRule(EAttachmentRule::SnapToTarget, false);

	// attach the weapon actor
	WeaponToAttach->AttachToActor(this, AttachmentRule);

	// attach the weapon meshes
	WeaponToAttach->GetFirstPersonMesh()->AttachToComponent(GetFirstPersonMesh(), AttachmentRule, FirstPersonWeaponSocket);
	WeaponToAttach->GetThirdPersonMesh()->AttachToComponent(GetMesh(), AttachmentRule, ThirdPersonWeaponSocket);
}

void AShooterNPC::PlayFiringMontage(UAnimMontage* Montage)
{
	// unused
}

void AShooterNPC::AddWeaponRecoil(float Recoil)
{
	// unused
}

void AShooterNPC::UpdateWeaponHUD(int32 CurrentAmmo, int32 MagazineSize, int32 ReserveAmmo)
{
	// unused
}

FVector AShooterNPC::GetWeaponTargetLocation()
{
	// start aiming from the camera location
	const FVector AimSource = GetFirstPersonCameraComponent()->GetComponentLocation();

	FVector AimDir, AimTarget = FVector::ZeroVector;

	// do we have an aim target?
	if (CurrentAimTarget)
	{
		// target the actor location
		AimTarget = CurrentAimTarget->GetActorLocation();

		// apply a vertical offset to target head/feet
		AimTarget.Z += FMath::RandRange(MinAimOffsetZ, MaxAimOffsetZ);

		// get the aim direction and apply randomness in a cone
		AimDir = (AimTarget - AimSource).GetSafeNormal();
		AimDir = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(AimDir, AimVarianceHalfAngle);

		
	} else {

		// no aim target, so just use the camera facing
		AimDir = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(GetFirstPersonCameraComponent()->GetForwardVector(), AimVarianceHalfAngle);

	}

	// calculate the unobstructed aim target location
	AimTarget = AimSource + (AimDir * AimRange);

	// run a visibility trace to see if there's obstructions
	FHitResult OutHit;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(OutHit, AimSource, AimTarget, ECC_Visibility, QueryParams);

	// return either the impact point or the trace end
	return OutHit.bBlockingHit ? OutHit.ImpactPoint : OutHit.TraceEnd;
}

void AShooterNPC::AddWeaponClass(const TSubclassOf<AShooterWeapon>& InWeaponClass)
{
	// unused
}

void AShooterNPC::OnWeaponActivated(AShooterWeapon* InWeapon)
{
	// unused
}

void AShooterNPC::OnWeaponDeactivated(AShooterWeapon* InWeapon)
{
	// unused
}

void AShooterNPC::OnSemiWeaponRefire()
{
	// are we still shooting?
	if (bIsShooting && Weapon)
	{
		// fire the weapon
		Weapon->StartFiring();
	}
}

void AShooterNPC::Die(AController* KillerController)
{
	if (!HasAuthority() || bIsDead)
	{
		return;
	}

	bIsDead = true;
	ApplyDeathVisuals();

	Tags.Add(DeathTag);
	OnPawnDeath.Broadcast();

	if (AShooterGameMode* GM = Cast<AShooterGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->OnEnemyKilled(KillerController);
	}

	ApplyDeathVisuals();

	GetWorld()->GetTimerManager().SetTimer(DeathTimer, this, &AShooterNPC::DeferredDestruction, DeferredDestructionTime, false);
}

void AShooterNPC::ApplyDeathVisuals()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->StopActiveMovement();
	GetMesh()->SetCollisionProfileName(RagdollCollisionProfile);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetPhysicsBlendWeight(1.0f);
}

void AShooterNPC::OnRep_CurrentHP()
{
	// HP replication hook for future UI; death visuals are driven by bIsDead
}

void AShooterNPC::OnRep_IsDead()
{
	if (bIsDead)
	{
		ApplyDeathVisuals();
	}
}

void AShooterNPC::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterNPC, CurrentHP);
	DOREPLIFETIME(AShooterNPC, bIsDead);
}

void AShooterNPC::DeferredDestruction()
{
	Destroy();
}

void AShooterNPC::StartShooting(AActor* ActorToShoot)
{
	if (!Weapon || !ActorToShoot || bIsDead)
	{
		return;
	}

	// save the aim target
	CurrentAimTarget = ActorToShoot;

	// raise the flag
	bIsShooting = true;

	// signal the weapon
	Weapon->StartFiring();
}

void AShooterNPC::StopShooting()
{
	// lower the flag
	bIsShooting = false;

	// signal the weapon
	if (Weapon)
	{
		Weapon->StopFiring();
	}
}

void AShooterNPC::HideDebugTextComponents()
{
	TArray<UTextRenderComponent*> TextComponents;
	GetComponents<UTextRenderComponent>(TextComponents);
	for (UTextRenderComponent* TextComponent : TextComponents)
	{
		if (TextComponent)
		{
			TextComponent->SetText(FText::GetEmpty());
			TextComponent->SetHiddenInGame(true);
			TextComponent->SetVisibility(false);
		}
	}

	TArray<UWidgetComponent*> WidgetComponents;
	GetComponents<UWidgetComponent>(WidgetComponents);
	for (UWidgetComponent* WidgetComponent : WidgetComponents)
	{
		if (WidgetComponent)
		{
			WidgetComponent->SetHiddenInGame(true);
			WidgetComponent->SetVisibility(false);
		}
	}
}

AActor* AShooterNPC::FindFallbackTarget() const
{
	AActor* BestTarget = nullptr;
	float BestDistanceSq = TNumericLimits<float>::Max();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		const APlayerController* PlayerController = It->Get();
		APawn* Pawn = PlayerController ? PlayerController->GetPawn() : nullptr;
		if (!IsValid(Pawn) || Pawn->Tags.Contains(DeathTag))
		{
			continue;
		}

		const float DistanceSq = FVector::DistSquared(GetActorLocation(), Pawn->GetActorLocation());
		if (DistanceSq < BestDistanceSq)
		{
			BestDistanceSq = DistanceSq;
			BestTarget = Pawn;
		}
	}

	return BestTarget;
}

bool AShooterNPC::HasLineOfSightToFallbackTarget(AActor* Target) const
{
	if (!Target)
	{
		return false;
	}

	FHitResult Hit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(Weapon);
	const FVector Start = GetFirstPersonCameraComponent()->GetComponentLocation();
	const FVector End = Target->GetActorLocation() + FVector(0.0f, 0.0f, 60.0f);

	if (!GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams))
	{
		return true;
	}

	return Hit.GetActor() == Target;
}

FVector AShooterNPC::GetFallbackDesiredLocation(AActor* Target) const
{
	if (!Target)
	{
		return GetActorLocation();
	}

	const FVector TargetLocation = Target->GetActorLocation();
	const FVector FromTarget = (GetActorLocation() - TargetLocation).GetSafeNormal2D();
	const FVector BaseDirection = FromTarget.IsNearlyZero() ? -Target->GetActorForwardVector().GetSafeNormal2D() : FromTarget;
	const FVector RightDirection = FVector::CrossProduct(FVector::UpVector, BaseDirection).GetSafeNormal();

	const int32 Slot = static_cast<int32>(GetUniqueID() % 7);
	const float NormalizedSlot = (static_cast<float>(Slot) - 3.0f) / 3.0f;
	const float DistanceOffset = (Slot % 2 == 0) ? 110.0f : -70.0f;

	return TargetLocation + BaseDirection * (PreferredCombatDistance + DistanceOffset) + RightDirection * NormalizedSlot * StrafeSpreadRadius;
}

FVector AShooterNPC::GetSeparationDirection() const
{
	FVector Separation = FVector::ZeroVector;

	for (TActorIterator<AShooterNPC> It(GetWorld()); It; ++It)
	{
		const AShooterNPC* OtherNPC = *It;
		if (!OtherNPC || OtherNPC == this || OtherNPC->IsDead())
		{
			continue;
		}

		const FVector Away = GetActorLocation() - OtherNPC->GetActorLocation();
		const float Distance = Away.Size2D();
		if (Distance > KINDA_SMALL_NUMBER && Distance < SeparationRadius)
		{
			Separation += Away.GetSafeNormal2D() * (1.0f - Distance / SeparationRadius);
		}
	}

	return Separation.GetSafeNormal2D();
}

void AShooterNPC::ResetCombatTiming(AActor* NewTarget)
{
	CombatTarget = NewTarget;
	TargetDetectionStartTime = -1.0f;
	NextCombatStateChangeTime = 0.0f;
	bAttackBurstActive = false;
	CurrentAimTarget = nullptr;
}

void AShooterNPC::UpdateCombatTiming(float CurrentTime)
{
	if (CurrentTime < NextCombatStateChangeTime)
	{
		return;
	}

	if (bAttackBurstActive)
	{
		bAttackBurstActive = false;
		NextCombatStateChangeTime = CurrentTime + FMath::FRandRange(RepositionMinSeconds, RepositionMaxSeconds);
		StopShooting();
		return;
	}

	bAttackBurstActive = true;
	NextCombatStateChangeTime = CurrentTime + FMath::FRandRange(AttackBurstMinSeconds, AttackBurstMaxSeconds);
}
