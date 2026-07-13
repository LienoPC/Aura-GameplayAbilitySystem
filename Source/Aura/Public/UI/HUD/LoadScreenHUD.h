// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LoadScreenHUD.generated.h"

class UMVVM_LoadScreen;
class ULoadScreenWidget;
/**
 * HUD Class for LoadScreen which enforces MVVM pattern.
 */
UCLASS()
class AURA_API ALoadScreenHUD : public AHUD
{
	GENERATED_BODY()

public:
	/**
	 * Class of the user widget of load screen
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ULoadScreenWidget> LoadScreenWidgetClass;

	/**
	 * Instance of the load screen widget
	 */
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ULoadScreenWidget> LoadScreenWidget;


	/**
	 * ViewModel type class
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UMVVM_LoadScreen> LoadScreenViewModelClass;

	/**
	 * ViewModel instance
	 */
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMVVM_LoadScreen> LoadScreenViewModel;
protected:

	virtual void BeginPlay() override;
};
