// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/LevelUpInfo.h"

#include "WorldPartition/Cook/WorldPartitionCookPackage.h"

ULevelUpInfo::ULevelUpInfo()
{
	/*
	Levels.KeySort([](int32* a, int32* b)
	{
		return *a < *b;
	});
	*/
}

void ULevelUpInfo::PostLoad()
{
	Super::PostLoad();
	Levels.Sort([](FLevelInfo A, FLevelInfo B)
{
	return A.Level  < B.Level;
});
}

int32 ULevelUpInfo::LevelAtXP(int32 XP) const
{
	for (int i = Levels.Num() - 1; i >= 0; i--)
	{
		if (XP > Levels[i].LevelUpRequirement)
		{
			return Levels[i].Level;
		}
	}
	return 1;
}

void ULevelUpInfo::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName ChangedPropertyName = PropertyChangedEvent.GetPropertyName();

	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(ULevelUpInfo, Levels))
	{
		if (PropertyChangedEvent.ChangeType == EPropertyChangeType::ArrayAdd)
		{
			if (Levels.Num() > 0)
			{
				Levels.Last().Level = Levels.Num(); 
			}
		}
	}
	Levels.Sort([](FLevelInfo A, FLevelInfo B)
	{
		return A.Level  < B.Level;
	});
}
