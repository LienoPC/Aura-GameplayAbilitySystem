// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class ULevelUpInfo;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChanged, int32);

class UAbilitySystemComponent;
class UAttributeSet;
/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:

	FOnPlayerStatChanged OnExperienceChanged;

	FOnPlayerStatChanged OnLevelChanged;

	FOnPlayerStatChanged OnAttributePointsChanged;

	FOnPlayerStatChanged OnSpellPointsChanged;
	
	AAuraPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const;

	int32 GetPlayerLevel() const {return Level;}

	void SetPlayerLevel(int32 NewLevel);
	void AddPlayerLevel(int32 NewLevel);
	
	void SetXP(int32 NewXP);
	int32 GetXP();
	void AddToXP(int32 AddXP);

	void AddToAttributePoints(int32 AddAttributePoints);
	int32 GetAttributePoints();
	void SetAttributePoints(const int32 InPoints);
	
	void AddToSpellPoints(int32 AddSpellPoints);
	int32 GetSpellPoints();
	void SetSpellPoints(const int32 InPoints);
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULevelUpInfo> LevelsInfo;
protected:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

private:

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_XP)
	int32 XP = 0;

	UFUNCTION()
	void OnRep_XP();
	
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Level)
	int32 Level = 1;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_AttributePoints)
	int32 AttributePoints = 0;

	UFUNCTION()
	void OnRep_AttributePoints();

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_SpellPoints)
	int32 SpellPoints = 1;

	UFUNCTION()
	void OnRep_SpellPoints();

};
