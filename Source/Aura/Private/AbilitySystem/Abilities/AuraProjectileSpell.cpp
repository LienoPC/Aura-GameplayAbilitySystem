// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
}

void UAuraProjectileSpell::SpawnProjectile(const FVector &ProjectileTargetLocation)
{
	const bool isServer = !GetAvatarActorFromActorInfo()->HasAuthority();
	if (isServer)
		return;
	FTransform SpawnTransform;
	// TODO: Set projectile rotation
	
	if (TScriptInterface<ICombatInterface> CombatInterface = GetAvatarActorFromActorInfo())
	{
		const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
		SpawnTransform.SetLocation(SocketLocation);

		FRotator Rotation = (ProjectileTargetLocation-SocketLocation).Rotation();
		Rotation.Pitch = 0.0f;
		SpawnTransform.SetRotation(Rotation.Quaternion());


		AAuraProjectile* Projectile = Cast<AAuraProjectile>(GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn));

		// TODO: Give the projectile a GameplayEffectSpec for causing damage
		Projectile->FinishSpawning(SpawnTransform);
	}
}
