// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraBeamSpell::StoreOwnerVariables()
{
	if (CurrentActorInfo)
	{
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	}
}

void UAuraBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		MouseHitLocation = HitResult.ImpactPoint;
		MouseHitActor = HitResult.GetActor();
	}else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UAuraBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	check(OwnerCharacter);
	if (OwnerCharacter->Implements<UCombatInterface>())
	{
		if (USkeletalMeshComponent* Weapon = ICombatInterface::Execute_GetWeapon(OwnerCharacter))
		{
			TArray<AActor*> IgnoreActors;
			IgnoreActors.Add(OwnerCharacter);
			const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), FAuraGameplayTags::Get().CombatSocket_Attack_Ranged);

			FHitResult HitResult;
			UKismetSystemLibrary::SphereTraceSingle(OwnerCharacter, SocketLocation, BeamTargetLocation, 10.f, TraceTypeQuery1, false, IgnoreActors, EDrawDebugTrace::None, HitResult, true);

			if (HitResult.bBlockingHit)
			{
				MouseHitLocation = HitResult.ImpactPoint;
				MouseHitActor = HitResult.GetActor();
			}
		}
	}
	if (TScriptInterface<ICombatInterface> CombatInterface = MouseHitActor)
	{
		if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::OnPrimaryTargetActorDied))
		{
			CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::OnPrimaryTargetActorDied);
		}
	}
	
}

void UAuraBeamSpell::StoreAdditionalTargets(TArray<AActor*>& NearbyTargets)
{
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(OwnerCharacter);
	IgnoreActors.Add(MouseHitActor);
	TArray<AActor*> FoundTargets;
	UAuraAbilitySystemLibrary::GetLiveEntitiesWithinRadius(GetAvatarActorFromActorInfo(), MouseHitActor->GetActorLocation(), BeamRadius, FoundTargets, IgnoreActors);

	//int32 NumAdditionalTargets = FMath::Min(GetAbilityLevel()-1, MaxShockTargets);
	int32 NumAdditionalTargets = 5;
	UAuraAbilitySystemLibrary::GetClosestTargets(NumAdditionalTargets, FoundTargets, NearbyTargets, MouseHitActor->GetActorLocation());


	for (auto Target : FoundTargets)
	{
		if (TScriptInterface<ICombatInterface> CombatInterface = Target)
		{
			if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::OnAdditionalTargetActorDied))
			{
				CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::OnAdditionalTargetActorDied);
			}
		}
	}
	
}
