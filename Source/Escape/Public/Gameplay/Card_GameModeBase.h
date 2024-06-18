// Fill out your copyright notice in the Description page of Project Settings.
// 登录逻辑层


#pragma once

#include "CoreMinimal.h"
#include "Card_GameStateBase.h"
#include "GameFramework/GameModeBase.h"
#include "Card_GameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class ESCAPE_API ACard_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	public:	
	// Sets default values for this actor's properties
	ACard_GameModeBase();

	virtual void BeginPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override; //登录主函数
	
};
