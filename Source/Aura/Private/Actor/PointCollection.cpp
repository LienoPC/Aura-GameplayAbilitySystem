// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/PointCollection.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

APointCollection::APointCollection()
{
	PrimaryActorTick.bCanEverTick = false;

}

TArray<USceneComponent*> APointCollection::GetGroundPoints(const FVector& GroundLocation, const int32 NumPoints,
	const float YawOverride)
{

	
	checkf(ImmutablePts.Num() >= NumPoints, TEXT("Attemped to access ImmutablePts out of bounds."));
	TArray<USceneComponent*> Points;
	if (ImmutablePts.Num() == 0)
		return Points;
	for (auto Pt : ImmutablePts)
	{
		if (Points.Num() >= NumPoints) return Points;

		if (Pt != ImmutablePts[0])
		{
			FVector ToPoint = Pt->GetComponentLocation() - GetActorLocation();
			ToPoint = ToPoint.RotateAngleAxis(YawOverride, FVector::UpVector);
			Pt->SetWorldLocation(GetActorLocation() + ToPoint);
		}

		const FVector RaisedLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, Pt->GetComponentLocation().Z + 250.0f);
		const FVector LowerLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, Pt->GetComponentLocation().Z - 250.0f);

		FHitResult HitResult;
		TArray<AActor*> IgnoreActors;
		UAuraAbilitySystemLibrary::GetLiveEntitiesWithinRadius(this, GetActorLocation(), 1500.0f , IgnoreActors, TArray<AActor*>());

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActors(IgnoreActors);
		GetWorld()->LineTraceSingleByProfile(HitResult, RaisedLocation, LowerLocation, FName("BlockAll"), QueryParams);

		const FVector AdjustedLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, HitResult.ImpactPoint.Z);

		Pt->SetWorldLocation(AdjustedLocation);
		Pt->SetWorldRotation(UKismetMathLibrary::MakeRotFromZ(HitResult.ImpactNormal));

		Points.Add(Pt);
	}

	return Points;
}

void APointCollection::BeginPlay()
{
	Super::BeginPlay();
	TArray<UActorComponent*> FoundComponents = GetComponentsByTag(USceneComponent::StaticClass(), FName("Point"));

	for (UActorComponent* Comp : FoundComponents)
	{
		if (USceneComponent* SceneComp = Cast<USceneComponent>(Comp))
		{
			ImmutablePts.Add(SceneComp);
		}
	}
}


