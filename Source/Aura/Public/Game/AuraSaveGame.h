// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/SaveGame.h"
#include "AuraSaveGame.generated.h"

class UGameplayAbility;

UENUM(BlueprintType)
enum ESaveSlotStatus
{
	Vacant,
	EnterName,
	Taken
};

USTRUCT(BlueprintType)
struct FSavedAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Class Defaults")
	TSubclassOf<UGameplayAbility> GameplayAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Class Defaults")
	FGameplayTag AbilityTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Class Defaults")
	FGameplayTag AbilityStatus = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Class Defaults")
	FGameplayTag AbilitySlot = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Class Defaults")
	FGameplayTag AbilityType = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Class Defaults")
	int32 AbilityLevel;

};


/**
 * 
 */
UCLASS()
class AURA_API UAuraSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FString SlotName = FString();

	UPROPERTY()
	int32 SlotIndex = 0;

	UPROPERTY()
	FString PlayerName = FString();

	UPROPERTY()
	FString MapName = FString();

	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SlotStatus = ESaveSlotStatus::Vacant;

	UPROPERTY()
	FName PlayerStartTag;

	/** Player **/

	UPROPERTY()
	int32 PlayerLevel = 1;

	UPROPERTY()
	int32 XP = 0;

	UPROPERTY()
	int32 AttributePoints = 0;

	UPROPERTY()
	int32 SpellPoints = 0;
	
	UPROPERTY()
	float Strength = 0.0f;
	UPROPERTY()
	float Intelligence = 0.0f;
	UPROPERTY()
	float Resilience = 0.0f;
	UPROPERTY()
	float Vigor = 0.0f;

	UPROPERTY()
	bool bFirstTimeLoadIn = true;

	/** Abilities */

	UPROPERTY()
	TArray<FSavedAbility> SavedAbilities;
};
