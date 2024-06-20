// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gameplay/Card_Info.h"
#include "WaitForEffectWidget.generated.h"

/**
 * 
 */
UCLASS()
class ESCAPE_API UWaitForEffectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly)
	ACard_Info* CardInfo;

	UPROPERTY(BlueprintReadOnly)
	int EffectID;
	
};
