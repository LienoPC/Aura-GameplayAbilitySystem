// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Game/AuraGameModeBase.h"
#include "Game/AuraSaveGame.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"

bool UAuraAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContext, FWidgetControllerParams& OutWCParams, AAuraHUD*& OutAuraHUD)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContext, 0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			OutAuraHUD = AuraHUD;
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			OutWCParams.AttributeSet = AS;
			OutWCParams.AbilitySystemComponent = ASC;
			OutWCParams.PlayerState = PS;
			OutWCParams.PlayerController = PC;
			return true;
		}
	}
	return false;
}


UAbilitySystemComponent* UAuraAbilitySystemLibrary::GetAbilitySystemComponent(AActor* Target)
{
	TScriptInterface<IAbilitySystemInterface> ASCInterface = Target;
	if (ASCInterface)
	{
		return ASCInterface->GetAbilitySystemComponent();
	}
	return nullptr;
}

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WidgetControllerParams;
	AAuraHUD* AuraHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WidgetControllerParams, AuraHUD))
	{
		return AuraHUD->GetOverlayWidgetController(WidgetControllerParams);
	}

	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WidgetControllerParams;
	AAuraHUD* AuraHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WidgetControllerParams, AuraHUD))
	{
		return AuraHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
	}

	return nullptr;
}

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WidgetControllerParams;
	AAuraHUD* AuraHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WidgetControllerParams, AuraHUD))
	{
		return AuraHUD->GetSpellMenuWidgetController(WidgetControllerParams);
	}
	return nullptr;
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContext, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* AbilitySystemComponent)
{
	AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContext));

	if(!GameMode)
		return;

	AActor* AvatarActor = AbilitySystemComponent->GetAvatarActor();

	FGameplayEffectContextHandle PrimaryContextHandle = AbilitySystemComponent->MakeEffectContext();
	PrimaryContextHandle.AddSourceObject(AvatarActor);

	FGameplayEffectContextHandle SecondaryContextHandle = AbilitySystemComponent->MakeEffectContext();
	SecondaryContextHandle.AddSourceObject(AvatarActor);

	FGameplayEffectContextHandle VitalContextHandle = AbilitySystemComponent->MakeEffectContext();
	VitalContextHandle.AddSourceObject(AvatarActor);
	
	// Get class default info to retreive GameplayEffect of default primary attributes
	FCharacterClassDefaultInfo ClassDefaultInfo = GameMode->CharacterClassInfo->GetCharacterClassDefaultInfo(CharacterClass);

	// Apply the gameplay effect to initialize the attributes
	FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, PrimaryContextHandle);
	FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GameMode->CharacterClassInfo->SecondaryAttributes, Level, SecondaryContextHandle);
	FGameplayEffectSpecHandle VitalAttributesSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GameMode->CharacterClassInfo->VitalAttributes, Level, VitalContextHandle);

	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
	
}

void UAuraAbilitySystemLibrary::InitializeDefaultAbilities(const UObject* WorldContext, UAbilitySystemComponent* ASC, ECharacterClass Class)
{
	// Get class default info to retrieve GameplayEffect of default primary attributes
	UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContext);

	if (ClassInfo == nullptr) return;
	
	for(auto AbilityClass : ClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}

	int32 Level = 1;
	if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
	{
		Level = ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor());
	}
	for (auto Ability : ClassInfo->GetCharacterClassDefaultInfo(Class).StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, Level);
		ASC->GiveAbility(AbilitySpec);
	}

}

