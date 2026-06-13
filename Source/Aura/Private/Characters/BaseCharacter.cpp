// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "GameplayEffect.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	BurnDebuffNiagaraComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>(TEXT("BurnDebuff"));
	BurnDebuffNiagaraComponent->SetupAttachment(GetRootComponent());
	BurnDebuffNiagaraComponent->DebuffTag = FAuraGameplayTags::Get().Debuff_Burn;
	
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");

	Weapon->SetupAttachment(GetMesh());

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
}

void ABaseCharacter::Destroyed()
{
	OnDestroyed.Broadcast(this);
	Super::Destroyed();
}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* ABaseCharacter::GetAttributeSet() const
{
	return AttributeSet;
}

FVector ABaseCharacter::GetCombatSocketLocation_Implementation(const FGameplayTag& AttackTag)
{
if (IsValid(Weapon->GetSkinnedAsset()) && (AttackTag.MatchesTagExact(FAuraGameplayTags::Get().CombatSocket_Attack_Weapon) || AttackTag.MatchesTagExact(FAuraGameplayTags::Get().CombatSocket_Attack_Ranged)))
	{
		return Weapon->GetSocketLocation(WeaponTipSocketName);
	}
	if (FName* SocketNamePtr = TagToSocketName.Find(AttackTag); SocketNamePtr)
	{
		return GetMesh()->GetSocketLocation(*SocketNamePtr);
	}
	return FVector();
}

UAnimMontage* ABaseCharacter::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

bool ABaseCharacter::IsDead_Implementation() const
{
	return bDead;
}

AActor* ABaseCharacter::GetAvatar_Implementation()
{
	return this;
}

void ABaseCharacter::Die(const FVector& DeathImpulse)
{
	FDetachmentTransformRules TransformRules(EDetachmentRule::KeepWorld, true);
	Weapon->DetachFromComponent(TransformRules);

	// Play death sound
	if (DeathSound)
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	// Called only on the server
	MulticastHandleDeath(DeathImpulse);
}

TArray<FTaggedMontage> ABaseCharacter::GetAttackMontages_Implementation()
{
	return TagToMontage;
}

UNiagaraSystem* ABaseCharacter::GetBloodEffectSystem_Implementation()
{
	return BloodEffect;
}

FTaggedMontage ABaseCharacter::GetTaggedMontageByTag_Implementation(const FGameplayTag& Tag)
{
	FTaggedMontage* Found = TagToMontage.FindByPredicate([Tag] (FTaggedMontage TagToMontage)
	{
		return TagToMontage.MontageTag.MatchesTagExact(Tag);
	});
	if (Found)
		return *Found;
	
	return FTaggedMontage();
}

int32 ABaseCharacter::GetMinionCount_Implementation()
{
	return MinionCount;
}

void ABaseCharacter::IncrementMinionCount_Implementation(int32 Amount)
{
	MinionCount += Amount;
}

ECharacterClass ABaseCharacter::GetCharacterClass_Implementation()
{
	return CharacterClass;
}

FOnASCRegistered ABaseCharacter::GetOnAscRegisteredDelegate()
{
	return OnAscRegistered;
}

FOnDeath ABaseCharacter::GetOnDeathDelegate()
{
	return OnDeath;
}


void ABaseCharacter::MulticastHandleDeath_Implementation(const FVector& DeathImpulse)
{
	// Enable ragdoll on the weapon
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::Type::PhysicsOnly);
	Weapon->AddImpulse(DeathImpulse);
	
	// Enable ragdoll on the enemy
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::Type::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Apply here death impulse
	GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);

	// Dissolve character after death
	Dissolve();
	bDead = true;
	OnDeath.Broadcast(this);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, BaseHandWeaponSocket);

	
}

void ABaseCharacter::InitAbilityActorInfo()
{
}

void ABaseCharacter::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);
	
	FGameplayEffectContextHandle EffectContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	EffectContextHandle.Get()->AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, level,EffectContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*EffectSpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void ABaseCharacter::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);

	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}

void ABaseCharacter::AddCharacterAbilities()
{
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	if (!HasAuthority())return;
	AuraASC->AddCharacterAbilities(StartupAbilities);
	AuraASC->AddCharacterPassiveAbilities(StartupPassiveAbilities);
}

void ABaseCharacter::Dissolve()
{
	if(IsValid(DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicMatInst);

		StartDissolvingTimeline(DynamicMatInst);
	}

	if(IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
		Weapon->SetMaterial(0, DynamicMatInst);

		StartWeaponDissolvingTimeline(DynamicMatInst);
	}
}





