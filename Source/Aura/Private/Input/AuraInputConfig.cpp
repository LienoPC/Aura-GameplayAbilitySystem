// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::GetInputActionFromTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const auto Action : AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.MatchesTagExact(InputTag))
		{
			return Action.InputAction;
		}
	}
	if (bLogNotFound)
		UE_LOG(LogTemp, Error, TEXT("No input action found for InputTag [%s], on Input Config [%s]"), *InputTag.ToString(), *GetName());
		
	return nullptr;
}
