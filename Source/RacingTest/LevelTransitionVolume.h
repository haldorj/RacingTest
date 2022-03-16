// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelTransitionVolume.generated.h"

UCLASS()
class RACINGTEST_API ALevelTransitionVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelTransitionVolume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transtition")
	class UBoxComponent* TransitionVolume;

	class UBillboardComponent* Billboard;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transtition")
	FName TransitionLevelName;

	// Colission / interacting with other classes
	UFUNCTION()
		void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult& SweepResult);

};
