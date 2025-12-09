// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Interaction/EnemyInterface.h"
#include "Enemy.generated.h"

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

	UPROPERTY(BlueprintReadOnly)
	bool bHighlighted = false;

	/* Combat Interface */
	virtual int32 GetPlayerLevel() override;
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	UPROPERTY(EditAnywhere, Category="Character Class Defaults")
	int32 Level = 1;
	
	
	
};
