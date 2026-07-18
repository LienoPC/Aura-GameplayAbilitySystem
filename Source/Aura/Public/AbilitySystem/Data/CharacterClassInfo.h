// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScalableFloat.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

class UGameplayAbility;
class UGameplayEffect;

UENUM(BlueprintType)
enum class ECharacterClass: uint8
{
	Elementalist,
	Warrior,
	Ranged,
};

USTRUCT()
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category="Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;

	UPROPERTY(EditDefaultsOnly, Category="Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
	
	UPROPERTY(EditDefaultsOnly, Category="Class Defaults")
	FScalableFloat GrantedXP = FScalableFloat();
	
};

/**
 * 
 */
UCLASS()
class AURA_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()

public:

	UFUNCTION()
	FCharacterClassDefaultInfo GetCharacterClassDefaultInfo(ECharacterClass CharacterClass) const;
	
	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;

	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes_Infinite;
	
	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes_SetByCaller;
	
	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;

	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> CommonAbilities;

	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults|Damage")
	TObjectPtr<UCurveTable> DamageCalculationCoefficients;


protected:

	UPROPERTY(EditDefaultsOnly, Category="Class Defaults")
	TMap<ECharacterClass, FCharacterClassDefaultInfo> CharacterClassInformation;

};
