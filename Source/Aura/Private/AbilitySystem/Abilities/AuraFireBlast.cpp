// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraFireBlast.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraFireball.h"


TArray<AAuraFireball*> UAuraFireBlast::SpawnFireballs()
{
	TArray<AAuraFireball*> SpawnedFireballs;
	
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	TArray<FRotator> Rotators = UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, 360.0f, NumFireballs);

	for (const FRotator& Rotator : Rotators)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(Location);
		SpawnTransform.SetRotation(Rotator.Quaternion());
		AAuraFireball* Fireball = GetWorld()->SpawnActorDeferred<AAuraFireball>(FireballClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			CurrentActorInfo->PlayerController->GetPawn(),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		Fireball->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
		SpawnedFireballs.Add(Fireball);
		Fireball->ReturnToActor = GetAvatarActorFromActorInfo();

		Fireball->ExplosionDamageParams = MakeDamageEffectParamsFromClassDefaults();
		Fireball->SetOwner(GetAvatarActorFromActorInfo());
		Fireball->FinishSpawning(SpawnTransform);
	}
	return SpawnedFireballs;
}

FString UAuraFireBlast::GetDescription(int32 Level)
{
	const int32 DamageVal = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);

	return FString::Printf(TEXT(
		// Title
		"<Title>FIRE BLAST</>\n\n"

		// Details
		"<Small>Level: </><Level>%d</>\n"
		// Mana Cost
		"<Small>ManaCost: </><ManaCost>%.1f</>\n"
		// Cooldown
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		
		// Description
		"<Default>Launches %d fire balls in all direction, each coming back and exploding upon return, causing:"
		"</><Damage>%d</><Default> radial fire "
		"damage with a chance to burn</>\n\n"
		),
		// Values
		Level, ManaCost, Cooldown, NumFireballs, DamageVal);

	
}

FString UAuraFireBlast::GetNextLevelDescription(int32 Level)
{
	return GetDescription(Level);
}
