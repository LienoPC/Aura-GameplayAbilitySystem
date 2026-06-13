// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Player/AuraPlayerState.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);

	for (auto& Pair : AS->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda([this, Pair](const FOnAttributeChangeData& Data)
		{
			BroadcastAttributeInfo(Pair.Key, Pair.Value());
		});
	}
	
	AAuraPlayerState* PS = CastChecked<AAuraPlayerState>(PlayerState);

	PS->OnAttributePointsChanged.AddLambda([this](int32 NewValue)
	{
		AttributePointsChangedDelegate.Broadcast(NewValue);
	});

	PS->OnSpellPointsChanged.AddLambda([this](int32 NewValue)
	{
		SpellPointsChangedDelegate.Broadcast(NewValue);
	});
	
}

void UAttributeMenuWidgetController::BroadCastInitialValues()
{
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);

	check(AttributeInfo);

	for (auto& Pair : AS->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
	
	AAuraPlayerState* PS = CastChecked<AAuraPlayerState>(PlayerState);

	AttributePointsChangedDelegate.Broadcast(PS->GetAttributePoints());
	SpellPointsChangedDelegate.Broadcast(PS->GetSpellPoints());
}

void UAttributeMenuWidgetController::AddAttributePoint(const FGameplayTag& AttributeTag)
{
	if (UAuraAbilitySystemComponent* ASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		ASC->UpgradeAttribute(AttributeTag);
	}
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag,
                                                            const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
