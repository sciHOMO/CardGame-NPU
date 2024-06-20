// Fill out your copyright notice in the Description page of Project Settings.
//核心输入层

#pragma once

#include "CoreMinimal.h"
#include "Card_Info.h"
#include "GameFramework/PlayerController.h"
#include "Card_PlayerController.generated.h"

class UBaseWidget;

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

	FTimerDelegate Delegate;
	FTimerHandle Handle;

	UFUNCTION(Reliable, NetMulticast)
	void GenerateDesk();

	UFUNCTION(BlueprintCallable, Reliable, Server)
	void PlayCard_Grave(ACard_Info* CardInfo, bool IsMainPlayer);
	
	UFUNCTION(BlueprintCallable, Reliable, Server)
	void PlayCard_Spawn(ACard_Info* CardInfo, ATriggerBox* CardPlace, bool IsMainPlayer);
	
	UFUNCTION(BlueprintCallable, Reliable, Server)
	void Attack(ACard_Info* CardInfo, ACard_Info* TargetCardInfo, bool IsMainPlayer);

	UFUNCTION()
	void Attack_Arch(ACard_Info* CardInfo, ACard_Info* TargetCardInfo, bool IsMainPlayer); //动画结束回调
	
	UFUNCTION(BlueprintCallable, Reliable, Server)
	void AttackDirectly(ACard_Info* CardInfo, FVector Location, bool IsMainPlayer);

	UFUNCTION()
	void AttackDirectly_Arch(ACard_Info* CardInfo, FVector Location, bool IsMainPlayer);	//动画结束回调
	
	UFUNCTION(BlueprintCallable, Reliable, Server)
	void AttackAnimCallBack();

	UFUNCTION(Reliable, Client)
	void WaitForEffect_UMG(ACard_Info* CardInfo, int EffectID);

	UFUNCTION(Reliable, Client)
	void WaitForResponse_UMG();
	
	UFUNCTION(Reliable, Client)
	void NewFocusCard(ACard_Info* CardInfo);	//那些新登场的卡牌会临时刷新到关注区
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UMG")	
	UBaseWidget* BaseWidget;	//UMG引用

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	bool MainPlayer = false;
	
};
