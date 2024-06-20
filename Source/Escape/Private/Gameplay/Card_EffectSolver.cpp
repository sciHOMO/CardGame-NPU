// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Card_EffectSolver.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACard_EffectSolver::ACard_EffectSolver()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void ACard_EffectSolver::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACard_EffectSolver::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow,
	//	FString::Printf(TEXT("%s"), *UEnum::GetValueAsString(GetLocalRole()).Append(LexToString(Primary))));
}

void ACard_EffectSolver::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACard_EffectSolver, InStack);
	DOREPLIFETIME(ACard_EffectSolver, Primary);
}