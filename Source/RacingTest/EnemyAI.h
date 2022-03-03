// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyAI.generated.h"

UCLASS()
class RACINGTEST_API AEnemyAI : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input

	// Don't need for AI:
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


public:
	// Sphere that will detect the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
		class USphereComponent* EnemySensingSphere = nullptr;

	// The Controller for the NPC - similar to the PlayerController
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
		class AAIController* AIController = nullptr;

	// Enemy Mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerVariables")
		UStaticMeshComponent* EnemyMesh = nullptr;

	//Function that actually moves the NPC
	//NPC moves towards the PlayerCar - Chase mode
	void MoveToTarget(class APlayerCar* APlayerCar);

	//

	// Colission / interacting with other classes
	UFUNCTION()
		void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);
};