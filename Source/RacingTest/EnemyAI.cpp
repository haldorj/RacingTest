// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAI.h"
#include "Components/SphereComponent.h"
#include "AIController.h"	//Unreals AIController class
#include "PlayerCar.h"	//Since we move towards the Player
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"	//for GetPlayerController

// Sets default values
AEnemyAI::AEnemyAI()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// "Sensing sphere" to detect player.
	EnemySensingSphere = CreateDefaultSubobject<USphereComponent>(TEXT("EnemySensingSphere"));
	EnemySensingSphere->SetupAttachment(GetRootComponent());
	EnemySensingSphere->InitSphereRadius(1000.f);

	// Mesh
	EnemyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EnemyMesh"));
	EnemyMesh->SetupAttachment(RootComponent);

	//Sets value in MovementComponent:
	GetCharacterMovement()->MaxWalkSpeed = 200.f;

}

// Called when the game starts or when spawned
void AEnemyAI::BeginPlay()
{
	Super::BeginPlay();

	//Characters in level will automatically be given an AIController if not given a PlayerController
	//For more advanced use, you should make an own version of the AIController and put the AI stuff in there
	AIController = Cast<AAIController>(GetController());

	//Had to put these here and not in the constructor, to get them to work...
	EnemySensingSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyAI::OnOverlap);
	EnemySensingSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemyAI::OnOverlapEnd);

}

// Called every frame
void AEnemyAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyAI::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	APlayerCar* Player = Cast<APlayerCar>(OtherActor);
	if (Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Overlaps"));
		AIController->SetFocus(Player);		//This makes the NPC to rotate towards the player at all times
		//Easy way to see if it works:
		AIController->MoveToActor(Player, 0);

		//or More elaborate way, but gives us more insight:
		//MoveToTarget(Player);
	}
}

void AEnemyAI::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	APlayerCar* Player = Cast<APlayerCar>(OtherActor);
	if (AIController && Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Ends Overlap"));
		AIController->StopMovement();

		AIController->ClearFocus(EAIFocusPriority::Gameplay);
	}
}

void AEnemyAI::MoveToTarget(APlayerCar* APlayerCar)
{
	//have to include "AIModule" in ProjectNameBuild.cs file for this to work
	if (AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Overlaps"));

		//https://docs.unrealengine.com/en-US/API/Runtime/AIModule/FAIMoveRequest/index.html
		FAIMoveRequest AIMoverequest;
		AIMoverequest.SetGoalActor(APlayerCar);	//What to move towards
		AIMoverequest.SetAcceptanceRadius(25.f);	//How close before stop

		//https://docs.unrealengine.com/en-US/API/Runtime/Engine/AI/Navigation/FNavPathSharedPtr/index.html
		FNavPathSharedPtr NavPath;	//Will contain all location nodes for the path

		AIController->MoveTo(AIMoverequest, &NavPath);

		// **************** this just shows us the path *********************
		// can be deleted:
		//if (NavPath)
		//{
		//	//auto guesses the type for us
		//	auto PathPoints = NavPath->GetPathPoints();

		//	for (auto Point : PathPoints)
		//	{
		//		FVector Location = Point.Location;
		//		UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Green, 3, 0.5f);
		//	}
		//}
	}
}

// Called to bind functionality to input
// Not needed for AI
// 
//void AEnemyAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//}
