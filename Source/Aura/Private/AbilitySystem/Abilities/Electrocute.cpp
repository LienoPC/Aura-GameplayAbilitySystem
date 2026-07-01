// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Electrocute.h"

FString UElectrocute::GetDescription(int32 Level)
{

	const int32 DamageVal = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			// Title
			"<Title>ELECTROCUTE</>\n\n"

			// Details
			"<Small>Level: </><Level>%d</>\n"
			// Mana Cost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			
			// Description
			"<Default>Launches a beam of lightning, "
			"connecting with the target repeatedly causing "
			"</><Damage>%d</><Default> lightning "
			"damage and a chance to stun.</>\n\n"
			),
			// Values
			Level, ManaCost, Cooldown, DamageVal);
	}
	else
	{
		return FString::Printf(TEXT(
					// Title
					"<Title>ELECTROCUTE</>\n\n"

					// Details
					"<Small>Level: </><Level>%d</>\n"
					"<Small>ManaCost: </><ManaCost>%.1f</>\n"
					// Cooldown
					"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
					
					// Description
					"<Default> Emits a beam of lightning propagating to %d additional targets with "
					"</><Damage>%d</><Default> lightning "
					"damage and a chance to stun.</>\n\n"
					),
					// Values
					Level, ManaCost, Cooldown, FMath::Min(Level, MaxShockTargets), DamageVal);
	}
	
}

FString UElectrocute::GetNextLevelDescription(int32 Level)
{
	const int32 DamageVal = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);

	return FString::Printf(TEXT(
	// Title
	"<Title>ELECTROCUTE</>\n\n"

	// Details
	"<Small>Level: </><Level>%d</>\n"
	"<Small>ManaCost: </><ManaCost>%.1f</>\n"
	// Cooldown
	"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
	
	// Description
	"<Default> Emits a beam of lightning propagating to %d additional targets with "
	"</><Damage>%d</><Default> lightning "
	"damage and a chance to stun.</>\n\n"
	),
	// Values
	Level, ManaCost, Cooldown, FMath::Min(Level, MaxShockTargets), DamageVal);

}
