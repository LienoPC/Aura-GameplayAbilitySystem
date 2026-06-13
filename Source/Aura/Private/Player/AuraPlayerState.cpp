// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
{
	SetNetUpdateFrequency(100.0f);
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAuraPlayerState, XP);
	DOREPLIFETIME(AAuraPlayerState, Level);
	DOREPLIFETIME(AAuraPlayerState, AttributePoints);
	DOREPLIFETIME(AAuraPlayerState, SpellPoints);
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* AAuraPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}

void AAuraPlayerState::SetPlayerLevel(int32 NewLevel)
{
	Level = NewLevel;
	OnLevelChanged.Broadcast(Level);
}

void AAuraPlayerState::AddPlayerLevel(int32 NewLevel)
{
	Level += NewLevel;
	OnLevelChanged.Broadcast(Level);
}

void AAuraPlayerState::SetXP(int32 NewXP)
{
	XP = NewXP;
	if (XP < 0)
	{
		XP = 0;
	}
	OnExperienceChanged.Broadcast(XP);
}

int32 AAuraPlayerState::GetXP()
{
	return XP;
}

void AAuraPlayerState::AddToXP(int32 AddXP)
{
	XP += AddXP;

	if (XP < 0)
	{
		XP = 0;
	}
	OnExperienceChanged.Broadcast(XP);
}

void AAuraPlayerState::AddToAttributePoints(int32 AddAttributePoints)
{
	AttributePoints += AddAttributePoints;
	OnAttributePointsChanged.Broadcast(AttributePoints);
}

int32 AAuraPlayerState::GetAttributePoints()
{
	return AttributePoints;
}

int32 AAuraPlayerState::GetSpellPoints()
{
	return SpellPoints;
}

void AAuraPlayerState::AddToSpellPoints(int32 AddSpellPoints)
{
	SpellPoints += AddSpellPoints;
	OnSpellPointsChanged.Broadcast(SpellPoints);
}


void AAuraPlayerState::OnRep_XP()
{
	// Broadcast level received to the client
	OnExperienceChanged.Broadcast(XP);
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
	OnLevelChanged.Broadcast(Level);
}

void AAuraPlayerState::OnRep_AttributePoints()
{
	OnAttributePointsChanged.Broadcast(AttributePoints);
}

void AAuraPlayerState::OnRep_SpellPoints()
{
	OnSpellPointsChanged.Broadcast(SpellPoints);
}

