// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Net/RepLayout.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}



void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();

}

void AAuraEffectActor::ApplyEffectToTarget(AActor* Target, TArray<TSubclassOf<UGameplayEffect>> GameplayEffectClass)
{
	if (TScriptInterface<IAbilitySystemInterface> ASCInterface = Target)
	{
		UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent();

		for (auto GameplayEffect : GameplayEffectClass)
		{
			ApplyEffect(GameplayEffect, TargetASC);
		}
	}
}

void AAuraEffectActor::ApplyEffect(TSubclassOf<UGameplayEffect> GameplayEffectClass, UAbilitySystemComponent* TargetASC)
{
	FGameplayEffectContextHandle Handle = TargetASC->MakeEffectContext();
	Handle.AddSourceObject(this);
	FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1.f, Handle);
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

	const bool bIsInfinite = SpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if (bIsInfinite && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		// Store handle to effect
		ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
	}
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	bool bApplied = false;
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectsClass);
		bApplied = true;
	}

	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectsClass);
		bApplied = true;
	}

	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectsClass);
		bApplied = true;
	}
	if (bApplied && bDestroyOnEffectRemoval)
	{
		Destroy();
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	bool bApplied = false;

	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectsClass);
		bApplied = true;
	}
	
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectsClass);
		bApplied = true;
	}

	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{	
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectsClass);
		bApplied = true;
	}

	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		if (TScriptInterface<IAbilitySystemInterface> ASCInterface = TargetActor)
		{
			UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent();
			if (!IsValid(TargetASC)) return;
			TArray<FActiveGameplayEffectHandle> HandlesToRemove;
			for (auto HandlePair : ActiveEffectHandles)
			{
				if (HandlePair.Value == TargetASC)
				{
					TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
					HandlesToRemove.Add(HandlePair.Key);
				}
			}

			for (auto& Handle : HandlesToRemove)
			{
				ActiveEffectHandles.FindAndRemoveChecked(Handle);
			}

		}
	}

	if (bApplied && bDestroyOnEffectRemoval)
	{
		Destroy();
	}
	
}





