// Fill out your copyright notice in the Description page of Project Settings.
// 卡组库文件，可扩充

#pragma once

#include "CoreMinimal.h"
#include "Card_Info.h"
#include "Engine/GameInstance.h"
#include "Card_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ESCAPE_API UCard_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSubclassOf<ACard_Info>> Desk_1; 

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSubclassOf<ACard_Info>> Desk_2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSubclassOf<ACard_Info>> Desk_3; 

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSubclassOf<ACard_Info>> Desk_4;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSubclassOf<ACard_Info>> Desk_5; 

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSubclassOf<ACard_Info>> Desk_6;
	
};
