// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PointCollection.generated.h"

UCLASS()
class AURA_API APointCollection : public AActor
{
	GENERATED_BODY()
	
public:	
	APointCollection();

	UFUNCTION(BlueprintPure)
	TArray<USceneComponent*> GetGroundPoints(const FVector& GroundLocation, const int32 NumPoints, const float YawOverride = 0.0f);
protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(AllowPrivateAccess = true))
	TArray<USceneComponent*> ImmutablePts;

};
