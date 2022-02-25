// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCar.h"
#include "GameFramework/PlayerInput.h"
#include "Components/InputComponent.h"
#include "Bullet.h"
#include "Coin.h"
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

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	CollisionBox->SetGenerateOverlapEvents(true);
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &APlayerCar::OnOverlap);
	SetRootComponent(CollisionBox);

	PlayerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerMesh"));
	PlayerMesh->SetupAttachment(RootComponent);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetUsingAbsoluteRotation(true);
	SpringArm->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
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

		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Nitro", EKeys::E));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Shoot", EKeys::SpaceBar));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Reload", EKeys::R));
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
		XValue -= Acceleration / 2;
	}
	else if (XValue < 0) {
		XValue += Acceleration / 2;
	}

	// 3D Car Movement
	this->AddActorLocalOffset(FVector(XValue, 0.f, 0.f));
	this->AddActorLocalRotation(FRotator(0, YawValue, 0));
	
	// Limit Springarm X-Rotation
	if (YCamera <= -70) {
		YCamera = -70;
	}
	if (YCamera >= -1) {
		YCamera = -1;
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
	PlayerInputComponent->BindAction("Shoot", EInputEvent::IE_Pressed, this, &APlayerCar::Shoot);
	PlayerInputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &APlayerCar::Reload);
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
		YawValue = Value * TurnAmt;
	}
	else if (XValue < 0) {
		YawValue = -Value * TurnAmt;
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



void APlayerCar::Shoot()
{
	Ammo--;
	
	if (Ammo > 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("Ammo :  %d "), Ammo));

		UWorld* World = GetWorld();
		if (World)
		{
			// "Shotgun"
			// Subject to change

			FVector Location = GetActorLocation();
			FRotator Rotation = GetActorRotation();
			World->SpawnActor<AActor>(ActorToSpawn, Location + FVector(140.f, 0.f, 80.f), Rotation + FRotator(0.f, -4.f, 0.f));
			World->SpawnActor<AActor>(ActorToSpawn, Location + FVector(140.f, 0.f, 80.f), Rotation + FRotator(0.f, -2.f, 0.f));
			World->SpawnActor<AActor>(ActorToSpawn, Location + FVector(140.f, 0.f, 80.f), Rotation + FRotator(0.f, 0.f, 0.f));
			World->SpawnActor<AActor>(ActorToSpawn, Location + FVector(140.f, 0.f, 80.f), Rotation + FRotator(0.f, 2.f, 0.f));
			World->SpawnActor<AActor>(ActorToSpawn, Location + FVector(140.f, 0.f, 80.f), Rotation + FRotator(0.f, 4.f, 0.f));
			UGameplayStatics::PlaySound2D(World, Shooting, 1.f, 1.f, 0.f, 0);
		}
	}

	if (Ammo <= 0)
	{
		Ammo = 0;
		UWorld* World = GetWorld();
		if (World)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("No ammo Reload %d "), Ammo));
			UGameplayStatics::PlaySound2D(World, OutOfAmmo, 1.f, 1.f, 0.f, 0);
		}
	}

		UE_LOG(LogTemp, Warning, TEXT("Shooting"));
}

void APlayerCar::Reload() {
	Ammo = 6;
	UWorld* NewWorld = GetWorld();
	UGameplayStatics::PlaySound2D(NewWorld, Reloading, 1.f, 1.f, 0.f, 0);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Reloaded %d "), Ammo));
}

void APlayerCar::Nitro()
{

}

void APlayerCar::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ACoin::StaticClass()))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, FString::Printf(TEXT("Player Picked Up Coin"), Ammo));
		UE_LOG(LogTemp, Warning, TEXT("Player Picked Up Coin"))
			OtherActor->Destroy();
	}
}
