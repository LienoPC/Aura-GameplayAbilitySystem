// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/SummonAbility.h"

#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> USummonAbility::GetSpawnLocations()
{

	TArray<FVector> SpawnLocations;

	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	
	const FVector RightOfSpread = Forward.RotateAngleAxis(-SpawnSpread/2.f, FVector::UpVector);
	const float DeltaSpread = SpawnSpread/NumMinions;

	for (int32 i = 0; i < NumMinions; i++)
	{
		const FVector Direction = RightOfSpread.RotateAngleAxis(DeltaSpread*i, FVector::UpVector);
		FVector SpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);
		//DrawDebugSphere(GetWorld(), SpawnLocation, 15, 12.0f, FColor::Red, false, 2.0f);

		//UKismetSystemLibrary::DrawDebugArrow(this, Location, Location + Direction*MaxSpawnDistance, 4.f, FLinearColor::Red, 1.0f);

		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, SpawnLocation + FVector::UpVector*500.f, SpawnLocation + FVector::UpVector*-500.f, ECC_Visibility);
		if (Hit.bBlockingHit)
		{
			SpawnLocation = Hit.ImpactPoint;
		}
		SpawnLocations.Add(SpawnLocation);
	}
	
	return SpawnLocations;
}

