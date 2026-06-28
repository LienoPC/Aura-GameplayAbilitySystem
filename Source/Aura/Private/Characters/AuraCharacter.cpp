// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AuraCharacter.h"
#include "Characters/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"

AAuraCharacter::AAuraCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	LevelupNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	LevelupNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelupNiagaraComponent->bAutoActivate = false;
	CharacterClass = ECharacterClass::Elementalist;
}

void AAuraCharacter::AddToXP_Implementation(int32 InXP)
{
	AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState());
	check(AuraPlayerState);
	AuraPlayerState->AddToXP(InXP);
	
}

void AAuraCharacter::LevelUp_Implementation(int32 TargetLevel)
{
	// TODO: GetAttributePointsReward and SpellPointsReward

	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	ULevelUpInfo* LevelUpInfo = AuraPlayerState->LevelsInfo;

	AuraPlayerState->AddPlayerLevel(1);
	int32 AttributePointReward = LevelUpInfo->Levels[TargetLevel-1].AttributePointReward;
	int32 SpellPointsReward = LevelUpInfo->Levels[TargetLevel-1].SpellPointReward;

	// Add here to attribute points
	AuraPlayerState->AddToAttributePoints(AttributePointReward);
	AuraPlayerState->AddToSpellPoints(SpellPointsReward);
	// Add here to spell points

	MulticastLevelupParticles();

	if (UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		ASC->UpdateAbilityStatuses(TargetLevel);
	}
}

int32 AAuraCharacter::GetXP_Implementation()
{
	AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState());
	check(AuraPlayerState);
	return AuraPlayerState->GetXP();
}

int32 AAuraCharacter::FindLevelForXP_Implementation(const int32& InXP)
{
	AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState());
	check(AuraPlayerState);
	return AuraPlayerState->LevelsInfo->LevelAtXP(InXP);
}

int32 AAuraCharacter::GetAttributePoints_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState());
	check(AuraPlayerState);
	return AuraPlayerState->GetAttributePoints();
}

int32 AAuraCharacter::GetSpellPoints_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState());
	check(AuraPlayerState);
	return AuraPlayerState->GetSpellPoints();
}

void AAuraCharacter::AddToAttributePoints_Implementation(const int32& InAttributePoints)
{
	AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState());
	check(AuraPlayerState);
	AuraPlayerState->AddToAttributePoints(InAttributePoints);
}

void AAuraCharacter::AddToSpellPoints_Implementation(const int32& InSpellPoints)
{
	AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState());
	check(AuraPlayerState);
	AuraPlayerState->AddToSpellPoints(InSpellPoints);
}

void AAuraCharacter::ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial)
{
	if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(GetController()))
	{
		PC->ShowMagicCircle(DecalMaterial);
	}
}

void AAuraCharacter::HideMagicCircle_Implementation()
{
	if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(GetController()))
	{
		PC->HideMagicCircle();
	}
}


void AAuraCharacter::BeginPlay()
{
	Super::BeginPlay();

	
}

void AAuraCharacter::InitAbilityActorInfo()
{
	// 1) Never run twice
	if (bAbilitySystemInitialized)
	{
		UE_LOG(LogTemp, Verbose, TEXT("%s: AbilitySystem already initialized, skipping."), *GetName());
		return;
	}

	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	if (!AuraPlayerState)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: No PlayerState yet, aborting InitAbilitySystem."), *GetName());
		return;
	}

	bAbilitySystemInitialized = true;

	
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	AttributeSet = AuraPlayerState->GetAttributeSet();
	
	OnAscRegistered.Broadcast(AbilitySystemComponent);
	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAuraCharacter::StunTagChanged);

	// Initialize HUD
	if(AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		if(AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
		
	}

	InitializeDefaultAttributes();
	
}

void AAuraCharacter::MulticastLevelupParticles_Implementation() const
{
	if (IsValid(LevelupNiagaraComponent))
	{
		const FVector CameraLocation = Camera->GetComponentLocation();
		const FVector NiagaraLocation = LevelupNiagaraComponent->GetComponentLocation();

		const FRotator ToCameraRotation = (CameraLocation - NiagaraLocation).Rotation();
		LevelupNiagaraComponent->SetWorldRotation(ToCameraRotation);
		LevelupNiagaraComponent->Activate(true);
	}
}


void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// Init ability actor info for the server
	InitAbilityActorInfo();
	if (bAbilitySystemInitialized)
		AddCharacterAbilities();
}

int32 AAuraCharacter::GetPlayerLevel_Implementation()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->GetPlayerLevel();
}


void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilityActorInfo();

	
}

void AAuraCharacter::OnRep_Burned()
{
	if (bIsStunned)
	{
		BurnDebuffNiagaraComponent->Activate();
	}else
	{
		BurnDebuffNiagaraComponent->Deactivate();
	}
}

void AAuraCharacter::OnRep_Stunned()
{
	if (UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		FGameplayTagContainer BlockedTags;
		BlockedTags.AddTag(FAuraGameplayTags::Get().Player_Block_Highlight);
		BlockedTags.AddTag(FAuraGameplayTags::Get().Player_Block_CursorTrace);
		BlockedTags.AddTag(FAuraGameplayTags::Get().Player_Block_InputPressed);
		BlockedTags.AddTag(FAuraGameplayTags::Get().Player_Block_InputHeld);
		BlockedTags.AddTag(FAuraGameplayTags::Get().Player_Block_InputReleased);
		if (bIsStunned)
		{
			ASC->AddLooseGameplayTags(BlockedTags);
			StunDebuffNiagaraComponent->Activate();
		}else
		{
			ASC->RemoveLooseGameplayTags(BlockedTags);
			StunDebuffNiagaraComponent->Deactivate();
		}
	}
}


