// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
	bAutoActivate = false;
}

void UPassiveNiagaraComponent::ActivateIfEquipped()
{
	if (UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
	{
		if (ASC->GetStatusFromAbilityTag(PassiveAbilityTag).MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped))
		{
			Activate();
		}
	}
}

void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
	{
		ASC->ActivatePassiveEffectDelegate.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);

		ActivateIfEquipped();
	}else if (TScriptInterface<ICombatInterface> CombatInterface = GetOwner())
	{
		CombatInterface->GetOnAscRegisteredDelegate().AddLambda([this] (UAbilitySystemComponent* InASC)
		{
			if (UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(InASC))
			{
				ASC->ActivatePassiveEffectDelegate.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
				ActivateIfEquipped();
			}
		});
	}
}

void UPassiveNiagaraComponent::OnPassiveActivate(const FGameplayTag& AbilityTag, const bool bActivate)
{
	if (PassiveAbilityTag.MatchesTagExact(AbilityTag))
	{
		if (bActivate && !IsActive())
		{
			Activate();
		}else
		{
			Deactivate();
		}
	}
}
