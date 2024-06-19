// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <string>

#include "CoreMinimal.h"
#include "Card_Info.h"
#include "GameFramework/Actor.h"
#include "Card_EffectSolver.generated.h"

DECLARE_DELEGATE_TwoParams(FTargetDelegate, TArray<ACard_Info*>, int);

UCLASS()
class ESCAPE_API ACard_EffectSolver : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ACard_EffectSolver();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	TArray<FTimerDelegate> Stack;

	UFUNCTION(BlueprintCallable)
	void AddEffecttoStack(TArray<ACard_Info*> Targets, int EffectID)
	{
		FTimerDelegate NewEffect;
		NewEffect.BindUFunction(this, FName("F" + FString::FromInt(EffectID)), Targets);
		Stack.Push(NewEffect);
	}

	UFUNCTION(BlueprintCallable)
	void SolveStack()
	{
		Stack.Last().Execute();
		Stack.Pop();
	}

	UFUNCTION()
	void F1001(TArray<ACard_Info*> Targets)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Happens")));;
	}
};