void UAuraAbilitySystemLibrary::InitializeDefaultAbilitiesFromSaveData(const UObject* WorldContext,
	UAbilitySystemComponent* ASC, UAuraSaveGame* SaveGame)
{
	UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContext);

	if (ClassInfo == nullptr) return;

	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();

	const AActor* SourceAvatarActor = ASC->GetAvatarActor();

	FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();

	EffectContextHandle.AddSourceObject(SourceAvatarActor);

	const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ClassInfo->PrimaryAttributes_SetByCaller, 1.0f, EffectContextHandle);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Primary_Strength, SaveGame->Strength);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Primary_Intelligence, SaveGame->Intelligence);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Primary_Resilience, SaveGame->Resilience);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Primary_Vigor, SaveGame->Vigor);

	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);

	FGameplayEffectContextHandle SecondaryContextHandle = ASC->MakeEffectContext();
	SecondaryContextHandle.AddSourceObject(SourceAvatarActor);

	FGameplayEffectContextHandle VitalContextHandle = ASC->MakeEffectContext();
	VitalContextHandle.AddSourceObject(SourceAvatarActor);
	

	// Apply the gameplay effect to initialize the attributes
	FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassInfo->SecondaryAttributes_Infinite, 1.0f, SecondaryContextHandle);
	FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassInfo->VitalAttributes, 1.0f, VitalContextHandle);

	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	if(!WorldContextObject)
		return nullptr;
	AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));

	if(GameMode)
		return GameMode->CharacterClassInfo;

	return nullptr;
}

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{

	if(!WorldContextObject)
		return nullptr;
	AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));

	if(GameMode)
		return GameMode->AbilityInfo;

	return nullptr;
}

FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams)
{
	if (!DamageEffectParams.WorldContextObject)
	{
		return FGameplayEffectContextHandle();
	}

	if (DamageEffectParams.TargetAbilitySystemComponent == nullptr)
	{
		return FGameplayEffectContextHandle();
	}
	if (DamageEffectParams.DamageGameplayEffectClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Calling Apply Damage Effect with null DamageGameplayEffectClass"));
		return FGameplayEffectContextHandle();
	}
	FGameplayEffectContextHandle ContextHandle = DamageEffectParams.TargetAbilitySystemComponent->MakeEffectContext();
	ContextHandle.AddSourceObject(DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor());
	ContextHandle.AddInstigator(DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor(), DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor());
	SetDeathImpulse(ContextHandle, DamageEffectParams.DeathImpulse);
	SetKnockbackForce(ContextHandle, DamageEffectParams.KnockbackForce);

	SetIsRadialDamage(ContextHandle, DamageEffectParams.bIsRadialDamage);
	SetRadialDamageInnerRadius(ContextHandle, DamageEffectParams.RadialDamageInnerRadius);
	SetRadialDamageOuterRadius(ContextHandle, DamageEffectParams.RadialDamageOuterRadius);
	SetRadialDamageOrigin(ContextHandle, DamageEffectParams.RadialDamageOrigin);
	
	FGameplayEffectSpecHandle SpecHandle = DamageEffectParams.TargetAbilitySystemComponent->MakeOutgoingSpec(DamageEffectParams.DamageGameplayEffectClass, DamageEffectParams.AbilityLevel, ContextHandle);
 
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageEffectParams.DamageType, DamageEffectParams.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, FAuraGameplayTags::Get().Debuff_Chance, DamageEffectParams.DebuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, FAuraGameplayTags::Get().Debuff_Damage, DamageEffectParams.DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, FAuraGameplayTags::Get().Debuff_Duration, DamageEffectParams.DebuffDuration);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, FAuraGameplayTags::Get().Debuff_Frequency, DamageEffectParams.DebuffFrequency);

	DamageEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());


	return ContextHandle;
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(EffectContext)
	{
		return EffectContext->IsBlockedHit();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(EffectContext)
	{
		return EffectContext->IsCriticalHit();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(EffectContext)
	{
		return EffectContext->IsSuccessfulDebuff();
	}
	return false;
}

float UAuraAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(EffectContext)
	{
		return EffectContext->GetDebuffDamage();
	}
	return -1.f;
}

float UAuraAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(EffectContext)
	{
		return EffectContext->GetDebuffDuration();
	}
	return -1.f;
}

float UAuraAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(EffectContext)
	{
		return EffectContext->GetDebuffFrequency();
	}
	return -1.f;
}

FGameplayTag UAuraAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(EffectContext)
	{
		if (EffectContext->GetDamageType().IsValid())
		{
			return *EffectContext->GetDamageType();
		}
	}
	return FGameplayTag();
}

FVector UAuraAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(EffectContext)
	{
		return EffectContext->GetDeathImpulse();
	}
	return FVector::ZeroVector;
}

FVector UAuraAbilitySystemLibrary::GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(EffectContext)
	{
		return EffectContext->GetKnockbackForce();
	}
	return FVector::ZeroVector;
}

