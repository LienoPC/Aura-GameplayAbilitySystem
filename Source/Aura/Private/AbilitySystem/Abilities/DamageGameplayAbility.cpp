// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/DamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "Interaction/CombatInterface.h"

void UDamageGameplayAbility::CauseDamage(AActor* Target)
{
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
	DamageSpecHandle.Data->GetContext().AddSourceObject(GetAvatarActorFromActorInfo());

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, DamageType, Damage.GetValueAtLevel(GetAbilityLevel()));
	

	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target))
	{
		GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data, TargetASC);

	}
}

FDamageEffectParams UDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor,
	FVector InRadialDamageOrigin) const
{
	FDamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.DamageGameplayEffectClass = DamageEffectClass;
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	if (TargetActor)
	{
		Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	}
	Params.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	Params.AbilityLevel = GetAbilityLevel();
	Params.DamageType = DamageType;
	Params.DebuffChance = DebuffChance;
	Params.DebuffDamage = DebuffDamage;
	Params.DebuffDuration = DebuffDuration;
	Params.DebuffFrequency = DebuffFrequency;
	Params.DeathImpulseMagnitude = DeathImpulseMagnitude;
	Params.KnockbackMagnitude = KnockbackMagnitude;
	Params.KnockbackChance = KnockbackChance;

	if (IsValid(TargetActor))
	{
		const FVector ToTarget = InRadialDamageOrigin != FVector::ZeroVector ? (TargetActor->GetActorLocation() - InRadialDamageOrigin).GetSafeNormal() : (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).GetSafeNormal();
		Params.DeathImpulse = ToTarget * DeathImpulseMagnitude;
		Params.KnockbackForce = ToTarget * KnockbackMagnitude;
	}

	if (bIsRadialDamage)
	{
		Params.bIsRadialDamage = bIsRadialDamage;
		Params.RadialDamageInnerRadius = RadialDamageInnerRadius;
		Params.RadialDamageOuterRadius = RadialDamageOuterRadius;
		Params.RadialDamageOrigin = InRadialDamageOrigin;
	}
	return Params;
}


float UDamageGameplayAbility::GetDamageAtLevel() const
{
	return Damage.GetValueAtLevel(GetAbilityLevel());
}

FTaggedMontage UDamageGameplayAbility::GetRandomDamageMontage(const TArray<FTaggedMontage>& Montages)
{

	if (Montages.IsEmpty())
	{
		return FTaggedMontage();
	}

	const int32 RandomIndex = FMath::RandRange(0, Montages.Num() - 1);
	return Montages[RandomIndex];
}
