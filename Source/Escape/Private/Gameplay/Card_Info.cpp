// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/Card_Info.h"
#include "Gameplay/Card_Shape.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACard_Info::ACard_Info()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

// Called when the game starts or when spawned
void ACard_Info::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACard_Info::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACard_Info::Cancel_Implementation()
{
	CardShape -> SetActorEnableCollision(true);
	switch(CardPos)
	{
	case E_Position::Desk : break;
	case E_Position::Hand : CardShape -> CardState = E_CardState::Attach; break;
	case E_Position::Front : CardShape -> CardState = E_CardState::Freeze; break;
	case E_Position::Back : CardShape -> CardState = E_CardState::Freeze; break;
	case E_Position::Grave : break;
	case E_Position::Banished : break;	//应该写到cardshape里去
	}
}

void ACard_Info::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACard_Info, CardID);
	DOREPLIFETIME(ACard_Info, MainPlayerOwning);
	DOREPLIFETIME(ACard_Info, Attack);
	DOREPLIFETIME(ACard_Info, Defend);
	DOREPLIFETIME(ACard_Info, Cost);
	DOREPLIFETIME(ACard_Info, Path);
	DOREPLIFETIME(ACard_Info, CardShape);
	DOREPLIFETIME(ACard_Info, CardPos);
	DOREPLIFETIME(ACard_Info, CardLastPos);
	DOREPLIFETIME(ACard_Info, CardType);
	DOREPLIFETIME(ACard_Info, StandUp);
};