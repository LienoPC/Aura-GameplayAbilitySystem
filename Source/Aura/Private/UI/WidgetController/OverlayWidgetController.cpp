// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/AuraPlayerState.h"

void UOverlayWidgetController::BroadCastInitialValues()
{
	if(UAuraAttributeSet* AuraAttributes = CastChecked<UAuraAttributeSet>(AttributeSet))
	{
		OnHealthChanged.Broadcast(AuraAttributes->GetHealth());
		OnMaxHealthChanged.Broadcast(AuraAttributes->GetMaxHealth());

		OnManaChanged.Broadcast(AuraAttributes->GetMana());
		OnMaxManaChanged.Broadcast(AuraAttributes->GetMaxMana());
	}
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	if(UAuraAttributeSet* AuraAttributes = CastChecked<UAuraAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributes->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			});
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributes->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			});
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributes->GetManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnManaChanged.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributes->GetMaxManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxManaChanged.Broadcast(Data.NewValue);
			}
			);

		Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
			[this](const FGameplayTagContainer& AssetTags)
			{
				for (const FGameplayTag& Tag : AssetTags)
				{
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					
					if (Tag.MatchesTag(MessageTag))
					{
						const FString Msg = FString::Printf(TEXT("GE Tag: %s"), *Tag.ToString());
						GEngine->AddOnScreenDebugMessage(-1,8.f, FColor::Blue, Msg);

						FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
						MessageWidgetRowDelegate.Broadcast(*Row);
					}
				}
			});

		Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
		Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityEquipped.AddUObject(this, &UOverlayWidgetController::OnAbilityEquipped);
	}

	if (AAuraPlayerState* PS = CastChecked<AAuraPlayerState>(PlayerState))
	{
		PS->OnExperienceChanged.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
		PS->OnLevelChanged.AddUObject(this, &UOverlayWidgetController::OnLevelChanged);
	}

}


void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& NewInputTag, const FGameplayTag& OldInputTag) const
{
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
	
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
	AAuraPlayerState* AS = Cast<AAuraPlayerState>(PlayerState);

	if (!AS)
		return;

	const ULevelUpInfo* LevelUpInfo = AS->LevelsInfo;

	check(LevelUpInfo);

	const int32 Level = LevelUpInfo->LevelAtXP(NewXP);

	const int32 MaxLevel = LevelUpInfo->Levels.Num();

	if (Level < MaxLevel && Level > 0)
	{
		const int32 LevelUpRequirement = LevelUpInfo->Levels[Level].LevelUpRequirement;
		const int32 PreviousLevelRequirement = LevelUpInfo->Levels[Level - 1].LevelUpRequirement;

		const int32 DeltaLevelRequirement = LevelUpRequirement - PreviousLevelRequirement;

		const int32 XPForThisLevel = NewXP - PreviousLevelRequirement;

		const float PercentageXP = static_cast<float>(XPForThisLevel) / static_cast<float>(DeltaLevelRequirement);
		OnXPChangedDelegate.Broadcast(PercentageXP);
	}
}

void UOverlayWidgetController::OnLevelChanged(int32 NewLevel, bool bLevelUp)
{
	OnPlayerLevelChangedDelegate.Broadcast(NewLevel, bLevelUp);
}

