// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/RepLayout.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

void AAuraEffectActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	RunningTime += DeltaSeconds;

	const float SinePeriod = 2*PI/SinPeriod;
	if (RunningTime > SinePeriod)
	{
		RunningTime = 0.0f;
	}
	ItemMovement(DeltaSeconds);
}


void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();

	InitialLocation = GetActorLocation();
	ItemLocation = GetActorLocation();
	ItemRotation = GetActorRotation();
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* Target, TArray<TSubclassOf<UGameplayEffect>> GameplayEffectClass)
{
	// Verify if we should apply effects to enemies
	if(Target->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;

	
	if (TScriptInterface<IAbilitySystemInterface> ASCInterface = Target)
	{
		UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent();

		for (auto GameplayEffect : GameplayEffectClass)
		{
			ApplyEffect(GameplayEffect, TargetASC);
		}
	}

}

void AAuraEffectActor::StartRotation()
{
	bRotates = true;
	ItemRotation = GetActorRotation();
}

void AAuraEffectActor::StartSinMovement()
{
	bSinMovement = true;
	InitialLocation = GetActorLocation();
	ItemLocation = GetActorLocation();
}

void AAuraEffectActor::ApplyEffect(TSubclassOf<UGameplayEffect> GameplayEffectClass, UAbilitySystemComponent* TargetASC)
{
	FGameplayEffectContextHandle Handle = TargetASC->MakeEffectContext();
	Handle.AddSourceObject(this);
	FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, Handle);
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

	const bool bIsInfinite = SpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if (bIsInfinite && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		// Store handle to effect
		ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
	}
}

void AAuraEffectActor::ItemMovement(const float DeltaTime)
{
	if (bRotates)
	{
		const FRotator DeltaRotation(0.f, DeltaTime * RotationRate, 0.f);
		ItemRotation = UKismetMathLibrary::ComposeRotators(ItemRotation, DeltaRotation);
	}

	if (bSinMovement)
	{
		const float Sin = SinAmplitude*FMath::Sin(RunningTime * SinPeriod);
		ItemLocation = InitialLocation + FVector(0.f, 0.f, Sin);
	}
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if(TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;
	
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
	if (bApplied && bDestroyOnEffectApplication)
	{
		Destroy();
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if(TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;

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

	if (bApplied && bDestroyOnEffectApplication)
	{
		Destroy();
	}
	
}





