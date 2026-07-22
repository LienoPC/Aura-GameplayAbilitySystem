// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/LootTiers.h"

TArray<FLootItem> ULootTiers::GetLootItems()
{
	TArray<FLootItem> OutItems;

	for (FLootItem& Item : LootItems)
	{
		for (int32 i = 0; i < Item.MaxNumberToSpawn; ++i)
		{
			if (FMath::RandRange(1, 100) < Item.ChanceToSpawn)
			{
				FLootItem NewItem = Item;

				OutItems.Add(NewItem);
			}
		}
	}
	
	return OutItems;
}
