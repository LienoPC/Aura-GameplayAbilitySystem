// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AuraGameplayTags.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
	const int32 DamageVal = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			// Title
			"<Title>FIRE BOLT</>\n\n"

			// Details
			"<Small>Level: </><Level>%d</>\n"
			// Mana Cost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			
			// Description
			"<Default>Launches a bolt of fire, "
			"exploding on impact and dealing: "
			"</><Damage>%d</><Default> fire "
			"damage with a chance to burn</>\n\n"
			),
			// Values
			Level, ManaCost, Cooldown, DamageVal);
	}
	else
	{
		return FString::Printf(TEXT(
					// Title
					"<Title>FIRE BOLT</>\n\n"

					// Details
					"<Small>Level: </><Level>%d</>\n"
					"<Small>ManaCost: </><ManaCost>%.1f</>\n"
					// Cooldown
					"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
					
					// Description
					"<Default>Launches %d a bolt of fire, "
					"exploding on impact and dealing: "
					"</><Damage>%d</><Default> fire "
					"damage with a chance to burn</>\n\n"
					),
					// Values
					Level, ManaCost, Cooldown, FMath::Min(Level, NumProjectiles), DamageVal);
	}
}

FString UAuraFireBolt::GetNextLevelDescription(int32 Level)
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
						"<Default>Launches %d a bolt of fire, "
						"exploding on impact and dealing: "
						"</><Damage>%d</><Default> fire "
						"damage with a chance to burn</>\n\n"
						),
						// Values
						Level, ManaCost, Cooldown, FMath::Min(Level, NumProjectiles), DamageVal);}