bool UAuraAbilitySystemLibrary::IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(EffectContext)
	{
		return EffectContext->IsRadialDamage();
	}
	return false;
}

float UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(EffectContext)
	{
		return EffectContext->GetRadialDamageInnerRadius();
	}
	return 0.0f;
}

float UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(EffectContext)
	{
		return EffectContext->GetRadialDamageOuterRadius();
	}
	return 0.0f;
}

FVector UAuraAbilitySystemLibrary::GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(EffectContext)
	{
		return EffectContext->GetRadialDamageOrigin();
	}
	return FVector::ZeroVector;
}


void UAuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool Value)
{
	FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(EffectContext)
	{
		EffectContext->SetIsBlockedHit(Value);
	}
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool Value)
{
	FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(EffectContext)
	{
		EffectContext->SetIsCriticalHit(Value);
	}
}

void UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(FGameplayEffectContextHandle& EffectContextHandle, bool Value)
{
	FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(EffectContext)
	{
		EffectContext->SetIsSuccessfulDebuff(Value);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle, float Value)
{
	FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(EffectContext)
	{
		EffectContext->SetDebuffDamage(Value);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle, float Value)
{
	FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(EffectContext)
	{
		EffectContext->SetDebuffDuration(Value);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle, float Value)
{
	FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(EffectContext)
	{
		EffectContext->SetDebuffFrequency(Value);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDamageType(FGameplayEffectContextHandle& EffectContextHandle,
	const FGameplayTag& Value)
{
	FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(EffectContext)
	{
		TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(Value);
		EffectContext->SetDamageType(DamageType);
	}
}

void UAuraAbilitySystemLibrary::SetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle, const FVector& Value)
{
	FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(EffectContext)
	{
		EffectContext->SetDeathImpulse(Value);
	}
}

void UAuraAbilitySystemLibrary::SetKnockbackForce(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& Value)
{
	FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(EffectContext)
	{
		EffectContext->SetKnockbackForce(Value);
	}
}

void UAuraAbilitySystemLibrary::SetIsRadialDamage(FGameplayEffectContextHandle& EffectContextHandle, bool Value)
{
	FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(EffectContext)
	{
		EffectContext->SetIsRadialDamage(Value);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageInnerRadius(FGameplayEffectContextHandle& EffectContextHandle,
	const float Value)
{
	FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(EffectContext)
	{
		EffectContext->SetRadialDamageInnerRadius(Value);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageOuterRadius(FGameplayEffectContextHandle& EffectContextHandle,
	const float Value)
{
	FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(EffectContext)
	{
		EffectContext->SetRadialDamageOuterRadius(Value);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageOrigin(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& Value)
{
	FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(EffectContext)
	{
		EffectContext->SetRadialDamageOrigin(Value);
	}
}

void UAuraAbilitySystemLibrary::GetLiveEntitiesWithinRadius(const UObject* WorldContext, const FVector& Location,
                                                            const float Radius, TArray<AActor*>& OutEntities, const TArray<AActor*> &FilteredOut)
{
	if(!WorldContext)
		return;
	FCollisionQueryParams Params;

	Params.AddIgnoredActors(FilteredOut);

	TArray<FOverlapResult> OverlapResults;

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull))
	{
		World->OverlapMultiByObjectType(OverlapResults, Location, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), Params);
	}

	for (auto &OverlapResult : OverlapResults)
	{
		if (AActor* OverlapActor = OverlapResult.GetActor())
		{
			if (OverlapActor->Implements<UCombatInterface>())
			{
				if (!ICombatInterface::Execute_IsDead(OverlapActor))
				{
					OutEntities.AddUnique(ICombatInterface::Execute_GetAvatar(OverlapActor));
				}
			}
		}
	}
	
}

void UAuraAbilitySystemLibrary::GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Actors,
	TArray<AActor*>& OutTargets, const FVector& Origin)
{
	if (Actors.Num() <= MaxTargets)
	{
		OutTargets = Actors;
		return;
	}

	OutTargets = Actors;

	while (MaxTargets < OutTargets.Num())
	{
		AActor* FarthestActor = nullptr;
		double FarthestDistance = -1.0f;
		for (AActor* PotentialTarget : OutTargets)
		{
			if (IsValid(PotentialTarget))
			{
				const double Distance = (PotentialTarget->GetActorLocation() - Origin).SquaredLength();
				if (FarthestDistance < Distance)
				{
					FarthestDistance = Distance;
					FarthestActor = PotentialTarget;
				}
			}
		
		}
		OutTargets.Remove(FarthestActor);
	}

	return;

	
}

bool UAuraAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	const bool bFirstIsPlayer = FirstActor->ActorHasTag("Player");
	const bool bSecondIsPlayer = SecondActor->ActorHasTag("Player");

	const bool bFirstIsEnemy = FirstActor->ActorHasTag("Enemy");
	const bool bSecondIsEnemy = SecondActor->ActorHasTag("Enemy");

	const bool bBothArePlayers = bFirstIsPlayer && bSecondIsPlayer;

	const bool bBothAreEnemies = bFirstIsEnemy && bSecondIsEnemy;

	return !(bBothArePlayers || bBothAreEnemies);
}

TArray<FRotator> UAuraAbilitySystemLibrary::EvenlySpacedRotators(const FVector& ForwardVector, const FVector& Axis, const float Spread, const int32 NumRotators)
{
	TArray<FRotator> Rotators;
	const FVector LeftOfSpread = ForwardVector.RotateAngleAxis(-Spread/2, Axis);
	if (NumRotators > 1)
	{
		const float DeltaSpread = Spread/(NumRotators-1);
		for (int32 i = 0; i < NumRotators; ++i)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread*i, Axis);
			Rotators.Add(Direction.Rotation());
		}
	}else
	{
		Rotators.Add(ForwardVector.Rotation());
	}
	return Rotators;
	

}

TArray<FVector> UAuraAbilitySystemLibrary::EvenlyRotatedVectors(const FVector& ForwardVector, const FVector& Axis, const float Spread,  const int32 NumVectors)
{
	TArray<FVector> Vectors;
	const FVector LeftOfSpread = ForwardVector.RotateAngleAxis(-Spread/2, Axis);
	if (NumVectors > 1)
	{
		const float DeltaSpread = Spread/(NumVectors-1);
		for (int32 i = 0; i < NumVectors; ++i)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread*i, Axis);
			Vectors.Add(Direction);
		}
	}else
	{
		Vectors.Add(ForwardVector);
	}
	return Vectors;
}

int32 UAuraAbilitySystemLibrary::GetGrantedXP(UWorld* WorldContext, const ECharacterClass CharacterClass, int32 Level)
{
	UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContext);
	FCharacterClassDefaultInfo DefaultInfo = ClassInfo->GetCharacterClassDefaultInfo(CharacterClass);

	return DefaultInfo.GrantedXP.GetValueAtLevel(Level);
}

