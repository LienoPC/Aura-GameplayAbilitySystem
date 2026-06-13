// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadCastInitialValues()
{
	BroadcastAbilityInfo();
	if (AAuraPlayerState* PS = Cast<AAuraPlayerState>(PlayerState))
	{
		OnSpellPointsChangedDelegate.Broadcast(PS->GetSpellPoints());
	}
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	if (UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		ASC->AbilityStatusChangedDelegate.AddLambda([this, ASC](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const int32 Level)
		{
			if (SelectedAbility.AbilityTag.MatchesTagExact(AbilityTag))
			{
				SelectedAbility.StatusTag = StatusTag;
				bool bEnableSpendPoints = false;
				bool bEnableEquip = false;

				ShouldEnableButtons(StatusTag, CurrentSpellPoints, bEnableSpendPoints, bEnableEquip);

				FString Description;
				FString NextLevelDescription;
				if (ASC)
					ASC->GetDescriptionsByAbilityTag(SelectedAbility.AbilityTag, Description, NextLevelDescription);
				
				OnSpellGlobeSelected.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
			
			}
			if (AbilityInfo)
			{
				FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
				Info.StatusTag = StatusTag;
				AbilityInfoDelegate.Broadcast(Info);
			}
		});

		ASC->AbilityEquipped.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);
	}

	if (AAuraPlayerState* PS = Cast<AAuraPlayerState>(PlayerState))
	{
		PS->OnSpellPointsChanged.AddLambda([this](const int32 NewPoints)
		{
			OnSpellPointsChangedDelegate.Broadcast(NewPoints);
			CurrentSpellPoints = NewPoints;

			bool bEnableSpendPoints = false;
			bool bEnableEquip = false;
			FString Description;
			FString NextLevelDescription;
			if (UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
				ASC->GetDescriptionsByAbilityTag(SelectedAbility.AbilityTag, Description, NextLevelDescription);
	
			ShouldEnableButtons(SelectedAbility.StatusTag, CurrentSpellPoints, bEnableSpendPoints, bEnableEquip);
			OnSpellGlobeSelected.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
			
		});
	}
}

void USpellMenuWidgetController::SpendSpellPoint()
{
	if (UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		ASC->ServerSpendSpellPoint(SelectedAbility.AbilityTag);
	}
}

void USpellMenuWidgetController::EquipAbilityRequested()
{
	const FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.AbilityTag).AbilityType;

	WaitForEquipSelectionDelegate.Broadcast(AbilityType);
	bWaitingForEquip = true;

	if (UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		const FGameplayTag& StatusTag = ASC->GetStatusFromAbilityTag(SelectedAbility.AbilityTag);
		if (StatusTag == FAuraGameplayTags::Get().Abilities_Status_Equipped)
		{
			SelectedInputSlot = ASC->GetInputTagFromAbilityTag(SelectedAbility.AbilityTag);
		}
	}
	
}

void USpellMenuWidgetController::EquipAbility(const FGameplayTag& InputTag, const FGameplayTag& AbilityType)
{
	if (!bWaitingForEquip)
		return;
	const FGameplayTag& SelectedAbilityType = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.AbilityTag).AbilityType;
	if (!SelectedAbilityType.MatchesTagExact(AbilityType))
		return;

	if (UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		ASC->ServerEquipAbility(SelectedAbility.AbilityTag, InputTag);
	}
	
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status,
	const FGameplayTag& NewInputTag, const FGameplayTag& OldInputTag)
{
	bWaitingForEquip = false;
	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = FAuraGameplayTags::Get().Abilities_Status_Unlocked;
	LastSlotInfo.InputTag = OldInputTag;
	LastSlotInfo.AbilityTag = FAuraGameplayTags::Get().Abilities_None;

	// Broadcast empty info if previous slot is a valid slot
	AbilityInfoDelegate.Broadcast(LastSlotInfo);


	FAuraAbilityInfo NewSlotInfo = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
	NewSlotInfo.StatusTag = Status;
	NewSlotInfo.InputTag = NewInputTag;
	// Broadcast the effective equipped ability
	AbilityInfoDelegate.Broadcast(NewSlotInfo);

	StopWaitingForEquipDelegate.Broadcast(AbilityInfo->FindAbilityInfoByTag(AbilityTag).AbilityType);
	OnSpellGlobeReassigned.Broadcast(AbilityTag);
	AbilityDeselected();
}


void USpellMenuWidgetController::AbilitySelected(const FGameplayTag& InSelectedAbility)
{
	if (bWaitingForEquip)
	{
		FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoByTag(InSelectedAbility).AbilityType;
		StopWaitingForEquipDelegate.Broadcast(AbilityType);
		bWaitingForEquip = false;
	}
	
	// Get ability info
	FGameplayTag AbilityStatus;
	
	AAuraPlayerState* PS = CastChecked<AAuraPlayerState>(PlayerState);
	const int32 SpellPoints = PS->GetSpellPoints();

	const bool bTagValid = InSelectedAbility.IsValid();
	const bool bTagNone = InSelectedAbility.MatchesTagExact(FAuraGameplayTags::Get().Abilities_None);
	if (UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		FGameplayAbilitySpec* AbilitySpec = ASC->GetSpecFromAbilityTag(InSelectedAbility);

		const bool bSpecValid = AbilitySpec != nullptr;

		if (!bTagValid || bTagNone || !bSpecValid)
		{
			AbilityStatus = FAuraGameplayTags::Get().Abilities_Status_Locked;
		}else
		{
			AbilityStatus = ASC->GetAbilityStatusFromSpec(*AbilitySpec);
		}
		SelectedAbility.AbilityTag = InSelectedAbility;
		SelectedAbility.StatusTag = AbilityStatus;
		bool bEnableSpendPoints = false;
		bool bEnableEquip = false;

		FString Description;
		FString NextLevelDescription;
		ASC->GetDescriptionsByAbilityTag(SelectedAbility.AbilityTag, Description, NextLevelDescription);
	
		ShouldEnableButtons(AbilityStatus, SpellPoints, bEnableSpendPoints, bEnableEquip);
		OnSpellGlobeSelected.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
	}
}

void USpellMenuWidgetController::AbilityDeselected()
{
	if (bWaitingForEquip)
	{
		FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.AbilityTag).AbilityType;
		StopWaitingForEquipDelegate.Broadcast(AbilityType);
		bWaitingForEquip = false;
	}
	
	SelectedAbility.AbilityTag = FAuraGameplayTags::Get().Abilities_None;
	SelectedAbility.StatusTag = FAuraGameplayTags::Get().Abilities_Status_Locked;

	OnSpellGlobeSelected.Broadcast(false, false, FString(), FString());
}

void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints,
                                                     bool& bShouldEnableSpellPontsButton, bool& bShouldEnableEquipButton)
{
	bShouldEnableSpellPontsButton = false;

	if (AbilityStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped))
	{
		bShouldEnableEquipButton = true;
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPontsButton = true;
		}
	}else if (AbilityStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Eligible))
	{
		bShouldEnableEquipButton = false;
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPontsButton = true;
		}
	}else if (AbilityStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Unlocked))
	{
		bShouldEnableEquipButton = true;
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPontsButton = true;
		}
	}else if (AbilityStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Locked))
	{
		bShouldEnableEquipButton = false;
	}
}
