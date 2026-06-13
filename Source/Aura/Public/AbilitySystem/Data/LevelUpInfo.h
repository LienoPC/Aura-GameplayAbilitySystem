// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelUpInfo.generated.h"

USTRUCT(BlueprintType)
struct FLevelInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	int32 Level;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level")
	int32 LevelUpRequirement = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level")
	int32 AttributePointReward = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level")
	int32 SpellPointReward = 0;
};

/**
 * Contains info for all levels
 */
UCLASS()
class AURA_API ULevelUpInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	ULevelUpInfo();
	virtual void PostLoad() override;
	
	int32 LevelAtXP(int32 XP) const;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level")
	TArray<FLevelInfo> Levels;
protected:


#if WITH_EDITOR
virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
};
