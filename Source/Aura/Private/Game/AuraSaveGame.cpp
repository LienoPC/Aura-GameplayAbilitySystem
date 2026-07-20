// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AuraSaveGame.h"

FSavedMap UAuraSaveGame::GetSavedMapWithMapName(const FString& InMapName)
{
	if (auto FoundMap = SavedMaps.FindByPredicate([InMapName](const FSavedMap& Map)
	{
		return Map.MapAssetName == InMapName;
	}))
	{
		return *FoundMap;
	}
	return FSavedMap();
}

bool UAuraSaveGame::HasMap(const FString& InMapName)
{
	if (auto FoundMap = SavedMaps.FindByPredicate([InMapName](const FSavedMap& Map)
	{
	return Map.MapAssetName == InMapName;
	}))
	{
		return true;
	}
	return false;
}
