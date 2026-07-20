// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "PassiveNiagaraComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UPassiveNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()

public:

	UPassiveNiagaraComponent();
	void ActivateIfEquipped();

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag PassiveAbilityTag;

protected:
	virtual void BeginPlay() override;

	void OnPassiveActivate(const FGameplayTag& AbilityTag, const bool bActivate);
};
