// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Enemy.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AI/AuraAIController.h"
#include "Aura/Aura.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"

AEnemy::AEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));

	HealthBar->SetupAttachment(GetRootComponent());
	HealthBar->SetVisibility(true);

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	BaseWalkSpeed = 300.f;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	
}

void AEnemy::HighlightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AEnemy::UnHighlightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

void AEnemy::SetMoveToLocation_Implementation(FVector& OutDestination)
{
}


int32 AEnemy::GetPlayerLevel_Implementation()
{
	return Level;
}

void AEnemy::BindOnAttributeChangedCallbacks()
{
	if(UAuraAttributeSet* AuraAS = Cast<UAuraAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		});
		
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		});
	}

	// Bind a callback when a particular tag is added
	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Abilities_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AEnemy::HitReactTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AEnemy::StunTagChanged);
}

void AEnemy::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* AEnemy::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}

void AEnemy::SetIsBeingShocked_Implementation(const bool InBeingShocked)
{
	Super::SetIsBeingShocked_Implementation(InBeingShocked);
	if (AuraAIController && AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("Shocked"), bIsBeingShocked);
	}
}

void AEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;

	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;

	if (AuraAIController && AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
	}
	
}

void AEnemy::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	Super::StunTagChanged(CallbackTag, NewCount);
	if (AuraAIController && AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("Stunned"), bIsStunned);
	}
}

void AEnemy::Die(const FVector& DeathImpulse)
{
	Super::Die(DeathImpulse);
	if (AuraAIController)
	{
		if (UBlackboardComponent* BlackboardComponent = AuraAIController->GetBlackboardComponent())
		{
			BlackboardComponent->SetValueAsBool(FName("IsDead"), true);
		}
	}
	SetLifeSpan(LifeSpan);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	check(AbilitySystemComponent);
	InitAbilityActorInfo();

	if(HasAuthority())
		UAuraAbilitySystemLibrary::InitializeDefaultAbilities(this, AbilitySystemComponent, CharacterClass);
	
	// HUD Controller initialization and bind
	BindOnAttributeChangedCallbacks();
	InitHUD();

	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

	
}

void AEnemy::InitializeDefaultAttributes() const
{
	UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}

void AEnemy::InitHUD()
{
	if(UAuraUserWidget* Widget = Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		Widget->SetWidgetController(this);
		if(UAuraAttributeSet* AuraAS = Cast<UAuraAttributeSet>(AttributeSet))
		{
			OnHealthChanged.Broadcast(AuraAS->GetHealth());
			OnMaxHealthChanged.Broadcast(AuraAS->GetMaxHealth());
		}
	}
}

void AEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	if(HasAuthority())
		InitializeDefaultAttributes();
	OnAscRegistered.Broadcast(AbilitySystemComponent);
}

void AEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// AI only matters on the server
	if(!HasAuthority())
		return;
	AuraAIController = Cast<AAuraAIController>(NewController);
	
	AuraAIController->RunBehaviorTree(BehaviorTree);
	AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->GetBlackboardAsset());


	AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), CharacterClass == ECharacterClass::Ranged || CharacterClass == ECharacterClass::Elementalist);
}
