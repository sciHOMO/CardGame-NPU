// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Card_Shape.generated.h"

class ACard_Info;

UENUM(BlueprintType)
enum class E_CardState : uint8
{
	Attach, //跟随其他对象，自身停止更新
	Free,	//跟随鼠标，自己更新
	Freeze	//跟随指定坐标，自己更新
};

UCLASS()
class ESCAPE_API ACard_Shape : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACard_Shape();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated)
	ACard_Info* CardInfo;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated)
	E_CardState CardState = E_CardState::Freeze;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated)
	FVector FreezeLocation;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated)
	bool HasAnim = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterial* BackMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterial* FrontMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterial* FrontUIMaterial;
	
	UFUNCTION(BlueprintCallable)
	void LocalCardState(E_CardState NewCardState);
	
	UFUNCTION(BlueprintCallable, Unreliable, Server)
	void Attack(FVector TargetPosition);

	UFUNCTION(BlueprintNativeEvent)
	void Attack_Anim(FVector TargetPosition);
	
};
