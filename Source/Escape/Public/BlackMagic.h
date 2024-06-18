// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BlackMagic.generated.h"

/**
 * 
 */
UCLASS()
class ESCAPE_API UBlackMagic : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	UFUNCTION()
	static void PRINTFLOAT(float Str, bool Permanent)
	{
		GEngine->AddOnScreenDebugMessage(-1, Permanent?0.0F:5.0F, FColor::Yellow, FString::Printf(TEXT("%f"), Str));
	}
};
