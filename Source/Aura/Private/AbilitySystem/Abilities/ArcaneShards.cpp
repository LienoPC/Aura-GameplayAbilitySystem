// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/ArcaneShards.h"

FString UArcaneShards::GetDescription(int32 Level)
{
	const int32 DamageVal = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			// Title
			"<Title>ARCANE SHARDS</>\n\n"

			// Details
			"<Small>Level: </><Level>%d</>\n"
			// Mana Cost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			
			// Description
			"<Default> Summon shards of arcane energy, causing radial damage of "
			"</><Damage>%d</><Default> arcane "
			"damage</>\n\n"
			),
			// Values
			Level, ManaCost, Cooldown, DamageVal);
	}
	else
	{
		return FString::Printf(TEXT(
					// Title
					"<Title>ARCANE SHARDS</>\n\n"

					// Details
					"<Small>Level: </><Level>%d</>\n"
					"<Small>ManaCost: </><ManaCost>%.1f</>\n"
					// Cooldown
					"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
					
					// Description
					"<Default> Summon %d shards of arcane energy, causing radial damage of "
					"</><Damage>%d</><Default> arcane "
					"damage</>\n\n"
					),
					// Values
					Level, ManaCost, Cooldown, FMath::Min(Level, MaxNumShards), DamageVal);
	}
	
}

FString UArcaneShards::GetNextLevelDescription(int32 Level)
{
	const int32 DamageVal = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
	// Title
	"<Title>NEXT LEVEL</>\n\n"

	// Details
	"<Small>Level: </><Level>%d</>\n"
	"<Small>ManaCost: </><ManaCost>%.1f</>\n"
	// Cooldown
	"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
	
	// Description
	"<Default> Summon %d shards of arcane energy, causing radial damage of "
	"</><Damage>%d</><Default> arcane "
	"damage</>\n\n"
	),
	// Values
	Level, ManaCost, Cooldown, FMath::Min(Level, MaxNumShards), DamageVal);


}
