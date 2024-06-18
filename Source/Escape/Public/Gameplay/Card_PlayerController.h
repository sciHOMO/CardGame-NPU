// Fill out your copyright notice in the Description page of Project Settings.
//核心输入层

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Card_PlayerController.generated.h"

/**
 * 
 */

UCLASS()
class ESCAPE_API ACard_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	ACard_PlayerController();

	virtual void BeginPlay() override;

	UFUNCTION(Reliable, NetMulticast)
	void GenerateDesk();

	UFUNCTION(BlueprintCallable, Reliable, Server)
	void PlayCard_Grave(ACard_Info* CardInfo, bool IsMainPlayer);
	
	UFUNCTION(BlueprintCallable, Reliable, Server)
	void PlayCard_Spawn(ACard_Info* CardInfo, ATriggerBox* CardPlace, bool IsMainPlayer);
	
	UFUNCTION(BlueprintCallable, Reliable, Server)
	void Attack(ACard_Info* CardInfo, ACard_Info* TargetCardInfo, bool IsMainPlayer);

	UFUNCTION()
	void Attack_NoAnim(ACard_Info* CardInfo, ACard_Info* TargetCardInfo, bool IsMainPlayer); //动画结束回调
	
	UFUNCTION(BlueprintCallable, Reliable, Server)
	void AttackDirectly(ACard_Info* CardInfo, FVector Location, bool IsMainPlayer);

	UFUNCTION()
	void AttackDirectly_NoAnim(ACard_Info* CardInfo, FVector Location, bool IsMainPlayer);	//动画结束回调
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UMG")	//UMG引用
	UUserWidget* BaseWidget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	bool MainPlayer = false;
	
};
