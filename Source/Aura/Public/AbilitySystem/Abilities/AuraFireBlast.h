// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/DamageGameplayAbility.h"
#include "AuraFireBlast.generated.h"

class AAuraFireball;
/**
 * 
 */
UCLASS()
class AURA_API UAuraFireBlast : public UDamageGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;

	UFUNCTION(BlueprintCallable)
	TArray<AAuraFireball*> SpawnFireballs();
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FireBlast")
	TSubclassOf<AAuraFireball> FireballClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FireBlast")
	int32 NumFireballs = 12;
};

