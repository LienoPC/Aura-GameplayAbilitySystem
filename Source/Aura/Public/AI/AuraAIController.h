// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AuraAIController.generated.h"

// Basic AI controller that contains a Behavior Tree component and a Blackboard component.
UCLASS()
class AURA_API AAuraAIController : public AAIController
{
	GENERATED_BODY()

public:
	AAuraAIController();
	
protected:
	
	UPROPERTY()
	TObjectPtr<class UBehaviorTreeComponent> BehaviorTreeComponent;
	
	
};
