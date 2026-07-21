// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AuraGameModeBase.h"

#include "Aura/AuraLogChannels.h"
#include "Game/AuraGameInstance.h"
#include "Game/AuraSaveGame.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/SaveInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void AAuraGameModeBase::SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(LoadSlot->LoadSlotName, SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(LoadSlot->LoadSlotName, SlotIndex);
	}
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	UAuraSaveGame* LoadScreenSaveGame = Cast<UAuraSaveGame>(SaveGameObject);


	LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayerName();
	LoadScreenSaveGame->SlotIndex = SlotIndex;
	LoadScreenSaveGame->SlotName = LoadSlot->LoadSlotName;
	LoadScreenSaveGame->SlotStatus = Taken;
	LoadScreenSaveGame->MapName = LoadSlot->GetMapName();
	LoadScreenSaveGame->PlayerStartTag = LoadSlot->PlayerStartTag;
		
	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot->LoadSlotName, SlotIndex);
}

UAuraSaveGame* AAuraGameModeBase::GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const
{
	USaveGame* SaveGameObject = nullptr;
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		SaveGameObject = UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);
	}else
	{
		SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	}
	return Cast<UAuraSaveGame>(SaveGameObject);
}

void AAuraGameModeBase::TravelToMap(UMVVM_LoadSlot* LoadSlot)
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(LoadSlot, Maps.FindChecked(LoadSlot->GetMapName()));	
}

void AAuraGameModeBase::DeleteSaveSlotData(const FString& SlotName, int32 SlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
	}
}

UAuraSaveGame* AAuraGameModeBase::RetreiveInGameSaveData()
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(UGameplayStatics::GetGameInstance(this));

	if (AuraGameInstance)
	{
		const FString InGameLoadSlotName = AuraGameInstance->LoadSlotName;
		const int32 InGameLoadSlotIndex = AuraGameInstance->LoadSlotIndex;

		return GetSaveSlotData(InGameLoadSlotName, InGameLoadSlotIndex);
	}
	return nullptr;
}

void AAuraGameModeBase::SaveInGameProgressData(UAuraSaveGame* SaveObject)
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(UGameplayStatics::GetGameInstance(this));

	if (AuraGameInstance)
	{
		const FString InGameLoadSlotName = AuraGameInstance->LoadSlotName;
		const int32 InGameLoadSlotIndex = AuraGameInstance->LoadSlotIndex;

		AuraGameInstance->PlayerStartTag = SaveObject->PlayerStartTag;
		UGameplayStatics::SaveGameToSlot(SaveObject, InGameLoadSlotName, InGameLoadSlotIndex);
	}
}

AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(UGameplayStatics::GetGameInstance(this));

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), OutActors);
	if (OutActors.Num() > 0)
	{
		AActor* SelectedActor = OutActors[0];

		for (auto Actor : OutActors)
		{
			if(APlayerStart* PlayerStart = Cast<APlayerStart>(Actor)){
				if (PlayerStart->PlayerStartTag == AuraGameInstance->PlayerStartTag)
				{
					SelectedActor = PlayerStart;
					break;
				}
			}
			
		}
		return SelectedActor;
	}
	return nullptr; 
}

void AAuraGameModeBase::SaveWorldState(UWorld* World, const FString& DestinationMapAssetName) const
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	UAuraGameInstance* GameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	check(GameInstance);

	if (UAuraSaveGame* SaveGame = GetSaveSlotData(GameInstance->LoadSlotName, GameInstance->LoadSlotIndex))
	{
		if (!DestinationMapAssetName.IsEmpty())
		{
			SaveGame->MapAssetName = DestinationMapAssetName;
			SaveGame->MapName = GetMapNameFromMapAssetName(DestinationMapAssetName);
		}
		if (!SaveGame->HasMap(WorldName))
		{
			FSavedMap NewSavedMap = FSavedMap();
			NewSavedMap.MapAssetName = WorldName;
			SaveGame->SavedMaps.Add(NewSavedMap);
		}

		FSavedMap SavedMap = SaveGame->GetSavedMapWithMapName(WorldName);
		SavedMap.SavedActors.Empty();
		
		TArray<AActor*> ToSaveActors;
		UGameplayStatics::GetAllActorsWithInterface(this, USaveInterface::StaticClass(), ToSaveActors);

		for (auto Actor : ToSaveActors)
		{
			FSavedActor SavedActor;
			SavedActor.ActorName = Actor->GetFName();
			SavedActor.ActorTransform = Actor->GetActorTransform();

			FMemoryWriter MemoryWriter(SavedActor.Bytes);
			FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, true);
			Archive.ArIsSaveGame = true;
			Actor->Serialize(Archive);
			
			SavedMap.SavedActors.Add(SavedActor);
		}


		for (FSavedMap& MapToReplace : SaveGame->SavedMaps)
		{
			if (MapToReplace.MapAssetName == WorldName)
			{
				MapToReplace = SavedMap;
			}
		}
		UGameplayStatics::SaveGameToSlot(SaveGame, GameInstance->LoadSlotName, GameInstance->LoadSlotIndex);
	}
}

void AAuraGameModeBase::LoadWorldState(UWorld* World) const
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	UAuraGameInstance* GameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	check(GameInstance);

	if (UGameplayStatics::DoesSaveGameExist(GameInstance->LoadSlotName, GameInstance->LoadSlotIndex))
	{
		UAuraSaveGame* SaveGame = Cast<UAuraSaveGame>(UGameplayStatics::LoadGameFromSlot(GameInstance->LoadSlotName, GameInstance->LoadSlotIndex));
		if (SaveGame == nullptr)
		{
			UE_LOG(LogAura, Error, TEXT("Failed to load slot"));
		}


		TArray<AActor*> ToLoadActors;
		UGameplayStatics::GetAllActorsWithInterface(this, USaveInterface::StaticClass(), ToLoadActors);

		FSavedActor ToFind;
		TSet<FSavedActor> SavedActors = TSet<FSavedActor>(SaveGame->GetSavedMapWithMapName(WorldName).SavedActors);
		for (auto Actor : ToLoadActors)
		{
			ToFind.ActorName = Actor->GetFName();
			FSavedActor* SavedActor = SavedActors.Find(ToFind);
			if (SavedActor)
			{
				if (ISaveInterface::Execute_ShouldLoadTransform(Actor))
				{
					Actor->SetActorTransform(SavedActor->ActorTransform);
				}

				FMemoryReader MemoryReader(SavedActor->Bytes);

				FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
				Archive.ArIsSaveGame = true;
				Actor->Serialize(Archive);

				ISaveInterface::Execute_LoadActor(Actor);
			}
		}
	}
}

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	Maps.Add(StartingMapName, StartingMap);
}

FString AAuraGameModeBase::GetMapNameFromMapAssetName(const FString& MapAssetName) const
{
	for (auto &Map : Maps)
	{
		if (Map.Value.ToSoftObjectPath().GetAssetName() == MapAssetName)
		{
			return Map.Key;
		}
	}
	return FString();
}
