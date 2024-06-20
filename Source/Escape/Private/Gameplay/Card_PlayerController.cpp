// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Card_PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Gameplay/Card_GameInstance.h"
#include "Gameplay/Card_Shape.h"
#include "Gameplay/Card_ShapeManager.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "UMG/BaseWidget.h"
#include "UMG/WaitForEffectWidget.h"
#include "UMG/WaitForResponseWidget.h"

class UBaseWidget;

ACard_PlayerController::ACard_PlayerController()
{
	SetShowMouseCursor(true);
	bAutoManageActiveCameraTarget = false;
	bReplicates = true;
}

void ACard_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (IsLocalPlayerController())
	{
		if (!BaseWidget)
		{
			UClass* WidgetClass = LoadClass<UUserWidget>(NULL, TEXT("WidgetBlueprint'/Game/UMG/UMG_Base.UMG_Base_C'"));
			if (!IsValid(WidgetClass))
			{
				return;
			}
			BaseWidget = CreateWidget<UBaseWidget>(this, WidgetClass, "BaseWidget");
			if (BaseWidget)
			{
				BaseWidget -> AddToViewport();
			}
		}
	} //生成UMG
}

void ACard_PlayerController::GenerateDesk_Implementation()
{
	if(HasAuthority())
	{
		Cast<ACard_GameStateBase>(UGameplayStatics::GetGameState(this)) -> GenerateDesk_Callback(Cast<UCard_GameInstance>(GetGameInstance()) -> Desk_1, MainPlayer ? 0 : 1);
	}	//在服务器上产生牌库
}

void ACard_PlayerController::PlayCard_Grave_Implementation(ACard_Info* CardInfo, bool IsMainPlayer)
{
	ACard_GameStateBase* GSB = Cast<ACard_GameStateBase>(UGameplayStatics::GetGameState(this));
	if(IsMainPlayer && GSB -> Player_Time.TryGetBoundFunctionName() == "Player_2_Time_Consume")
	{
		CardInfo -> Cancel();	//A的回合，A的响应，则取消
		return;
	}
	if (!IsMainPlayer && GSB -> Player_Time.TryGetBoundFunctionName() == "Player_1_Time_Consume")
	{
		CardInfo -> Cancel();	//A的回合，A的响应，则取消
		return;
	}
	if (CardInfo -> MainPlayerOwning != IsMainPlayer)
	{
		CardInfo -> Cancel();	//试图控制的不是自己的牌，取消
		return;
	}
	if (CardInfo -> Cost > (IsMainPlayer ? GSB -> Player_1_Mana : GSB -> Player_2_Mana))
	{
		CardInfo -> Cancel();	//法力不足，取消
		return;
	}
	if (CardInfo -> CardType != E_Type::Spell)
	{
		CardInfo -> Cancel();	//生物，取消
		return;
	}
	if (GSB -> LockUp)
	{
		CardInfo -> Cancel();
		return;
	}
	if (GSB -> Phase != E_Phase::Main)
	{
		for (auto &i : CardInfo -> CardEffect)
		{
			if (i.EffectTimer == E_EffectTimer::OnUse && i.Quick == true)
			{
				Cast<ACard_GameStateBase>(UGameplayStatics::GetGameState(this)) -> PlayCard_Grave(CardInfo, IsMainPlayer);
				return;
			}
		}
		CardInfo -> Cancel();	//阶段错误，取消
		return;
	}
	Cast<ACard_GameStateBase>(UGameplayStatics::GetGameState(this)) -> PlayCard_Grave(CardInfo, IsMainPlayer);
}

void ACard_PlayerController::PlayCard_Spawn_Implementation(ACard_Info* CardInfo, ATriggerBox* CardPlace, bool IsMainPlayer)
{
	ACard_GameStateBase* GSB = Cast<ACard_GameStateBase>(UGameplayStatics::GetGameState(this));
	if(IsMainPlayer && GSB -> Player_Time.TryGetBoundFunctionName() == "Player_2_Time_Consume")
	{
		CardInfo -> Cancel();
		return;
	}
	if (!IsMainPlayer && GSB -> Player_Time.TryGetBoundFunctionName() == "Player_1_Time_Consume")
	{
		CardInfo -> Cancel();
		return;
	}
	if (CardInfo -> MainPlayerOwning != IsMainPlayer)
	{
		CardInfo -> Cancel();
		return;
	}
	if (CardInfo -> Cost > (IsMainPlayer ? GSB -> Player_1_Mana : GSB -> Player_2_Mana))
	{
		CardInfo -> Cancel();
		return;
	}
	if (IsMainPlayer && (UKismetSystemLibrary::GetDisplayName(CardPlace).RightChop(2) == "3" || UKismetSystemLibrary::GetDisplayName(CardPlace).RightChop(2) == "4"))
	{
		CardInfo -> Cancel();	//位置错误，取消
		return;
	}
	if (!IsMainPlayer && (UKismetSystemLibrary::GetDisplayName(CardPlace).RightChop(2) == "1" || UKismetSystemLibrary::GetDisplayName(CardPlace).RightChop(2) == "2"))
	{
		CardInfo -> Cancel();
		return;
	}
	if (GSB -> Phase != E_Phase::Main)
	{
		CardInfo -> Cancel();
		return;
	}
	if (CardInfo -> CardType != E_Type::Creature)
	{
		CardInfo -> Cancel();
		return;
	}
	if (GSB -> Player_Turn != IsMainPlayer)
	{
		CardInfo -> Cancel();
		return;
	}
	if (GSB -> LockUp)
	{
		CardInfo -> Cancel();
		return;
	}
	Cast<ACard_GameStateBase>(UGameplayStatics::GetGameState(this)) -> PlayCard_Spawn(CardInfo, CardPlace, IsMainPlayer);
}

