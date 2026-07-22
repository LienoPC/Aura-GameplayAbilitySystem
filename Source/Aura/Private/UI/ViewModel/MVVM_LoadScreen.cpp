// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "Game/AuraGameInstance.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadScreen::InitializeLoadSlot()
{
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_0->LoadSlotName = FString("LoadSlot_0");
	LoadSlot_0->SlotIndex = 0;
	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_1->LoadSlotName = FString("LoadSlot_1");
	LoadSlot_1->SlotIndex = 1;
	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_2->LoadSlotName = FString("LoadSlot_2");
	LoadSlot_2->SlotIndex = 2;

	LoadSlots.Add(0, LoadSlot_0);
	LoadSlots.Add(1, LoadSlot_1);
	LoadSlots.Add(2, LoadSlot_2);
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index) const
{
	return LoadSlots.FindChecked(Index);
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 Slot, const FString& SlotName)
{
	if (AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		LoadSlots[Slot]->SetPlayerName(SlotName);
		LoadSlots[Slot]->SetMapName(AuraGameMode->StartingMapName);
		LoadSlots[Slot]->SetPlayerLevel(1);
		LoadSlots[Slot]->SlotStatus = Taken; 
		LoadSlots[Slot]->InitializeSlot();
		LoadSlots[Slot]->PlayerStartTag = AuraGameMode->DefaultPlayerStartTag;
		LoadSlots[Slot]->MapAssetName = AuraGameMode->StartingMap.ToSoftObjectPath().GetAssetName();
	
		AuraGameMode->SaveSlotData(LoadSlots[Slot], Slot);
		UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(UGameplayStatics::GetGameInstance(this));

		AuraGameInstance->LoadSlotName = LoadSlots[Slot]->LoadSlotName;
		AuraGameInstance->LoadSlotIndex = LoadSlots[Slot]->SlotIndex;
		AuraGameInstance->PlayerStartTag = AuraGameMode->DefaultPlayerStartTag;
	}else
	{
		GEngine->AddOnScreenDebugMessage(1, 15.0f, FColor::Magenta, FString("Switch to single player"));
	}
}

void UMVVM_LoadScreen::NewGameButtonPressed(int32 Slot)
{
	LoadSlots[Slot]->SetWidgetSwitcherIndex.Broadcast(1);
}

void UMVVM_LoadScreen::SelectSlotButtonPressed(int32 Slot)
{
	for (const auto& LoadSlot : LoadSlots)
	{
		LoadSlot.Value->EnableSelectSlotButton.Broadcast(LoadSlot.Key != Slot);
	}
	SlotSelected.Broadcast();
	SelectedSlot = Slot;
}

void UMVVM_LoadScreen::DeleteButtonPressed()
{
	if (auto Slot = LoadSlots[SelectedSlot]; IsValid(Slot))
	{
		AAuraGameModeBase::DeleteSaveSlotData(Slot->LoadSlotName, Slot->SlotIndex);
		LoadSlots[SelectedSlot]->SlotStatus = Vacant;
		LoadSlots[SelectedSlot]->InitializeSlot();
		LoadSlots[SelectedSlot]->EnableSelectSlotButton.Broadcast(true);
	}
}

void UMVVM_LoadScreen::PlayButtonPressed()
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(UGameplayStatics::GetGameInstance(this));
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (!AuraGameMode)
		return;

	if (!IsValid(LoadSlots[SelectedSlot]))
		return;
	AuraGameInstance->PlayerStartTag = LoadSlots[SelectedSlot]->PlayerStartTag;	
	AuraGameInstance->LoadSlotName = LoadSlots[SelectedSlot]->LoadSlotName;
	AuraGameInstance->LoadSlotIndex = LoadSlots[SelectedSlot]->SlotIndex;

	AuraGameMode->TravelToMap(LoadSlots[SelectedSlot]);
}

void UMVVM_LoadScreen::LoadData()
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (!IsValid(AuraGameMode))
		return;
	for (const auto& Slot : LoadSlots)
	{
		UAuraSaveGame* SaveObject = AuraGameMode->GetSaveSlotData(Slot.Value->LoadSlotName, Slot.Key);

		if (SaveObject == nullptr)
			continue;
		Slot.Value->SetPlayerName(SaveObject->PlayerName);
		Slot.Value->SlotStatus = SaveObject->SlotStatus;
		Slot.Value->SetMapName(SaveObject->MapName);
		Slot.Value->SetPlayerLevel(SaveObject->PlayerLevel);
		Slot.Value->PlayerStartTag = SaveObject->PlayerStartTag;
		Slot.Value->InitializeSlot();
		
	}
}
