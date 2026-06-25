// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

class UNiagaraComponent;
class UCameraComponent;
class USpringArmComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public ABaseCharacter, public IPlayerInterface
{
	GENERATED_BODY()
public:
	AAuraCharacter();

	// Player Interface

	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual void LevelUp_Implementation(int32 TargetLevel) override;
	virtual int32 GetXP_Implementation() override;
	virtual int32 FindLevelForXP_Implementation(const int32& InXP) override;
	virtual int32 GetAttributePoints_Implementation() const override;
	virtual int32 GetSpellPoints_Implementation() const override;
	virtual void AddToAttributePoints_Implementation(const int32& InAttributePoints) override;
	virtual void AddToSpellPoints_Implementation(const int32& InSpellPoints) override;
	//End Player Interface

	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Stunned() override;
	virtual void PossessedBy(AController* NewController) override;

	virtual int32 GetPlayerLevel_Implementation() override;

protected:

	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> LevelupNiagaraComponent;
private:

	bool bAbilitySystemInitialized = false;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelupParticles() const;
};
