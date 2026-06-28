// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Characters/BaseCharacter.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "Enemy.generated.h"


class UBehaviorTree;
class AAuraAIController;
class UOverlayWidgetController;
class UAuraWidgetController;
class UWidgetComponent;
/**
 * 
 */
UCLASS()
class AURA_API AEnemy : public ABaseCharacter, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;

	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignatue OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignatue OnMaxHealthChanged;
	
	UPROPERTY(BlueprintReadOnly)
	bool bHighlighted = false;

	/* Combat Interface */
	virtual int32 GetPlayerLevel_Implementation() override;
	void BindOnAttributeChangedCallbacks();
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() const override;
	virtual void SetIsBeingShocked_Implementation(const bool InBeingShocked) override;
	
	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount) override;


	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Combat")
	float LifeSpan = 5.f;
	
	UPROPERTY(BlueprintReadOnly, Category="Combat")
	bool bHitReacting = false;

	UPROPERTY(BlueprintReadWrite, Category="Combat")
	TObjectPtr<AActor> CombatTarget;
	
	virtual void Die(const FVector& DeathImpulse) override;


protected:
	virtual void BeginPlay() override;

	virtual void InitializeDefaultAttributes() const override;
	
	/* Initializes enemy HUD components */
	void InitHUD();
	virtual void InitAbilityActorInfo() override;
	UPROPERTY(EditAnywhere, Category="Character Class Defaults")
	int32 Level = 1;

	// Widget for health bar
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Widget")
	TObjectPtr<UWidgetComponent> HealthBar;

	// Since the attribute set is the same between player and enemies, we can use the OverlayWidgetController
	// also for enemy HUD. This is also useful for future extensions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Widget")
	TSubclassOf<UOverlayWidgetController> WidgetControllerClass;

	UPROPERTY(BlueprintReadWrite, Category="Widget")
	TObjectPtr<UOverlayWidgetController> WidgetController;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<AAuraAIController> AuraAIController;
	
	
};
