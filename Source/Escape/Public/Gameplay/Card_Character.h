// Fill out your copyright notice in the Description page of Project Settings.
// 转接口类，负责UI事件转发，无逻辑层实现
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Card_Character.generated.h"

UCLASS()
class ESCAPE_API ACard_Character : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACard_Character();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Unreliable, Server)
	void NextPhase_Cha();

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated)
	bool IsControl = true;
};
