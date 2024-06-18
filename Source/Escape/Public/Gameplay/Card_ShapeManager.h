// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Card_Info.h"
#include "Card_Shape.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "Card_ShapeManager.generated.h"

UCLASS()
class ESCAPE_API ACard_ShapeManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACard_ShapeManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USplineComponent* Player_1_HandLine;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USplineComponent* Player_2_HandLine;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated)
	TArray<ACard_Shape*> Player_1_Hand;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated)
	TArray<ACard_Shape*> Player_2_Hand;
	
	UFUNCTION(Reliable, Server)
	void DrawCard(int Player, ACard_Info* CardInfo);

	UFUNCTION()
	void UpdateCardLocationAndRotation_Hand();

	UFUNCTION(Reliable, Server)
	void RemoveFromHand(ACard_Info* CardInfo, bool IsMainPlayer);

	UFUNCTION(Reliable, NetMulticast)
	void AddtoField(ACard_Info* CardInfo, ATriggerBox* CardPlace);
	
	UFUNCTION(Reliable, Server)
	void SendtoGrave(ACard_Info* CardInfo);

	UFUNCTION(Reliable, Server)
	void Banish(ACard_Info* CardInfo);
	
};
