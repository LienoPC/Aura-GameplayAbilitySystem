// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "AuraFireBolt.generated.h"

/**
 * Fire bolt ability
 */
UCLASS()
class AURA_API UAuraFireBolt : public UAuraProjectileSpell
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void SpawnProjectiles(const FVector &ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride, AActor* HomingTarget);
	
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ProjectileSpread = 90.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HomingAccelerationMin = 1600.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HomingAccelerationMax = 2300.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bLaunchHoming = true;
};
