// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AuraInputConfig.generated.h"

// Wrapper that contains an InputAction associated with a GameplayTag
USTRUCT(BlueprintType)
struct FAuraInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	const class UInputAction* InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag = FGameplayTag();
};

// Data asset that contains arrays of input mapping. Elements within the array are custom struct
// for InputAction and GameplayTag mapping.
UCLASS()
class AURA_API UAuraInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	// Return InputAction based on a GameplayTag
	const UInputAction* GetInputActionFromTag(const FGameplayTag &InputTag, bool bLogNotFound = false) const;
	
	// Array of InputActions associated with a tag
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FAuraInputAction> AbilityInputActions;
};
