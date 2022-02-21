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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerMesh")
		UStaticMeshComponent* PlayerMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerMesh")
		USpringArmComponent* SpringArm = { nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerMesh")
		UCameraComponent* Camera = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerMesh")
		float Speed = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerMesh")
		float ForwardSpeed = 1.f;


private:
	FVector InitLocation = FVector::ZeroVector;

	//3D Car Movement
	void Accelerate(float Value);
	void Yaw(float Value);

	void Dash();

	float XValue = 0.f;
	float YValue = 0.f;
	float ZValue = 0.f;

	float YaValue = 0.f;

	float DashTimer = 0.f;
};
