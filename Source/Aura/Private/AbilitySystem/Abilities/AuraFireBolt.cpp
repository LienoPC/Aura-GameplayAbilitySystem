// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"
#include "GameFramework/ProjectileMovementComponent.h"

void UAuraFireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag,
                                     bool bOverridePitch, float PitchOverride, AActor* HomingTarget)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer)
		return;

	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);

	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	if (bOverridePitch)
	{
		Rotation.Pitch = PitchOverride;
	}

	const int32 NumSpawnProjectiles = FMath::Min(NumProjectiles, GetAbilityLevel());
	//const int32 NumSpawnProjectiles = NumProjectiles;

	const FVector Forward = Rotation.Vector();
	auto Rotators = UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, ProjectileSpread, NumSpawnProjectiles);
	for (const auto& Rot : Rotators)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rot.Quaternion());

		AAuraProjectile* Projectile = Cast<AAuraProjectile>(GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn));

		Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults(nullptr);
		if (HomingTarget && HomingTarget->Implements<UCombatInterface>())
		{
			Projectile->ProjectileMovementComponent->HomingTargetComponent = HomingTarget->GetRootComponent();
		}else
		{
			Projectile->HomingTargetSceneComponent = NewObject<USceneComponent>(USceneComponent::StaticClass());
			Projectile->HomingTargetSceneComponent->SetWorldLocation(ProjectileTargetLocation);
			Projectile->ProjectileMovementComponent->HomingTargetComponent = Projectile->HomingTargetSceneComponent;
		}

		Projectile->ProjectileMovementComponent->HomingAccelerationMagnitude = FMath::FRandRange(HomingAccelerationMin, HomingAccelerationMax);
		Projectile->ProjectileMovementComponent->bIsHomingProjectile = bLaunchHoming;
		Projectile->FinishSpawning(SpawnTransform);
	}

	

}

FString UAuraFireBolt::GetDescription(int32 Level)
{
	const int32 DamageVal = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			// Title
			"<Title>FIRE BOLT</>\n\n"

			// Details
			"<Small>Level: </><Level>%d</>\n"
			// Mana Cost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			
			// Description
			"<Default>Launches a bolt of fire, "
			"exploding on impact and dealing: "
			"</><Damage>%d</><Default> fire "
			"damage with a chance to burn</>\n\n"
			),
			// Values
			Level, ManaCost, Cooldown, DamageVal);
	}
	else
	{
		return FString::Printf(TEXT(
					// Title
					"<Title>FIRE BOLT</>\n\n"

					// Details
					"<Small>Level: </><Level>%d</>\n"
					"<Small>ManaCost: </><ManaCost>%.1f</>\n"
					// Cooldown
					"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
					
					// Description
					"<Default>Launches %d a bolt of fire, "
					"exploding on impact and dealing: "
					"</><Damage>%d</><Default> fire "
					"damage with a chance to burn</>\n\n"
					),
					// Values
					Level, ManaCost, Cooldown, FMath::Min(Level, NumProjectiles), DamageVal);
	}
}

FString UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 DamageVal = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
						// Title
						"<Title>NEXT LEVEL</>\n\n"

						// Details
						"<Small>Level: </><Level>%d</>\n"
						"<Small>ManaCost: </><ManaCost>%.1f</>\n"
						// Cooldown
						"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

						// Description
						"<Default>Launches %d a bolt of fire, "
						"exploding on impact and dealing: "
						"</><Damage>%d</><Default> fire "
						"damage with a chance to burn</>\n\n"
						),
						// Values
						Level, ManaCost, Cooldown, FMath::Min(Level, NumProjectiles), DamageVal);
}