// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/DamageGameplayAbility.h"
#include "AuraBeamSpell.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraBeamSpell : public UDamageGameplayAbility
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void StoreOwnerVariables();
	
	UFUNCTION(BlueprintCallable)
	void StoreMouseDataInfo(const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable)
	void TraceFirstTarget(const FVector& BeamTargetLocation);

	UFUNCTION(BlueprintCallable)
	void StoreAdditionalTargets(TArray<AActor*>& NearbyTargets);

	UFUNCTION(BlueprintImplementableEvent)
	void OnPrimaryTargetActorDied(AActor* DeadActor);

	UFUNCTION(BlueprintImplementableEvent)
	void OnAdditionalTargetActorDied(AActor* DeadActor);
protected:

	UPROPERTY(BlueprintReadWrite, Category="Beam")
	FVector MouseHitLocation;

	UPROPERTY(BlueprintReadWrite, Category="Beam")
	TObjectPtr<AActor> MouseHitActor;

	UPROPERTY(BlueprintReadWrite, Category="Beam")
	TObjectPtr<APlayerController> OwnerPlayerController;

	UPROPERTY(BlueprintReadWrite, Category="Beam")
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Beam")
	int32 MaxShockTargets = 7;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Beam")
	float BeamRadius = 850.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Beam")
	float DamageDeltaTime = 0.1;
};
