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
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Engine.h"


// Sets default values
APlayerCar::APlayerCar()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	//SetRootComponent(Root);

	PlayerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerMesh"));
	SetRootComponent(PlayerMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	CollisionBox->SetGenerateOverlapEvents(true);
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &APlayerCar::OnOverlap);
	CollisionBox->SetupAttachment(PlayerMesh);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetUsingAbsoluteRotation(true);
	SpringArm->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
	SpringArm->TargetArmLength = 800.f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 7.f;

	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	Ammo = 6;
	MaxAmmo = 6;
	Health = 25.f;
	MaxHealth = 100.f;
	Coins = 0;
	Forwards = true;
}

// Called when the game starts or when spawned
void APlayerCar::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APlayerCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

// Called to bind functionality to input
void APlayerCar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent != nullptr);
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerCar::MoveForward);
	InputComponent->BindAxis(TEXT("MoveRight"), this, &APlayerCar::MoveRight);
	InputComponent->BindAxis(TEXT("MoveCameraY"), this, &APlayerCar::MoveCameraY);

	PlayerInputComponent->BindAction("Nitro", EInputEvent::IE_Pressed, this, &APlayerCar::Nitro);
	PlayerInputComponent->BindAction("Shoot", EInputEvent::IE_Pressed, this, &APlayerCar::Shoot);
	PlayerInputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &APlayerCar::Reload);
}

void APlayerCar::MoveForward(float Value)
{
	float Force = 500000.f;
	FVector ForwardForce = (GetActorForwardVector() * Force);

	PlayerMesh->AddForce(ForwardForce * Value);

	PlayerMesh->SetAngularDamping(5.f);
	PlayerMesh->SetLinearDamping(3.f);

	if (Value < 0) { Forwards = false; }
	else if (Value > 0) { Forwards = true; }

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Forward x %f, y %f, z %f "), GetActorForwardVector().X, GetActorForwardVector().Y, GetActorForwardVector().Z));

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Linear %f "), PlayerMesh->GetLinearDamping()));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Angular %f "), PlayerMesh->GetAngularDamping()));
	
}

void APlayerCar::MoveRight(float Value)
{
	float Torque = 2500000.f;
	
	// Backwards steering functionality
	float Select;
	if (Forwards) { Select = 1; }
	else { Select = -1; }
	//float Product = FVector::DotProduct(PlayerMesh->GetPhysicsLinearVelocity(), GetActorForwardVector());
	//float Select = UKismetMathLibrary::SelectFloat(1, -1, Product > 0);
	FVector TorqueVector = FVector(0.f, 0.f, Select * Torque);
	
	PlayerMesh->AddTorqueInRadians(TorqueVector * Value);
}

void APlayerCar::MoveCameraY(float Value) 
{
	SpringArm->AddRelativeRotation(FRotator(0.f, Value, 0.f));
}

void APlayerCar::Shoot()
{	
	if (Ammo > 0)
	{
		
		UWorld* World = GetWorld();
		if (World)
		{
			Ammo--;
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("Ammo :  %d "), Ammo));
			// "Shotgun"
			// Subject to change

			FVector Location = GetActorLocation();
			FRotator Rotation = GetActorRotation();
			World->SpawnActor<AActor>(ActorToSpawn, Location + FVector(0.f, 0.f, 80.f), Rotation + FRotator(0.f, -4.f, 0.f));
			World->SpawnActor<AActor>(ActorToSpawn, Location + FVector(0.f, 0.f, 80.f), Rotation + FRotator(0.f, -2.f, 0.f));
			World->SpawnActor<AActor>(ActorToSpawn, Location + FVector(0.f, 0.f, 80.f), Rotation + FRotator(0.f, 0.f, 0.f));
			World->SpawnActor<AActor>(ActorToSpawn, Location + FVector(0.f, 0.f, 80.f), Rotation + FRotator(0.f, 2.f, 0.f));
			World->SpawnActor<AActor>(ActorToSpawn, Location + FVector(0.f, 0.f, 80.f), Rotation + FRotator(0.f, 4.f, 0.f));
			UGameplayStatics::PlaySound2D(World, Shooting, 1.f, 1.f, 0.f, 0);
		}
	}

	else if (Ammo <= 0)
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
	Ammo = MaxAmmo;
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
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("Player Picked Up Health %f "), Health));
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
