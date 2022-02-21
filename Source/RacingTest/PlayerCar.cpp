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

	SpringArm->SetupAttachment(PlayerMesh);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	Camera->AddLocalOffset(FVector(0.f, 0.f, 0.f));
	Camera->bUsePawnControlRotation = false;

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

	// 3D Car Movement
	this->AddActorLocalOffset(FVector(XValue, YValue, 0.f));

	this->AddActorLocalRotation(FRotator(0.f, YaValue, 0.f));
	SpringArm->AddLocalRotation(FRotator(0.f, YaValue, 0.f));
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

	PlayerInputComponent->BindAction("Dash", EInputEvent::IE_Pressed, this, &APlayerCar::Dash);
}

//3D Car Movement
void APlayerCar::Accelerate(float Value)
{
	if (XValue > ForwardSpeed) { XValue = ForwardSpeed; }
	else if (XValue < -ForwardSpeed) { XValue = -ForwardSpeed; }
	else { XValue += Value; }
}

void APlayerCar::Yaw(float Value)
{
	YaValue = Value;
}

void APlayerCar::Dash()
{

}
