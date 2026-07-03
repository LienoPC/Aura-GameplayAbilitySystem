// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraFireball.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

void AAuraFireball::BeginPlay()
{
	Super::BeginPlay();
	StartOutgoingTimeline();
}

void AAuraFireball::OnOverlap(AActor* Other)
{
	if (!IsValidOverlap(Other))
		return;

	if(HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAuraAbilitySystemLibrary::GetAbilitySystemComponent(Other))
		{
			DamageEffectParams.DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
	}
}

void AAuraFireball::OnEndOverlap(AActor* Other)
{
}
