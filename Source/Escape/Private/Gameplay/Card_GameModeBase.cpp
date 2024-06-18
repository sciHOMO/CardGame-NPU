// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/Card_GameModeBase.h"
#include "Gameplay/Card_PlayerController.h"
#include "Gameplay/Card_Character.h"
#include "Kismet/GameplayStatics.h"

ACard_GameModeBase::ACard_GameModeBase()
{
	GameStateClass = ACard_GameStateBase::StaticClass();
	PlayerControllerClass = ACard_PlayerController::StaticClass();
	DefaultPawnClass = ACard_Character::StaticClass();
	HUDClass = nullptr;
}

void ACard_GameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

void ACard_GameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	TArray<AActor*> Player_1_Camera;
	UGameplayStatics::GetAllActorsWithTag(this, "Normal", Player_1_Camera);
	TArray<AActor*> Player_2_Camera;
	UGameplayStatics::GetAllActorsWithTag(this, "Reverse", Player_2_Camera); //指定玩家相机
	if (UGameplayStatics::GetGameState(this)->PlayerArray.Num() == 1)
	{
		NewPlayer -> ClientSetViewTarget(Player_1_Camera[0]);
		Cast<ACard_PlayerController>(NewPlayer) -> MainPlayer = true;	//先登录的玩家固定为主玩家
	}
	else
	{
		NewPlayer -> ClientSetViewTarget(Player_2_Camera[0]);
		Cast<ACard_PlayerController>(NewPlayer) -> MainPlayer = false;
		Cast<ACard_GameStateBase>(UGameplayStatics::GetGameState(this)) -> InitTurn(); //玩家准备完毕
	}
}

