// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AsyncTasks/WaitCooldownChange.h"

#include "AbilitySystemComponent.h"

UWaitCooldownChange* UWaitCooldownChange::WaitForCooldwonChange(UAbilitySystemComponent* AbilitySystemComponent,
                                                                const FGameplayTag& InCooldownTag)
{

	UWaitCooldownChange* NewCooldownChange = NewObject<UWaitCooldownChange>();
	NewCooldownChange->ASC = AbilitySystemComponent;
	NewCooldownChange->CooldownTag = InCooldownTag;

	if (!IsValid(AbilitySystemComponent) || !InCooldownTag.IsValid())
	{
		NewCooldownChange->EndTask();
		return nullptr;
	}
	AbilitySystemComponent->RegisterGameplayTagEvent(InCooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(NewCooldownChange, &UWaitCooldownChange::CooldwonTagChanged);
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(NewCooldownChange, &UWaitCooldownChange::OnActiveEffectAdded);
	return NewCooldownChange;
}

void UWaitCooldownChange::EndTask()
{
	if (!IsValid(ASC))
		return;
	
	ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

	SetReadyToDestroy();
	MarkAsGarbage();
}

void UWaitCooldownChange::CooldwonTagChanged(const FGameplayTag InCooldownTag, int32 NewCount)
{
	if (NewCount == 0)
	{
		OnCooldownEnd.Broadcast(0.f);
	}
	
}

void UWaitCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC,
	const FGameplayEffectSpec& EffectApplied, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer GrantedTags;
	FGameplayTagContainer AssetTags;
	EffectApplied.GetAllGrantedTags(GrantedTags);
	EffectApplied.GetAllAssetTags(AssetTags);
	if (AssetTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag))
	{
		FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
		TArray<float> TimesRemaining = ASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery);
		if (TimesRemaining.Num() > 0)
		{
			TimesRemaining.Sort();
			float TimeRemaining = TimesRemaining.Last();
			OnCooldownStart.Broadcast(TimeRemaining);
		}
	}
}
