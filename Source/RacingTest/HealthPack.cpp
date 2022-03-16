// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPack.h"

// Sets default values
AHealthPack::AHealthPack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	CoinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	CoinMesh->SetupAttachment(GetRootComponent());
	Cast<UShapeComponent>(RootComponent)->OnComponentBeginOverlap.AddDynamic(this, &AHealthPack::OnOverlap);

}

// Called when the game starts or when spawned
void AHealthPack::BeginPlay()
{
	Super::BeginPlay();


	
}

// Called every frame
void AHealthPack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHealthPack::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}
