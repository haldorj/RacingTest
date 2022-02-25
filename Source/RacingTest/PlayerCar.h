// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "PlayerCar.generated.h"

UCLASS()
class RACINGTEST_API APlayerCar : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerCar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY(VisibleAnywhere)
		UShapeComponent* CollisionBox = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerVariables")
		UStaticMeshComponent* PlayerMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerVariables")
		USpringArmComponent* SpringArm = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerVariables")
		UCameraComponent* Camera = nullptr;

	UPROPERTY(EditAnywhere, Category = "PlayerVariables")
		USoundBase* Shooting = nullptr;

	UPROPERTY(EditAnywhere, Category = "PlayerVariables")
		USoundBase* OutOfAmmo = nullptr;

	UPROPERTY(EditAnywhere, Category = "PlayerVariables")
		USoundBase* Reloading = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerVariables")
		float Acceleration = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerVariables")
		float MaxSpeed = 12.f;

// For spawning Bullets:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"));
	TSubclassOf<AActor> ActorToSpawn;
//

private:
	FVector InitLocation = FVector::ZeroVector;

	// 3D Car Movement
	void Accelerate(float Value);
	void Yaw(float Value);

	// Camera movement
	void XView(float Value);
	void YView(float Value);
	
	// Functions
	void Shoot();
	void Reload();
	void Nitro();

	// how sharp the car turns (Multiplier)
	int TurnAmt = 2;

	float XValue = 0.f;
	float YawValue = 0.f;
	float CarFacingDirection;

	float XCamera = 0.f;
	float YCamera = 0.f;

	int Ammo = 6;

	float NitroTimer = 0.f;

	// For interacting with other classes / collision.
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
			const FHitResult& SweepResult);
};
