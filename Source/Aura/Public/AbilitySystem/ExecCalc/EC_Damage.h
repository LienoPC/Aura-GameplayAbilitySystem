// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "EC_Damage.generated.h"



/**
 * 
 */
UCLASS()
class AURA_API UEC_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UEC_Damage();
	void ComputeDebuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FGameplayEffectSpec& Spec,
	                   FAggregatorEvaluateParameters EvaluationParameters,const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& InTagsToDef) const;

	void ComputeKnockback(const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FGameplayEffectSpec& Spec,
					   FAggregatorEvaluateParameters EvaluationParameters,const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& InTagsToDef) const;

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

private:
	float GetCurveValue(AActor* Avatar, FName CurveName, float Level) const;
};
