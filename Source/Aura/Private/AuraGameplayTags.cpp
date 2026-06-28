// Fill out your copyright notice in the Description page of Project Settings.


#include "AuraGameplayTags.h"

#include "GameplayTagsManager.h"

FAuraGameplayTags FAuraGameplayTags::Instance;

void FAuraGameplayTags::InitializeNativeGameplayTags()
{
	/*
	 * Primary Attributes
	 */
	Instance.Attributes_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Strength"), FString("Increases physical damage")
		);

	Instance.Attributes_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Intelligence"), FString("Increases magical damage")
		);

	Instance.Attributes_Primary_Resilience = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Resilience"), FString("Increases armor and armor penetration")
		);

	Instance.Attributes_Primary_Vigor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Vigor"), FString("Increases Health")
		);

	/*
	 * Secondary Attributes
	 */
	Instance.Attributes_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.Armor"), FString("Reduces damage taken, improves Block Chance")
		);

	Instance.Attributes_Secondary_ArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.ArmorPenetration"), FString("Increases armor penetration")
		);
	Instance.Attributes_Secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.BlockChance"), FString("Increases chance of blocking attacks")
		);
	
	Instance.Attributes_Secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitChance"), FString("Increases chance of critic")
		);

	Instance.Attributes_Secondary_CriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitDamage"), FString("Increases critic damage")
		);

	Instance.Attributes_Secondary_CriticalHitResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitResistance"), FString("Increases critic hit resistance")
		);

	Instance.Attributes_Secondary_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.HealthRegeneration"), FString("Increases health regeneration")
		);

	Instance.Attributes_Secondary_ManaRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.ManaRegeneration"), FString("Increases mana regeneration")
		);

	Instance.Attributes_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxHealth"), FString("Increases max Health")
		);

	Instance.Attributes_Secondary_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxMana"), FString("Increases max Mana")
		);

	/**
	 * Meta Attributes
	 */

	Instance.Attributes_Meta_IncomingXP = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Meta.IncomingXP"), FString("Incoming XP meta attribute")
			);

	
	/*
	 * Input Tags
	 */
	Instance.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.LMB"), FString("Input Tag for Left Mouse Button")
		);

	Instance.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.RMB"), FString("Input Tag for Right Mouse Button")
		);

	Instance.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.1"), FString("Input Tag for One")
		);

	Instance.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.2"), FString("Input Tag for Two")
		);
	
	Instance.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.3"), FString("Input Tag for Three")
		);
	
	Instance.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.4"), FString("Input Tag for Four")
		);

	Instance.InputTag_Passive_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Passive.1"), FString("Input Tag for Passive 1")
	);

	Instance.InputTag_Passive_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Passive.2"), FString("Input Tag for Passive 2")
	);

	/*
	 * Damage Types
	 */
	Instance.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage"), FString("Identifies a damage response")
	);

	Instance.Damage_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Damage.Fire"), FString("Fire damage type")
			);

	Instance.Damage_Lightining = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Damage.Lightning"), FString("Lightning damage type")
			);

	Instance.Damage_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Damage.Arcane"), FString("Arcane damage type")
			);

	Instance.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Damage.Physical"), FString("Physical damage type")
			);
	/*
	 * Resistances
	 */

	Instance.Attributes_Resistance_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Resistance.Fire"), FString("Resistance to fire damage")
			);

	Instance.Attributes_Resistance_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Resistance.Lightning"), FString("Resistance to lightning damage")
		);

	Instance.Attributes_Resistance_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Resistance.Arcane"), FString("Resistance to arcane damage")
		);

	Instance.Attributes_Resistance_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Resistance.Physical"), FString("Resistance to physical damage")
		);

	Instance.DamageTypesToResistances.Add(Instance.Damage_Fire, Instance.Attributes_Resistance_Fire);
	Instance.DamageTypesToResistances.Add(Instance.Damage_Lightining, Instance.Attributes_Resistance_Lightning);
	Instance.DamageTypesToResistances.Add(Instance.Damage_Arcane, Instance.Attributes_Resistance_Arcane);
	Instance.DamageTypesToResistances.Add(Instance.Damage_Physical, Instance.Attributes_Resistance_Physical);


	/*
	 * Debuffs
	 */
	Instance.Debuff_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Arcane"), FString("Debuff for arcane damage")
		);

	Instance.Debuff_Burn = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Burn"), FString("Debuff for fire damage")
		);

	Instance.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Physical"), FString("Debuff for physical damage")
		);

	Instance.Debuff_Stun = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Stun"), FString("Debuff for lightning damage")
		);

	Instance.DamageTypesToDebuffs.Add(Instance.Damage_Arcane, Instance.Debuff_Arcane);
	Instance.DamageTypesToDebuffs.Add(Instance.Damage_Lightining, Instance.Debuff_Stun);
	Instance.DamageTypesToDebuffs.Add(Instance.Damage_Fire, Instance.Debuff_Burn);
	Instance.DamageTypesToDebuffs.Add(Instance.Damage_Physical, Instance.Debuff_Physical);
	
	Instance.Debuff_Chance = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Debuff.Chance"), FString("Debuff chance")
	);

	Instance.Debuff_Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Debuff.Damage"), FString("Debuff damage")
	);

	Instance.Debuff_Duration = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Debuff.Duration"), FString("Debuff duration")
	);

	Instance.Debuff_Frequency = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Debuff.Frequency"), FString("Debuff frequency")
	);
		
	
	/*
	 * Abilities
	 */
	Instance.Abilities_None = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.None"), FString("None ability")
		);

	Instance.Abilities_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Attack"), FString("General attack ability")
		);

	Instance.Abilities_Summon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Summon"), FString("Summon attack ability")
		);

	Instance.Abilities_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.HitReact"), FString("Tag granted when hit reacting")
		);

	/**
	 * Offensive spells
	 */

	Instance.Abilities_Fire_FireBolt = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Fire.FireBolt"), FString("Firebolt ability")
		);

	Instance.Abilities_Lightning_Electrocute = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Lightning.Electrocute"), FString("Electrocute ability")
		);
	Instance.Abilities_Arcane_ArcaneShards = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Arcane.ArcaneShards"), FString("Arcane Shards ability")
		);

	
	Instance.Abilities_Status_Locked = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Status.Locked"), FString("Abilities that are locked (Player has not reached yet the required level)")
		);

	Instance.Abilities_Status_Eligible = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Status.Eligible"), FString("Abilities that are available")
		);

	Instance.Abilities_Status_Unlocked = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Status.Unlocked"), FString("Abilities that are available and unlocked by the player")
		);

	Instance.Abilities_Status_Equipped = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Status.Equipped"), FString("Abilities that are equipped and usable")
		);
	
	Instance.Abilities_Type_Offensive = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Type.Offensive"), FString("Abilities that are actively triggered")
		);

	Instance.Abilities_Type_Passive = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Type.Passive"), FString("Abilities that are always active as passive")
		);
	
	Instance.Abilities_Type_None = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Type.None"), FString("Generic abilities")
		);

	/**
	 * Passive spells
	 */

	Instance.Abilities_Passive_LifeSiphon = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Passive.LifeSiphon"), FString("Life siphon ability")
	);
	
	Instance.Abilities_Passive_ManaSiphon = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Passive.ManaSiphon"), FString("Mana siphon ability")
	);

	Instance.Abilities_Passive_HaloOfProtection = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Passive.HaloOfProtection"), FString("Mana siphon ability")
	);
