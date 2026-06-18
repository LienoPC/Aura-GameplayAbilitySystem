// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraProjectile.h"

#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	// Create projectile movement component
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovement"));
	ProjectileMovementComponent->InitialSpeed = 550.f;
	ProjectileMovementComponent->MaxSpeed = 550.f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;

	UGameplayStatics::PlaySoundAtLocation(this, FlySound, GetActorLocation());
}

void AAuraProjectile::OnHit()
{
	UGameplayStatics::PlaySoundAtLocation(this,ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation(), FRotator::ZeroRotator);
	if(LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	bHit = true;
}

void AAuraProjectile::Destroyed()
{
	if(LoopingSoundComponent)
    {
    	LoopingSoundComponent->Stop();
    	LoopingSoundComponent->DestroyComponent();
    }
	if(!bHit && !HasAuthority())
	{
		OnHit();
	}
	Super::Destroyed();
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);
	// Start looping sound by spawning it and attaching it to the actor.
	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(FlySound, GetRootComponent());
}

void AAuraProjectile::OnOverlap(AActor* Other)
{
	AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	if(SourceAvatarActor == Other)
		return;

	if (!UAuraAbilitySystemLibrary::IsNotFriend(SourceAvatarActor, Other))
		return;
	// Play Impact Sound
	if(!bHit){
		OnHit();
	}

	
	if(HasAuthority())
	{
		// Before destroying apply the effect
		if (UAbilitySystemComponent* TargetASC = UAuraAbilitySystemLibrary::GetAbilitySystemComponent(Other))
		{
			DamageEffectParams.DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			const bool bKnockback = FMath::RandRange(1, 100) < DamageEffectParams.KnockbackChance;
			if (bKnockback)
			{
				DamageEffectParams.KnockbackForce = DamageEffectParams.KnockbackMagnitude * GetActorForwardVector();
			}
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
		Destroy();

	}
	else
	{
		bHit = true;
	}
}

void AAuraProjectile::OnEndOverlap(AActor* Other)
{
}