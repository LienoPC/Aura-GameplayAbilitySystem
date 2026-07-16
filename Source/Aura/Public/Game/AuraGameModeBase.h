// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class UAuraSaveGame;
class USaveGame;
class UMVVM_LoadSlot;
class UCharacterClassInfo;
class UAbilityInfo;
/**
 * 
 */
UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	void SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex);

	UAuraSaveGame* GetSaveSlotData(const FString& SlotName, int32 SlotIndex);

	void TravelToMap(UMVVM_LoadSlot* LoadSlot);
	
	static void DeleteSaveSlotData(const FString& SlotName, int32 SlotIndex);

	UAuraSaveGame* RetreiveInGameSaveData();
	void SaveInGameProgressData(UAuraSaveGame* SaveObject);
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	
	
	UPROPERTY(EditDefaultsOnly, Category="Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly, Category="Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;


	UPROPERTY(EditDefaultsOnly)
	FString StartingMapName;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> StartingMap;
	
	UPROPERTY(EditDefaultsOnly)
	TMap<FString, TSoftObjectPtr<UWorld>> Maps;

	UPROPERTY(EditDefaultsOnly)
	FName DefaultPlayerStartTag;
	
protected:
	virtual void BeginPlay() override;


	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USaveGame> LoadScreenSaveGameClass;
};
