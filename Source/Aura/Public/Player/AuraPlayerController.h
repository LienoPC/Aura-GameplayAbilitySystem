// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class AMagicCircle;
class UNiagaraSystem;
class UDamageTextComponent;
struct FGameplayTag;
class UAuraAbilitySystemComponent;
class UAuraInputConfig;
class IEnemyInterface;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class USplineComponent;

/**
 * Player Controller for Aura (player) that manages all input information from its InputMappingContext
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
public:

	AAuraPlayerController();
	
	virtual void PlayerTick(float DeltaTime) override;
	void HighlightTarget();

	// Spawns a Widget to show damage number
	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, bool bBlockedHit, bool bCritHit, ACharacter* TargetCharacter);

	UFUNCTION(BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* MagicCircleMaterial);
	UFUNCTION(BlueprintCallable)
	void HideMagicCircle();
protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
private:

	void Move(const FInputActionValue& InputActionValue);

	void ShiftPressed(const FInputActionValue& InputActionValue);

	void ShiftReleased(const FInputActionValue& InputActionValue);
	
	// Manages AutoRunning functionalities
	void AutoRun();
	
	// Callback function for whenever an input associated with GameplayAbilities
	// is pressed. It checks if a Target is selected and resets the auto-running.
	void AbilityInputTagPressed(FGameplayTag InputTag);

	// Callback function for whenever an input associated with GameplayAbilities
	// is released.
	void AbilityInputTagReleased(FGameplayTag InputTag);

	// Callback function for whenever an input associated with GameplayAbilities
	// is held.
	void AbilityInputTagHeld(FGameplayTag InputTag);

	void UpdateMagicCircleLocation();
	
	// InputMappingContext for the player.
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveInputAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ShiftInputAction;
	
	void CursorTrace();

	TScriptInterface<IEnemyInterface> LastActor;
	TScriptInterface<IEnemyInterface> ThisActor;
	
	// Contains InputConfig mapping to GameplayTags
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AbilitySystemComponent;

	// Initializes and returns reference to ASC of the associated Pawn.
	UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent();


	bool bIsShiftPressed = false;
	
	// Position clicked by the user.
	FVector CachedDestination = FVector::ZeroVector;
	// Time passed since player started to follow the position held by the user.
	float FollowTime = 0.f;
	// Maximum time of click before it is considered a long click.
	float ShortPressedThreshold = 0.5f;
	// If it should auto-run every frame, call AddMovementInput.
	bool bAutoRunning = false;
	// Indicates if, upon clicking, the player is targeting something.
	bool bTargeting = false;

	// Minimum acceptance radius allowed for player auto-run.
	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;

	// Path that the character, during auto-run, needs to follow.
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> SplinePath;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> ClickNiagaraSystem;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMagicCircle> MagicCircleClass;
	UPROPERTY()
	TObjectPtr<AMagicCircle> SpawnedMagicCircle;

	// Cached cursor hit location for every frame
	FHitResult CursorHitLocation;

	// Text component class to show damage number
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;
};
