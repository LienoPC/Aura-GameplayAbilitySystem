// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"

#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayTagContainer.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/MagicCircle.h"
#include "Components/SplineComponent.h"
#include "Components/WidgetComponent.h"
#include "Input/AuraInputComponent.h"
#include "GameFramework/Character.h"
#include "UI/Widget/DamageTextComponent.h"
#include "Interaction/EnemyInterface.h"
#include "Components/DecalComponent.h"


AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;

	SplinePath = CreateDefaultSubobject<USplineComponent>("Spline");
}



void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
	AutoRun();
	UpdateMagicCircleLocation();
}

void AAuraPlayerController::HighlightTarget()
{
	if (LastActor != ThisActor)
	{
		if (LastActor)
		{
			LastActor->UnHighlightActor();
		}
		if (ThisActor)
		{
			ThisActor->HighlightActor();
		}
	}
}

void AAuraPlayerController::ShowMagicCircle(UMaterialInterface* MagicCircleMaterial)
{
	if (!IsValid(SpawnedMagicCircle))
	{
		SpawnedMagicCircle = GetWorld()->SpawnActor<AMagicCircle>(MagicCircleClass);
		if (MagicCircleMaterial != nullptr)
		{
			SpawnedMagicCircle->MagicCircleDecal->SetMaterial(0, MagicCircleMaterial);
		}
	}
}

void AAuraPlayerController::HideMagicCircle()
{
	if (IsValid(SpawnedMagicCircle))
	{
		SpawnedMagicCircle->Destroy();
	}
}


void AAuraPlayerController::ShowDamageNumber_Implementation(float DamageAmount, bool bBlockedHit, bool bCritHit, ACharacter* TargetCharacter)
{
	if(IsValid(TargetCharacter) && DamageTextComponentClass && IsLocalController())
	{
		UDamageTextComponent* TextComponent = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);

		TextComponent->RegisterComponent();
		TextComponent->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		TextComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

		TextComponent->SetDamageText(DamageAmount, bBlockedHit, bCritHit);
	}
}

void AAuraPlayerController::CursorTrace()
{
	if (GetAuraAbilitySystemComponent() && GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_CursorTrace))
	{
		if (LastActor != ThisActor)
		{
			if (LastActor)
			{
				LastActor->UnHighlightActor();
			}
			if (ThisActor)
			{
				ThisActor->UnHighlightActor();
			}
			LastActor = nullptr;
			ThisActor = nullptr;
		}
		return;
	}
	
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHitLocation);
	if(!CursorHitLocation.bBlockingHit)
		return;

	LastActor = ThisActor;
	ThisActor = CursorHitLocation.GetActor();
	
	HighlightTarget();
	
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetAuraAbilitySystemComponent()
{
	if (AbilitySystemComponent == nullptr)
	{
		AbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAuraAbilitySystemLibrary::GetAbilitySystemComponent(GetPawn()));
	}
	return AbilitySystemComponent;
}



void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if(Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	TObjectPtr<UAuraInputComponent> AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);

	AuraInputComponent->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAbilityActions(InputConfig, this, &AAuraPlayerController::AbilityInputTagPressed, &AAuraPlayerController::AbilityInputTagReleased, &AAuraPlayerController::AbilityInputTagHeld);
	AuraInputComponent->BindAction(ShiftInputAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftInputAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	if (GetAuraAbilitySystemComponent() && GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputHeld))
	{
		return;
	}
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if(APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::ShiftPressed(const FInputActionValue& InputActionValue)
{
	bIsShiftPressed = true;
}

void AAuraPlayerController::ShiftReleased(const FInputActionValue& InputActionValue)
{
	bIsShiftPressed = false;
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (GetAuraAbilitySystemComponent() && GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed))
	{
		return;
	}
	if (InputTag.IsValid() && InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		bTargeting = ThisActor == nullptr ? false : true;
		bAutoRunning = false;
	}
	if (GetAuraAbilitySystemComponent())
	{
		GetAuraAbilitySystemComponent()->AbilityInputTagPressed(InputTag);
	}
}


void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetAuraAbilitySystemComponent() && GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputReleased))
	{
		return;
	}
	if (InputTag.IsValid() && !InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (!GetAuraAbilitySystemComponent())
			return;
		GetAuraAbilitySystemComponent()->AbilityInputTagReleased(InputTag);
		return;
	}

	// Verify, while pressing LMB, if it is targeting something.
	if (bTargeting || bIsShiftPressed)
	{
		if (GetAuraAbilitySystemComponent())
			GetAuraAbilitySystemComponent()->AbilityInputTagReleased(InputTag);
	}else
	{
		const APawn* ControlledPawn = GetPawn<APawn>();
		// Click-to-move behavior
		if (FollowTime <= ShortPressedThreshold && ControlledPawn)
		{
			FVector StartLocation = ControlledPawn->GetActorLocation();
			// Find a path to follow
			if (UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(this, StartLocation, CachedDestination))
			{
				// Add path points to the spline
				SplinePath->ClearSplinePoints();
				for (const FVector& Point : Path->PathPoints)
				{
					SplinePath->AddSplinePoint(Point, ESplineCoordinateSpace::World);
				}
				bAutoRunning = true;
				if (Path->PathPoints.Num() > 0)
					CachedDestination = Path->PathPoints[Path->PathPoints.Num() - 1];
			}

			if (GetAuraAbilitySystemComponent() && GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputReleased))
			{
				return;
			}
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ClickNiagaraSystem, CachedDestination);
		}
	}
	FollowTime = 0.f;
	bTargeting = false;
	
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (GetAuraAbilitySystemComponent() && GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputHeld))
	{
		return;
	}
	// Verify if we are pressing any other button different from LMB
	if (InputTag.IsValid() && !InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (!GetAuraAbilitySystemComponent())
			return;
		GetAuraAbilitySystemComponent()->AbilityInputTagHeld(InputTag);
		return;
	}
	
	// Verify, while pressing LMB, if it is targeting something.
	if (!bTargeting && !bIsShiftPressed)
	{
		// Take the target destination on movement input
		FollowTime += GetWorld()->GetDeltaSeconds();


		if (CursorHitLocation.bBlockingHit)
		{
			CachedDestination = CursorHitLocation.ImpactPoint;
		}

		if (APawn* ControlledPawn = GetPawn<APawn>())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}else
	{
		if (GetAuraAbilitySystemComponent())
			GetAuraAbilitySystemComponent()->AbilityInputTagHeld(InputTag);
	}
}

void AAuraPlayerController::UpdateMagicCircleLocation()
{
	if (IsValid(SpawnedMagicCircle))
	{
		SpawnedMagicCircle->SetActorLocation(CursorHitLocation.ImpactPoint);
	}
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning) return;
	
	if (APawn* ControlledPawn = GetPawn() )
	{
		const FVector LocationOnSpline = SplinePath->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector Direction = SplinePath->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);
		
		const float DistanceToDestination = (LocationOnSpline-CachedDestination).Length();

		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}

