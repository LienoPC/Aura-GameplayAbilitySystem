// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraEnemySpawnPoint.h"

#include "Characters/Enemy.h"

void AAuraEnemySpawnPoint::SpawnEnemy()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AEnemy* Enemy = GetWorld()->SpawnActorDeferred<AEnemy>(EnemyClass, GetActorTransform());

	Enemy->SetLevel(EnemyLevel);
	Enemy->SetCharacterClass(CharacterClass);

	Enemy->FinishSpawning(GetActorTransform());
	Enemy->SpawnDefaultController();
}
