// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/EC_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"

// Local struct that works with static variables to contains CaptureDefinition variables.
struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);

	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);

	
	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Source, false);


	}	
};

static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DStatics;
	return DStatics;
}

UEC_Damage::UEC_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	
	RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
}

void UEC_Damage::ComputeDebuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FGameplayEffectSpec& Spec, FAggregatorEvaluateParameters EvaluationParameters, const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& InTagsToDef) const
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	for (auto Pair : GameplayTags.DamageTypesToDebuffs)
	{
		const FGameplayTag& DamageType = Pair.Key;
		const FGameplayTag& DebuffType = Pair.Value;
		const float TypeDamage = Spec.GetSetByCallerMagnitude(DamageType, false, -1.f);
		if (TypeDamage > 0.f)
		{
			// Determine a successful debuff
			const float SourceDebuffChance = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Chance, false, -1.f);
			float TargetDebuffResistance = 0.f;
			const FGameplayTag& DebuffResistanceType = GameplayTags.DamageTypesToResistances[DamageType];
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(InTagsToDef[DebuffResistanceType], EvaluationParameters, TargetDebuffResistance);

			TargetDebuffResistance = FMath::Max(TargetDebuffResistance, 0.f);

			const float EffectiveDebuffChance = SourceDebuffChance * (100 - TargetDebuffResistance) / 100.f;

			const bool bDebuff = EffectiveDebuffChance >  FMath::RandRange(1, 100);

			if (bDebuff)
			{
				// TODO: Gay?
				FGameplayEffectContextHandle ContextHandle = Spec.GetContext();

				UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(ContextHandle, true);
				const float DebuffDamage = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Damage, false, -1.f);
				if (DebuffDamage < 0.f)
					return;
				const float DebuffDuration = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Duration, false, -1.f);
				const float DebuffFrequency = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Frequency, false, -1.f);
				UAuraAbilitySystemLibrary::SetDebuffDamage(ContextHandle, DebuffDamage);
				UAuraAbilitySystemLibrary::SetDebuffDuration(ContextHandle, DebuffDuration);
				UAuraAbilitySystemLibrary::SetDebuffFrequency(ContextHandle, DebuffFrequency);
				UAuraAbilitySystemLibrary::SetDebuffDamageType(ContextHandle, DamageType);
			}
		}
	}
}


