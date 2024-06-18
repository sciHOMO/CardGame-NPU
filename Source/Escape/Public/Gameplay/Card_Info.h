// Fill out your copyright notice in the Description page of Project Settings.
// 纯数据层

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Card_GameStateBase.h"
#include "GameFramework/Actor.h"
#include "Card_Info.generated.h"

class ACard_GameStateBase;
class ACard_Shape;

UCLASS()
class ESCAPE_API ACard_Info : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACard_Info();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated)
	int CardID = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	int Cost = 0;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	int Attack = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	int Defend = 1;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated)
	bool MainPlayerOwning = false;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated)
	TArray<E_Buff> CardBuff;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	FString Path = TEXT(""); //卡牌类路径
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated)
	ACard_Shape* CardShape = nullptr;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated)
	E_Position CardPos = E_Position::Desk;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated)
	E_Position CardLastPos = E_Position::Desk;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	E_Type CardType = E_Type::Spell;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	bool StandUp = true;
	
	UFUNCTION(Reliable, NetMulticast)
	void Cancel();
	
};
