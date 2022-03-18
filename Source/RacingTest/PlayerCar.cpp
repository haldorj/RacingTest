// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCar.h"
#include "GameFramework/PlayerInput.h"
#include "Components/InputComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Bullet.h"
#include "Coin.h"
#include "HealthPack.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Camera/CameraActor.h"
#include "Engine/Engine.h"


// Sets default values
APlayerCar::APlayerCar()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlayerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerMesh"));
	SetRootComponent(PlayerMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	CollisionBox->SetGenerateOverlapEvents(true);
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &APlayerCar::OnOverlap);
	CollisionBox->SetupAttachment(RootComponent);

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

	Ammo = 6;
	MaxAmmo = 6;
	Health = 25.f;
	MaxHealth = 100.f;
	Coins = 0;
}

static void InitializeDefaultPawnInputBinding()
{
	static bool bindingsAdded = false;

	if (bindingsAdded == false)
	{
		bindingsAdded = true;

		// 3D Car Movement
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Forward", EKeys::W, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Forward", EKeys::S, -1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Sideways", EKeys::D, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Sideways", EKeys::A, -1.f));
	
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
	
	// 3D Car Movement
	this->SetActorRelativeRotation(FRotator(0, YawValue, 0));

	//this->SetActorLocation(FVector(XValue, YValue, 150.f));
	
	// Limit Springarm X-Rotation
	if (YCamera <= -70) {
		YCamera = -70;
	}
	if (YCamera >= -1) {
		YCamera = -1;
	}

	SpringArm->SetRelativeRotation(FRotator(YCamera, XCamera, 0));

	// Movement Info
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("---------------------------------")));
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("XSpeed :  %d "), static_cast<int>(XSpeed)));
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("YSpeed :  %d "), static_cast<int>(YSpeed)));
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("YawValue :  %d "), static_cast<int>(YawValue)));
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("YawRad :  %d "), static_cast<int>(YawRad)));
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("XValue :  %d "), static_cast<int>(XValue)));
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("YValue :  %d "), static_cast<int>(YValue)));
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("---------------------------------")));
}

// Called to bind functionality to input
void APlayerCar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent != nullptr);
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InitializeDefaultPawnInputBinding();

	// 3D Car Movement
	PlayerInputComponent->BindAxis("Forward", this, &APlayerCar::Forward);
	PlayerInputComponent->BindAxis("Sideways", this, &APlayerCar::Sideways);

	PlayerInputComponent->BindAxis("XView", this, &APlayerCar::XView);
	PlayerInputComponent->BindAxis("YView", this, &APlayerCar::YView);

	PlayerInputComponent->BindAction("Nitro", EInputEvent::IE_Pressed, this, &APlayerCar::Nitro);
	PlayerInputComponent->BindAction("Shoot", EInputEvent::IE_Pressed, this, &APlayerCar::Shoot);
	PlayerInputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &APlayerCar::Reload);
}

void APlayerCar::Force()
{

}

//3D Hover"Car" Movement
void APlayerCar::Forward(float Value)
{
	YawRad = YawValue * (PI / 180);
	XSpeed = (Acceleration * Value) * cos(YawRad);
	YSpeed = (Acceleration * Value) * sin(YawRad);

	if (XSpeed > MaxSpeed) {
		XSpeed = MaxSpeed;
	}	
	else if (XSpeed < -MaxSpeed) {
		XSpeed = -MaxSpeed;
	}	
	
	if (YSpeed > MaxSpeed) {
		YSpeed = MaxSpeed;
	}	
	else if (YSpeed < -MaxSpeed) {
		YSpeed = -MaxSpeed;
	}
	
	
	PlayerMesh->AddImpulse(FVector(XSpeed, YSpeed, 0.f));
}

void APlayerCar::Sideways(float Value) 
{
	YawRad = YawValue * (PI / 180);
	XSpeed = (Acceleration * Value) * -sin(YawRad);
	YSpeed = (Acceleration * Value) * cos(YawRad);

	if (XSpeed > MaxSpeed) {
		XSpeed = MaxSpeed;
	}
	else if (XSpeed < -MaxSpeed) {
		XSpeed = -MaxSpeed;
	}

	if (YSpeed > MaxSpeed) {
		YSpeed = MaxSpeed;
	}
	else if (YSpeed < -MaxSpeed) {
		YSpeed = -MaxSpeed;
	}

	PlayerMesh->AddImpulse(FVector(XSpeed, YSpeed, 0.f));
}

//// Car Turning
//void APlayerCar::Yaw(float Value)
//{
//	if (XValue > 0) {
//		YawValue = Value * TurnAmt;
//	}
//	else if (XValue < 0) {
//		YawValue = -Value * TurnAmt;
//	}
//}

void APlayerCar::XView(float Value)
{
	XCamera += Value * TurnAmt;
	YawValue += Value * TurnAmt;
}

void APlayerCar::YView(float Value)
{
	YCamera -= Value;
}

void APlayerCar::Shoot()
{	
	if (Ammo > 0)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			// "Shotgun"
			// Subject to change

			FVector Location = GetActorLocation();
			FRotator Rotation = GetActorRotation();
			float X = 140.f * cos(YawRad);
			float Y = 140.f * sin(YawRad);
			World->SpawnActor<AActor>(ActorToSpawn, Location + FVector(X, Y, 80.f), Rotation + FRotator(0.f, -4.f, 0.f));
			World->SpawnActor<AActor>(ActorToSpawn, Location + FVector(X, Y, 80.f), Rotation + FRotator(0.f, -2.f, 0.f));
			World->SpawnActor<AActor>(ActorToSpawn, Location + FVector(X, Y, 80.f), Rotation + FRotator(0.f, 0.f, 0.f));
			World->SpawnActor<AActor>(ActorToSpawn, Location + FVector(X, Y, 80.f), Rotation + FRotator(0.f, 2.f, 0.f));
			World->SpawnActor<AActor>(ActorToSpawn, Location + FVector(X, Y, 80.f), Rotation + FRotator(0.f, 4.f, 0.f));
			UGameplayStatics::PlaySound2D(World, Shooting, 1.f, 1.f, 0.f, 0);
		}
	}

	Ammo--;
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("Ammo :  %d "), Ammo));

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
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, FString::Printf(TEXT("Player Picked Up Coin")));
		UE_LOG(LogTemp, Warning, TEXT("Player Picked Up Coin"))
			OtherActor->Destroy();
		Coins++;
	}
	if (OtherActor->IsA(AHealthPack::StaticClass()))
	{
		Health += 20;

		if (Health > MaxHealth)
		{
			Health = MaxHealth;
		}
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("Player Picked Up Health %f"), Health));
		UE_LOG(LogTemp, Warning, TEXT("Player Picked Up Health %f "), Health)
			OtherActor->Destroy();
	}
}

void APlayerCar::SwitchLevel(FName LevelName)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FString CurrentLevel = World->GetMapName();

		FName CurrentLevelName(*CurrentLevel);
		if (CurrentLevelName != LevelName)
		{
			UGameplayStatics::OpenLevel(World, LevelName);
		}

	}
}

