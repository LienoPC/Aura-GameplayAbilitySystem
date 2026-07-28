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
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "Game/AuraGameInstance.h"
#include "Game/AuraGameModeBase.h"
#include "Game/AuraSaveGame.h"
#include "Kismet/GameplayStatics.h"

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
		PC->bShowMouseCursor = false;
	}
}

void AAuraCharacter::HideMagicCircle_Implementation()
{
	if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(GetController()))
	{
		PC->HideMagicCircle();
		PC->bShowMouseCursor = true;
	}
}

void AAuraCharacter::SaveProgress_Implementation(const FName& CheckpointTag)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	if (AuraGameMode && AuraGameInstance)
	{
		UAuraSaveGame* SaveData = AuraGameMode->RetreiveInGameSaveData();
		if (SaveData == nullptr)
			return;

		SaveData->PlayerStartTag = CheckpointTag;
		
		if (AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState()))
		{
			SaveData->PlayerLevel = AuraPlayerState->GetPlayerLevel();
			SaveData->XP = AuraPlayerState->GetXP();
			SaveData->AttributePoints = AuraPlayerState->GetAttributePoints();
			SaveData->SpellPoints = AuraPlayerState->GetSpellPoints();
		}

		SaveData->Strength = UAuraAttributeSet::GetStrengthAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Intelligence = UAuraAttributeSet::GetIntelligenceAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Resilience = UAuraAttributeSet::GetResilienceAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Vigor = UAuraAttributeSet::GetVigorAttribute().GetNumericValue(GetAttributeSet());
		SaveData->bFirstTimeLoadIn = false;

		if (!HasAuthority())
			return;

		UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent());
		FForEachAbility SaveAbilityDelegate;
		SaveData->SavedAbilities.Empty();
		SaveAbilityDelegate.BindLambda([this, ASC, &SaveData](const FGameplayAbilitySpec& AbilitySpec)
		{
			FSavedAbility SavedAbility;

			auto AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(this);
			auto Info = AbilityInfo->FindAbilityInfoByTag(ASC->GetAbilityTagFromSpec(AbilitySpec));
			SavedAbility.GameplayAbility = Info.Ability;
			SavedAbility.AbilityTag = Info.AbilityTag;
			SavedAbility.AbilitySlot = ASC->GetInputTagFromAbilityTag(Info.AbilityTag);
			SavedAbility.AbilityStatus = ASC->GetStatusFromAbilityTag(Info.AbilityTag);
			SavedAbility.AbilityLevel = SavedAbility.AbilityStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Locked) ? Info.LevelRequirement : AbilitySpec.Level;
			SavedAbility.AbilityType = Info.AbilityType;
			
			SaveData->SavedAbilities.AddUnique(SavedAbility);
		});

		ASC->ForEachAbility(SaveAbilityDelegate);
		AuraGameMode->SaveInGameProgressData(SaveData);
	}
}

void AAuraCharacter::Die(const FVector& DeathImpulse)
{
	Super::Die(DeathImpulse);

	FTimerDelegate DeathTimerDelegate;
	DeathTimerDelegate.BindLambda([this]()
	{
		if (AAuraGameModeBase* AuraGM = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			AuraGM->PlayerDied(this);
		}
	});

	GetWorldTimerManager().SetTimer(DeathTimer, DeathTimerDelegate, DeathTime, false);
	Camera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}


void AAuraCharacter::BeginPlay()
{
	Super::BeginPlay();

	
}

void AAuraCharacter::InitAbilityActorInfo()
{
	// Never run twice
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

	//InitializeDefaultAttributes();
	
}

void AAuraCharacter::LoadProgress()
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (AuraGameMode)
	{
		UAuraSaveGame* SaveData = AuraGameMode->RetreiveInGameSaveData();
		if (SaveData == nullptr)
			return;

		if (SaveData->bFirstTimeLoadIn)
		{
			InitializeDefaultAttributes();
			AddCharacterAbilities();
		}else
		{

			if (UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent()))
			{
				ASC->AddCharacterAbilitiesFromSaveData(SaveData);
			}
			if (AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState()))
			{
				AuraPlayerState->SetPlayerLevel(SaveData->PlayerLevel);
				AuraPlayerState->SetXP(SaveData->XP);
				AuraPlayerState->SetAttributePoints(SaveData->AttributePoints);
				AuraPlayerState->SetSpellPoints(SaveData->SpellPoints);
			}
			UAuraAbilitySystemLibrary::InitializeDefaultAbilitiesFromSaveData(this, AbilitySystemComponent, SaveData);

			
		}
		
	}
	
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
	LoadProgress();
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (AuraGameMode)
	{
		AuraGameMode->LoadWorldState(GetWorld());
	}
	//if (bAbilitySystemInitialized)
		//AddCharacterAbilities();
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


