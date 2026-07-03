// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "NiagaraSystem.h"
#include "AuraProjectile.generated.h"


class UGameplayEffect;
class UProjectileMovementComponent;
// Base class for all projectile actor that can be spawned within the game
UCLASS()
class AURA_API AAuraProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AAuraProjectile();

	UFUNCTION(BlueprintCallable)
	void OnHit();

	virtual void Destroyed() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FDamageEffectParams DamageEffectParams;

	UPROPERTY()
	TObjectPtr<USceneComponent> HomingTargetSceneComponent;
protected:
	virtual void BeginPlay() override;

	// Play the sound and spawn the effect of destroying projectile
	UFUNCTION(BlueprintCallable)
	virtual void OnOverlap(AActor* Other);

	UFUNCTION(BlueprintCallable)
	virtual void OnEndOverlap(AActor* Other);

	bool IsValidOverlap(AActor* Other);

	// Niagara effect played whenever the projectile hits something and it's destroyed.
	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	// Sound played whenever the projectile hits something and it's destroyed.
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound;

	// Looping sound played when the projectile is in air.
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> FlySound;

	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 15.f;

	bool bHit = false;

private:

	TObjectPtr<UAudioComponent> LoopingSoundComponent;
	
};
