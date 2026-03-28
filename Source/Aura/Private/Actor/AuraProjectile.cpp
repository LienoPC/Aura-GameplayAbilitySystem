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
	/*
	Collision->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Collision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
*/
	// Create projectile movement component
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovement"));
	ProjectileMovementComponent->InitialSpeed = 550.f;
	ProjectileMovementComponent->MaxSpeed = 550.f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;

	UGameplayStatics::PlaySoundAtLocation(this, FlySound, GetActorLocation());
}

void AAuraProjectile::Destroyed()
{
	if(!bHit && !HasAuthority())
	{
		UGameplayStatics::PlaySoundAtLocation(this,ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation(), FRotator::ZeroRotator);
		if(LoopingSoundComponent)LoopingSoundComponent->Stop();
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
	if(DamageEffectSpecHandle.Data.IsValid() && DamageEffectSpecHandle.Data.Get()->GetContext().GetEffectCauser() == Other)
		return;
	// Play Impact Sound
	if(!bHit){
		UGameplayStatics::PlaySoundAtLocation(this,ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation(), FRotator::ZeroRotator);
		if(LoopingSoundComponent)LoopingSoundComponent->Stop();
	}

	
	if(HasAuthority())
	{
		// Before destroying apply the effect
		UAbilitySystemComponent* TargetASC = UAuraAbilitySystemLibrary::GetAbilitySystemComponent(Other);
		if(TargetASC)
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
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