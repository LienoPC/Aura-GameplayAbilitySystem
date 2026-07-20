#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SaveInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USaveInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * All actors that implement this class are saved in the world state
 */
class AURA_API ISaveInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void LoadActor();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool ShouldLoadTransform() const;
};
