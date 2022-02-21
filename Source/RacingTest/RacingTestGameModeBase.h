// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCar.h"
#include "GameFramework/GameModeBase.h"
#include "RacingTestGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class RACINGTEST_API ARacingTestGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<class APlayerCar> PlayerBP;
};
