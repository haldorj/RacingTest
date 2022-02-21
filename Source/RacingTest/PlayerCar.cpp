// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCar.h"
#include "GameFramework/PlayerInput.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraActor.h"
#include "Engine/Engine.h"


// Sets default values
APlayerCar::APlayerCar()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PlayerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerMesh"));
	SetRootComponent(PlayerMesh);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetUsingAbsoluteRotation(true);
	SpringArm->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	SpringArm->TargetArmLength = 1000.f;
	SpringArm->bEnableCameraLag = false;
	SpringArm->CameraLagSpeed = 5.f;

	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

}

static void InitializeDefaultPawnInputBinding()
{
	static bool bindingsAdded = false;

	if (bindingsAdded == false)
	{
		bindingsAdded = true;

		// 3D Car Movement
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Accelerate", EKeys::W, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Accelerate", EKeys::S, -1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Yaw", EKeys::D, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Yaw", EKeys::A, -1.f));
	
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("XView", EKeys::MouseX));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("YView", EKeys::MouseY));

		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Dash", EKeys::E));
	}
}

// Called when the game starts or when spawned
void APlayerCar::BeginPlay()
{
	Super::BeginPlay();
	InitLocation = PlayerMesh->GetComponentLocation();
}

// Called every frame
void APlayerCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// De-acceleration 
	if (XValue > 0) {
		XValue -= Acceleration / 8;
	}
	else if (XValue < 0) {
		XValue += Acceleration / 8;
	}

	// 3D Car Movement
	this->AddActorLocalOffset(FVector(XValue, 0.f, 0.f));
	this->AddActorLocalRotation(FRotator(0, YaValue, 0));
	
	// Limit Springarm X-Rotation
	if (YCamera <= -80) {
		YCamera = -80;
	}
	if (YCamera >= 0) {
		YCamera = 0;
	}

	SpringArm->SetRelativeRotation(FRotator(YCamera, XCamera, 0));
}

// Called to bind functionality to input
void APlayerCar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent != nullptr);
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InitializeDefaultPawnInputBinding();

	// 3D Car Movement
	PlayerInputComponent->BindAxis("Accelerate", this, &APlayerCar::Accelerate);
	PlayerInputComponent->BindAxis("Yaw", this, &APlayerCar::Yaw);

	PlayerInputComponent->BindAxis("XView", this, &APlayerCar::XView);
	PlayerInputComponent->BindAxis("YView", this, &APlayerCar::YView);

	PlayerInputComponent->BindAction("Nitro", EInputEvent::IE_Pressed, this, &APlayerCar::Nitro);
}

//3D Car Movement
void APlayerCar::Accelerate(float Value)
{
	
	if (XValue > MaxSpeed) { XValue = MaxSpeed; }
	else if (XValue < -MaxSpeed) { XValue = -MaxSpeed; }
	else { XValue += Value * Acceleration; }
}

// Car Turning
void APlayerCar::Yaw(float Value)
{
	if (XValue > 0) {
		YaValue = Value * TurnAmt;
	}
	else if (XValue < 0) {
		YaValue = -Value * TurnAmt;
	}
}

void APlayerCar::XView(float Value)
{
	XCamera += Value;
}

void APlayerCar::YView(float Value)
{
	YCamera -= Value;
}



void APlayerCar::Nitro()
{

}
