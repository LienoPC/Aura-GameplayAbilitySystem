// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/AbilityInfo.h"

#include "Aura/AuraLogChannels.h"

FAuraAbilityInfo UAbilityInfo::FindAbilityInfoByTag(const FGameplayTag& Tag, bool bLog) const
{
	for (const FAuraAbilityInfo& Info : AbilityInformation)
	{
		if (Info.AbilityTag.MatchesTagExact(Tag))
		{
			return Info;
		}
	}

	if (bLog)
	{
		UE_LOG(LogAura, Error, TEXT("Ability Tag not found"));
	}
	return FAuraAbilityInfo();
}