/**
 * Cooldowns
 */
	Instance.Cooldown_Fire_FireBolt = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Cooldown.Fire.FireBolt"), FString("Firebolt ability cooldown")
	);
	

	Instance.Cooldown_Lightning_Electrocute = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Cooldown.Lightning.Electrocute"), FString("Electrocute ability cooldown")
	);
	
	/*
	 * Effects
	 */

	/**
	 * Montages
	 */
	Instance.CombatSocket_Attack_Weapon = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("CombatSocket.Weapon"), FString("Weapon attack")
	);

	Instance.CombatSocket_Attack_RightHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("CombatSocket.RightHand"), FString("Right hand attack")
	);

	Instance.CombatSocket_Attack_LeftHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("CombatSocket.LeftHand"), FString("Left hand attack")
	);

	Instance.CombatSocket_Attack_Ranged = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("CombatSocket.Ranged"), FString("Ranged attack")
	);
	
	Instance.Montage_Attack_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.1"), FString("Attack 1")
		);

	Instance.Montage_Attack_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Montage.Attack.2"), FString("Attack 2")
	);

	Instance.Montage_Attack_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Montage.Attack.3"), FString("Attack 3")
	);

	Instance.Montage_Attack_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Montage.Attack.4"), FString("Attack 4")
	);

	Instance.Montage_Attack_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Montage.Attack.5"), FString("Attack 5")
	);

	/**
	 * Player Tags
	 */
	Instance.Player_Block_CursorTrace = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Player.Block.CursorTrace"), FString("Block tracing under the cursor.")
	);
	
	Instance.Player_Block_InputHeld = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Player.Block.InputHeld"), FString("Block input held callback for input.")
	);

	Instance.Player_Block_InputPressed = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Player.Block.InputPressed"), FString("Block input pressed callback for input.")
	);
	
	Instance.Player_Block_InputReleased = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Player.Block.InputReleased"), FString("Block input released callback for input.")
	);
	
	
	UGameplayTagsManager::Get().DoneAddingNativeTags();
}
