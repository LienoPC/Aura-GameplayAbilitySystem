// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "MathUtil.h"
#include "GameFramework/Actor.h"
#include "AuraEffectActor.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;
class USphereComponent;

UENUM(BlueprintType)
enum class EEffectApplicationPolicy: uint8
{
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply,
};

UENUM(BlueprintType)
enum class EEffectRemovalPolicy: uint8
{
	RemoveOnEndOverlap,
	DoNotRemove,
};

UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AAuraEffectActor();

	virtual void Tick(float DeltaSeconds) override;
protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, TArray<TSubclassOf<UGameplayEffect>> GameplayEffectClass);

	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	bool bDestroyOnEffectApplication = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	TArray<TSubclassOf<UGameplayEffect>> InstantGameplayEffectsClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	EEffectApplicationPolicy InstantEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	TArray<TSubclassOf<UGameplayEffect>> DurationGameplayEffectsClass;	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	EEffectApplicationPolicy DurationEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	TArray<TSubclassOf<UGameplayEffect>> InfiniteGameplayEffectsClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	EEffectApplicationPolicy InfiniteEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	EEffectRemovalPolicy InfiniteEffectRemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlap;

	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Apply Effects")
	float ActorLevel = 1.f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup Movement")
	bool bRotates = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup Movement")
	float RotationRate = 45.f;

	UFUNCTION(BlueprintCallable)
	void StartRotation();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup Movement")
	bool bSinMovement = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup Movement")
	float SinAmplitude = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup Movement")
	float SinPeriod = 1.f;

	UFUNCTION(BlueprintCallable)
	void StartSinMovement();

	UPROPERTY(BlueprintReadOnly, Category="Pickup Movement")
	FVector InitialLocation;
	
	UPROPERTY(BlueprintReadOnly, Category="Pickup Movement")
	FVector ItemLocation;

	UPROPERTY(BlueprintReadOnly, Category="Pickup Movement")
	FRotator ItemRotation;
private:
	void ApplyEffect(TSubclassOf<UGameplayEffect> GameplayEffectClass, UAbilitySystemComponent* TargetASC);

	void ItemMovement(const float DeltaTime);
	UPROPERTY(EditAnywhere, Category="Apply Effects")
	bool bApplyEffectsToEnemies = false;

	float RunningTime = 0.f;
};
