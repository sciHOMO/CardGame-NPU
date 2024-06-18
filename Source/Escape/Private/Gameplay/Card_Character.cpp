// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Card_Character.h"
#include "Gameplay/Card_GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACard_Character::ACard_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void ACard_Character::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACard_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACard_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACard_Character::NextPhase_Cha_Implementation()
{
	ACard_GameStateBase* GSB = Cast<ACard_GameStateBase>(UGameplayStatics::GetGameState(this));
	if (IsControl) //主控端获得操作权限
	{
		GSB->NextPhase();
	}
}

void ACard_Character::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACard_Character, IsControl);

};