void ACard_PlayerController::Attack_Implementation(ACard_Info* CardInfo, ACard_Info* TargetCardInfo, bool IsMainPlayer)
{
	ACard_GameStateBase* GSB = Cast<ACard_GameStateBase>(UGameplayStatics::GetGameState(this));
	if (CardInfo -> MainPlayerOwning != IsMainPlayer)
	{
		CardInfo -> Cancel();
		return;
	}
	if (CardInfo -> MainPlayerOwning == TargetCardInfo -> MainPlayerOwning)
	{
		CardInfo -> Cancel();
		return;
	}
	if (GSB -> Phase != E_Phase::Battle)
	{
		CardInfo -> Cancel();	//非战阶
		return;
	}
	if (CardInfo -> CardType != E_Type::Creature || TargetCardInfo -> CardType != E_Type::Creature)
	{
		CardInfo -> Cancel();	//非生物
		return;
	}
	if (!CardInfo -> StandUp)
	{
		return;
	}
	CardInfo -> CardShape -> Attack(TargetCardInfo -> CardShape -> GetActorLocation());	//播放动画
	Delegate.BindUFunction(this, "Attack_Arch", CardInfo, TargetCardInfo, IsMainPlayer);
	GetWorldTimerManager().SetTimer(Handle, Delegate, 1000.0F, false);	//动画结束处理逻辑
}

void ACard_PlayerController::Attack_Arch(ACard_Info* CardInfo, ACard_Info* TargetCardInfo, bool IsMainPlayer)
{
	Cast<ACard_GameStateBase>(UGameplayStatics::GetGameState(this)) -> Attack(CardInfo, TargetCardInfo, IsMainPlayer);
}

void ACard_PlayerController::AttackDirectly_Implementation(ACard_Info* CardInfo, FVector Location, bool IsMainPlayer)
{
	ACard_GameStateBase* GSB = Cast<ACard_GameStateBase>(UGameplayStatics::GetGameState(this));
	if (CardInfo -> MainPlayerOwning != IsMainPlayer)
	{
		CardInfo -> Cancel();
		return;
	}
	if (GSB -> Phase != E_Phase::Battle)
	{
		CardInfo -> Cancel();
		return;
	}
	if (CardInfo -> CardType != E_Type::Creature)
	{
		CardInfo -> Cancel();
		return;
	}
	if (IsMainPlayer)
	{
		for (auto &i : GSB -> Player_2_Cards)
		{
			if (i -> CardPos == E_Position::Front)
			{
				CardInfo -> Cancel();	//还有生物没清干净，取消
				return;
			}
		}
	}
	else
	{
		for (auto &i : GSB -> Player_1_Cards)
		{
			if (i -> CardPos == E_Position::Front)
			{
				CardInfo -> Cancel();
				return;
			}
		}
	}
	if (!CardInfo -> StandUp)
	{
		return;
	}
	CardInfo -> CardShape -> Attack(Location);
	Delegate.BindUFunction(this, "AttackDirectly_Arch", CardInfo, Location, IsMainPlayer);
	GetWorldTimerManager().SetTimer(Handle, Delegate, 1000.0F, false);
}

void ACard_PlayerController::AttackDirectly_Arch(ACard_Info* CardInfo, FVector Location, bool IsMainPlayer)
{
	Cast<ACard_GameStateBase>(UGameplayStatics::GetGameState(this)) -> AttackDirectly(CardInfo, IsMainPlayer);
}

void ACard_PlayerController::AttackAnimCallBack_Implementation()
{
	bool A = Delegate.ExecuteIfBound();
	Delegate.Unbind();
	Handle.Invalidate();
}

void ACard_PlayerController::WaitForEffect_UMG_Implementation(ACard_Info* CardInfo, int EffectID)
{
	if (IsLocalPlayerController())
	{
		UClass* WidgetClass = LoadClass<UUserWidget>(NULL, TEXT("WidgetBlueprint'/Game/UMG/UMG_WaitforEffect.UMG_WaitforEffect_C'"));
			if (!IsValid(WidgetClass))
			{
				return;
			}
		if (UWaitForEffectWidget* WaitForEffect = CreateWidget<UWaitForEffectWidget>(this, WidgetClass, "WaitForEffect"))
			{
				WaitForEffect -> CardInfo = CardInfo;
				WaitForEffect -> EffectID = EffectID;
				WaitForEffect -> AddToViewport();
			}
	} 
}

void ACard_PlayerController::WaitForResponse_UMG_Implementation()
{
	if (IsLocalPlayerController())
	{
		UClass* WidgetClass = LoadClass<UUserWidget>(NULL, TEXT("WidgetBlueprint'/Game/UMG/UMG_WaitforResponse.UMG_WaitforResponse_C'"));
		if (!IsValid(WidgetClass))
		{
			return;
		}
		if (UWaitForResponseWidget* WaitForResponse = CreateWidget<UWaitForResponseWidget>(this, WidgetClass, "WaitForResponse"))
		{
			WaitForResponse -> AddToViewport();
		}
	} 
}

void ACard_PlayerController::NewFocusCard_Implementation(ACard_Info* CardInfo)
{
	BaseWidget;
}

void ACard_PlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACard_PlayerController, MainPlayer);
};