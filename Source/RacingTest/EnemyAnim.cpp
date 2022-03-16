// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnim.h"
#include "GameFramework/CharacterMovementComponent.h"

void UEnemyAnim::UpdateAnimationProperties()
{
	if (MyPawn == nullptr)
	{
		MyPawn = TryGetPawnOwner();
	}

	if (MyPawn)
	{
		FVector TempSpeed = MyPawn->GetVelocity();
		FVector LateralSpeed = FVector(TempSpeed.X, TempSpeed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();
	}
}