void UAuraAbilitySystemLibrary::SetIsRadialDamageEffectParams(FDamageEffectParams& DamageEffectParams, bool Value,
	float InnerRadius, float OuterRadius, FVector Origin)
{
	DamageEffectParams.bIsRadialDamage = Value;
	DamageEffectParams.RadialDamageInnerRadius = InnerRadius;
	DamageEffectParams.RadialDamageOuterRadius = OuterRadius;
	DamageEffectParams.RadialDamageOrigin = Origin;
}

void UAuraAbilitySystemLibrary::SetKnockbackDirection(FDamageEffectParams& DamageEffectParams,
	FVector KnockbackDirection, float MagnitudeOverride)
{
	KnockbackDirection.Normalize();
	DamageEffectParams.KnockbackForce = MagnitudeOverride > 0 ? KnockbackDirection * MagnitudeOverride : KnockbackDirection * DamageEffectParams.KnockbackMagnitude;
}

void UAuraAbilitySystemLibrary::SetDeathImpulseDirection(FDamageEffectParams& DamageEffectParams,
	FVector ImpulseDirection, float MagnitudeOverride)
{
	ImpulseDirection.Normalize();
	DamageEffectParams.DeathImpulse = MagnitudeOverride > 0 ? ImpulseDirection * MagnitudeOverride : ImpulseDirection * DamageEffectParams.DeathImpulseMagnitude;
}

void UAuraAbilitySystemLibrary::SetTargeASCDamageEffectParams(FDamageEffectParams& DamageEffectParams,
	UAbilitySystemComponent* TargetASC)
{
	DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
}

