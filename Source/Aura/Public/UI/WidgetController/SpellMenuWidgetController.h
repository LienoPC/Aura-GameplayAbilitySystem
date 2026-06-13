// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayTags.h"
#include "OverlayWidgetController.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSpellGlobeSelectedSignature, bool, bSpendPointsButtonEnabled, bool, bEquippedButtonEnabled, FString, DescriptionString, FString, NextDescriptionString);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForEquipSelectionSignature, const FGameplayTag&, AbilityType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpellGlobeReassignedSignature, const FGameplayTag&, AbilityTag);
struct FSelectedAbility
{
	FGameplayTag AbilityTag = FGameplayTag();
	FGameplayTag StatusTag = FGameplayTag();
};
/**
 * Widget controller that controls Spell Menu UI
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="GAS|Spell")
	FOnPlayerStatChangedSignature OnSpellPointsChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|Spell")
	FSpellGlobeSelectedSignature OnSpellGlobeSelected;

	UPROPERTY(BlueprintAssignable, Category="GAS|Spell")
	FWaitForEquipSelectionSignature WaitForEquipSelectionDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|Spell")
	FWaitForEquipSelectionSignature StopWaitingForEquipDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|Spell")
	FSpellGlobeReassignedSignature OnSpellGlobeReassigned;
	
	virtual void BroadCastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;


	UFUNCTION(BlueprintCallable, Category="Spell")
	void AbilitySelected(const FGameplayTag& InSelectedAbility);

	UFUNCTION(BlueprintCallable, Category="Spell")
	void AbilityDeselected();
	
	UFUNCTION(BlueprintCallable, Category="Spell")
	void SpendSpellPoint();
	
	UFUNCTION(BlueprintCallable, Category="Spell")
	void EquipAbilityRequested();

	UFUNCTION(BlueprintCallable, Category="Spell")
	void EquipAbility(const FGameplayTag& InputTag, const FGameplayTag& AbilityType);
	
private:
	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& NewInputTag, const FGameplayTag& OldInputTag);
	
	void ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints, bool& bShouldEnableSpellPontsButton, bool& bShouldEnableEquipButton);

	FSelectedAbility SelectedAbility = {FAuraGameplayTags::Get().Abilities_None, FAuraGameplayTags::Get().Abilities_Status_Locked};
	int32 CurrentSpellPoints = 0;
	bool bWaitingForEquip = false;
	FGameplayTag SelectedInputSlot;
};
