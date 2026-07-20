// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

class UAuraSaveGame;
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetTags*/)
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven);
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChanged, const FGameplayTag& /** Ability Tag */, const FGameplayTag& /** Status Tag */, const int32 /** Level */)
DECLARE_MULTICAST_DELEGATE_FourParams(FAbilityEquipped, const FGameplayTag& /** AbilityType*/, const FGameplayTag& /** Status */, const FGameplayTag& /** Input */, const FGameplayTag& /** OldInput */)
DECLARE_MULTICAST_DELEGATE_OneParam(FDeactivatePassiveAbility, const FGameplayTag& /** Ability Tag */);
DECLARE_MULTICAST_DELEGATE_TwoParams(FActivatePassiveEffect, const FGameplayTag& /** Ability Tag */, const bool /** bActivate */);


/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	
	void AbilityActorInfoSet();

	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& PassiveAbilities);
	void AddCharacterAbilitiesFromSaveData(UAuraSaveGame* SaveGame);
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	
	// Iterate through abilities and activate all abilities (not already active) associated with a particular Tag.
	void AbilityInputTagHeld(const FGameplayTag& InputTag);

	// Iterate through abilities and activate all abilities (not already active) associated with a particular Tag.
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ForEachAbility(const FForEachAbility& Delegate);

	void UpgradeAttribute(const FGameplayTag& AttributeTag);

	bool GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription);

	void ClearInputTag(FGameplayAbilitySpec* AbilitySpec);
	void ClearAbilitiesOfInputTag(const FGameplayTag& InputTag);
	bool InputSlotIsEmpty(const FGameplayTag& InputTag);
	bool IsPassiveAbilityFromSpec(const FGameplayAbilitySpec& Spec);
	static bool AbilityHasAnyInputTag(const FGameplayAbilitySpec& Spec);
	void AssignInputTagToAbility(FGameplayAbilitySpec& Spec, const FGameplayTag& InputTag);
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastActivatePassiveEffect(const FGameplayTag& AbilityTag, bool bActivate);
	FGameplayAbilitySpec* GetAbilitySpecFromInputTag(const FGameplayTag& InputTag);

	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);

	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoint(const FGameplayTag& AbilityTag);

	UFUNCTION(Server, Reliable)
	void ServerEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag);

	UFUNCTION(Client, Reliable)
	void ClientEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& NewInputTag, const FGameplayTag& OldInputTag);
	
	void UpdateAbilityStatuses(int32 Level);
	
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetAbilityInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetAbilityStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	FGameplayTag GetStatusFromAbilityTag(const FGameplayTag& AbilityTag);
	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);
	FGameplayTag GetInputTagFromAbilityTag(const FGameplayTag& AbilityTag);
	
	
	FEffectAssetTags EffectAssetTags;
	FAbilitiesGiven AbilitiesGivenDelegate;
	FAbilityStatusChanged AbilityStatusChangedDelegate;
	FAbilityEquipped AbilityEquipped;
	FDeactivatePassiveAbility DeactivatePassiveAbilityDelegate;
	FActivatePassiveEffect ActivatePassiveEffectDelegate;

	bool bStartupAbilitiesGiven = false;

protected:

	virtual void OnRep_ActivateAbilities() override;

	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 Level);

};



