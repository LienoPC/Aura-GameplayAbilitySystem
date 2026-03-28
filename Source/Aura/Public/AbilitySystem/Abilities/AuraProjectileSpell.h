// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/DamageGameplayAbility.h"
#include "AuraProjectileSpell.generated.h"

class AAuraProjectile;
// Gameplay Ability class for all projectile types.
UCLASS()
class AURA_API UAuraProjectileSpell : public UDamageGameplayAbility
{
	GENERATED_BODY()

protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable)
	void SpawnProjectile(const FVector &ProjectileTargetLocation);
	// Class of projectile to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AAuraProjectile> ProjectileClass;
	

private:

	UPROPERTY()
	AAuraProjectile* SpawiningProjectile;
};