void UEC_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                        FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_Armor, DamageStatics().ArmorDef);
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_ArmorPenetration, DamageStatics().ArmorPenetrationDef);
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_BlockChance, DamageStatics().BlockChanceDef);
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_CriticalHitChance, DamageStatics().CriticalHitChanceDef);
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance, DamageStatics().CriticalHitResistanceDef);
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_CriticalHitDamage, DamageStatics().CriticalHitDamageDef);

	TagsToCaptureDefs.Add(GameplayTags.Attributes_Resistance_Arcane, DamageStatics().ArcaneResistanceDef);
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Resistance_Fire, DamageStatics().FireResistanceDef);
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Resistance_Lightning, DamageStatics().LightningResistanceDef);
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Resistance_Physical, DamageStatics().PhysicalResistanceDef);

	
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	TScriptInterface<ICombatInterface> SourceAvatarInterface = SourceAvatar;
	TScriptInterface<ICombatInterface> TargetAvatarInterface = TargetAvatar;

	int32 SourceAvatarLevel = 1;
	int32 TargetAvatarLevel = 1;

	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();

	if (SourceAvatar->Implements<UCombatInterface>())
	{
		SourceAvatarLevel = ICombatInterface::Execute_GetPlayerLevel(SourceAvatar);
	}

	if (TargetAvatar->Implements<UCombatInterface>())
	{
		TargetAvatarLevel = ICombatInterface::Execute_GetPlayerLevel(TargetAvatar);
	}
	
	FAggregatorEvaluateParameters EvaluationParameters;

	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// Debuffs
	ComputeDebuff(ExecutionParams, Spec, EvaluationParameters, TagsToCaptureDefs);
	
	
	// Get Damage SetByCaller Magnitude
	float Damage = 0.f;
	for (auto &Pair : FAuraGameplayTags::Get().DamageTypesToResistances)
	{
		// Get Damage Resistance from Target associated with tag
		const FGameplayTag DamageTypeTag = Pair.Key;
		const FGameplayTag ResistanceTag = Pair.Value;
		checkf(TagsToCaptureDefs.Contains(ResistanceTag), TEXT("DamageTypesToResistances are invalid"));
		
		
		const FGameplayEffectAttributeCaptureDefinition CaptureDef = TagsToCaptureDefs[ResistanceTag];

		float Resistance = 0.f;

		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluationParameters, Resistance);
		Resistance = FMath::Clamp(Resistance, 0.f, 100.f);

		// Reduce damage using resistances
		float DamageTypeValue = Spec.GetSetByCallerMagnitude(Pair.Key, false);
		if (DamageTypeValue <= 0.f)
			continue;

		DamageTypeValue *= (100.f-Resistance)/100.f;

		if (UAuraAbilitySystemLibrary::IsRadialDamage(EffectContextHandle))
		{
			if (TScriptInterface<ICombatInterface> CombatInterface = TargetAvatar)
			{
				CombatInterface->GetOnDamageTakenDelegate().AddLambda([&DamageTypeValue] (float InDamage)
				{
					DamageTypeValue = InDamage;
				});
			}
			UGameplayStatics::ApplyRadialDamageWithFalloff(TargetAvatar,
				DamageTypeValue,
				0.f,
				UAuraAbilitySystemLibrary::GetRadialDamageOrigin(EffectContextHandle),
				UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(EffectContextHandle),
				UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(EffectContextHandle),
				1.0f,
				UDamageType::StaticClass(),
				TArray<AActor*>(),
				SourceAvatar);
		}
		Damage += DamageTypeValue;
	}
	
	// Capture BlockChance on Target, and determine if it was a successiful block
	float BlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, BlockChance);
	BlockChance = FMath::Max(BlockChance, 0.f);


	// Capture TargetArmor on Target, and determine if it was a successiful block
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
	TargetArmor = FMath::Max(TargetArmor, 0.f);

	// Capture ArmorPenetration on Source, and determine if it was a successiful block
	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max(SourceArmorPenetration, 0.f);

	
	// If block, Halve the damage
	const bool bBlock = FMath::RandRange(1, 100) < BlockChance;
	Damage = bBlock ? Damage * 0.5f : Damage;
	// Add the bBlock to the Context
	UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlock);

	// Armor penetration ignores a percentage of the target armor
	const float ArmorPenetrationCoefficient = GetCurveValue(SourceAvatar, FName("ArmorPenetration"), SourceAvatarLevel);
	const float EffectiveArmor = TargetArmor * (100 - SourceArmorPenetration * ArmorPenetrationCoefficient)/100.f;
	// Armor ignores a percentage of incoming damage
	const float EffectiveArmorCoefficient = GetCurveValue(TargetAvatar, FName("EffectiveArmor"), TargetAvatarLevel);
	Damage *= (100.f - EffectiveArmor * EffectiveArmorCoefficient)/100.f;


	// Critical Hit Calculation
	float SourceCriticalChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCriticalChance);
	SourceCriticalChance = FMath::Max(SourceCriticalChance, 0.f);
	
	float TargetCriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluationParameters, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max(TargetCriticalHitResistance, 0.f);
	
	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max(SourceCriticalHitDamage, 0.f);

	float EffectiveCriticalHitChance = SourceCriticalChance - TargetCriticalHitResistance * 0.15f;

	const bool bCriticalHit = FMath::RandRange(1, 100) < EffectiveCriticalHitChance;

	Damage = bCriticalHit ? 2.f * Damage + SourceCriticalChance : Damage;

	UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);	
	
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}

float UEC_Damage::GetCurveValue(AActor* Avatar, FName CurveName, float Level) const
{
	UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(Avatar);

	FRealCurve* Curve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(CurveName, FString());

	float Coefficient = Curve->Eval(Level);

	return Coefficient;
}
