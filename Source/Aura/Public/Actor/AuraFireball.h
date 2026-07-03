// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/AuraProjectile.h"
#include "AuraFireball.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraFireball : public AAuraProjectile
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void StartOutgoingTimeline();


	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> ReturnToActor;
	
	UPROPERTY(BlueprintReadWrite)
	FDamageEffectParams ExplosionDamageParams;
	
protected:

	virtual void BeginPlay() override;
	
	virtual void OnOverlap(AActor* Other) override;
	virtual void OnEndOverlap(AActor* Other) override;


};
