// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/Card_GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Gameplay/Card_Character.h"
#include "Gameplay/Card_Info.h"
#include "Gameplay/Card_ShapeManager.h"
#include "Gameplay/Card_PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ACard_GameStateBase::ACard_GameStateBase()
{
	PrimaryActorTick.bCanEverTick = true;
	ShapeManager = Cast<ACard_ShapeManager>(UGameplayStatics::GetActorOfClass(this, ACard_ShapeManager::StaticClass()));	//初始化场地管理类方便引用
}

void ACard_GameStateBase::BeginPlay()
{
	Super::BeginPlay();
}

void ACard_GameStateBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	bool IsValid = Player_Time.ExecuteIfBound(DeltaSeconds);	//计时器绑定时，消耗回合时间
}

void ACard_GameStateBase::InitTurn_Implementation()
{
	Player_Turn = true;
	Phase = E_Phase::Prepare;
	Cast<ACard_Character>(PlayerArray[0] -> GetPawn()) -> IsControl = true;
	Cast<ACard_Character>(PlayerArray[1] -> GetPawn()) -> IsControl = false;
	GenerateDesk(0);
	GenerateDesk(1);
	React();
	DrawCard(0, 5);
	DrawCard(1, 5);	//抽五张
}

void ACard_GameStateBase::Turn_Implementation()
{
	TurnCount++;	//增加回合计数
	Player_Turn = !Player_Turn;	//重置倒计时和法力
	Player_Turn ? Player_1_Mana = 10 : Player_2_Mana = 10;
	Player_1_Time = 120.0F;
	Player_2_Time = 120.0F;
	Phase = E_Phase::Prepare;
	if (PlayerArray.Num() == 2)
	{
		Cast<ACard_Character>(PlayerArray[0]->GetPawn())->IsControl = !Cast<ACard_Character>(PlayerArray[0]->GetPawn())->IsControl;
		Cast<ACard_Character>(PlayerArray[1]->GetPawn())->IsControl = !Cast<ACard_Character>(PlayerArray[1]->GetPawn())->IsControl;
	}
	React();
	DrawCard(Player_Turn ? 0 : 1, 1); //回合玩家抽一
}

void ACard_GameStateBase::React_Implementation()
{
	Player_Time.Unbind();	//主控消耗倒计时
	if(Player_Turn)
	{
		Player_Time.BindUFunction(this, "Player_1_Time_Consume");
	}
	else
	{
		Player_Time.BindUFunction(this, "Player_2_Time_Consume");
	}
}

void ACard_GameStateBase::Player_1_Time_Consume(float DeltaTime)
{
	Player_1_Time = Player_1_Time - DeltaTime;	//倒计时，倒计时毕结束回合
	if (Player_1_Time < 0.0F)
	{
		Turn();	//强制进入下个回合，BYD烧绳是吧
	}
}

void ACard_GameStateBase::Player_2_Time_Consume(float DeltaTime)
{
	Player_2_Time = Player_2_Time - DeltaTime;	//倒计时，倒计时毕结束回合
	if (Player_2_Time < 0.0F)
	{
		Turn();
	}
}

void ACard_GameStateBase::GenerateDesk_Implementation(int Player)
{
	if (HasAuthority())
	{
		APlayerState* PS = Player == 0 ? PlayerArray[0] : PlayerArray[1];
		Cast<ACard_PlayerController>(PS -> GetOwningController()) -> GenerateDesk(); //由Controller转递牌库
	}
}

void ACard_GameStateBase::GenerateDesk_Callback_Implementation(const TArray<TSubclassOf<ACard_Info>>& Card_Info, int Player)
{
	int CardID = 0;
	for (auto &i : Card_Info)
	{
		FActorSpawnParameters SpawnInfo;	
		ACard_Info* NewCard = Cast<ACard_Info>(GetWorld()->SpawnActor(i, 0, 0, SpawnInfo)); //从类类型生成实例
		Player == 0 ? NewCard -> CardID = CardID++ : NewCard -> CardID = 100 + CardID++;	//卡牌唯一ID，主玩家从0-99
		Player == 0 ? Player_1_Cards.Add(NewCard) : Player_2_Cards.Add(NewCard);
		Player == 0 ? NewCard -> MainPlayerOwning = true : NewCard -> MainPlayerOwning = false;
	}
	ShuffleDesk(Player);	//洗牌
}

void ACard_GameStateBase::ShuffleDesk(int Player)
{
	if (Player == 0)
	{
		const int32 LastIndex = Player_1_Cards.Num() - 1;
		for (int32 i = 0; i <= LastIndex; ++i)
		{
			const int32 Index = FMath::RandRange(i, LastIndex);
			if (i != Index)
			{
				Player_1_Cards.Swap(i, Index);
			}
		}
	}
	else
	{
		const int32 LastIndex = Player_2_Cards.Num() - 1;
		for (int32 i = 0; i <= LastIndex; ++i)
		{
			const int32 Index = FMath::RandRange(i, LastIndex);
			if (i != Index)
			{
				Player_2_Cards.Swap(i, Index);
			}
		}
	}
}

void ACard_GameStateBase::DrawCard_Implementation(int Player, int Number)
{
	TArray<ACard_Info*> Player_Card = Player == 0 ? Player_1_Cards : Player_2_Cards;
	int n = 0;
	for (auto &i : Player_Card)
	{
		if(n < Number && i -> CardPos == E_Position::Desk)
		{
			ShapeManager -> DrawCard(Player, i);
			n++;
		}
	}
	if (n < Number)
	{
		Win(!Player); //抽不满牌意味着抽光了。死了！
	}
}

void ACard_GameStateBase::PlayCard_Grave_Implementation(ACard_Info* CardInfo, bool IsMainPlayer)
{
	IsMainPlayer ? Player_1_Mana = Player_1_Mana - CardInfo -> Cost : Player_2_Mana = Player_2_Mana - CardInfo -> Cost;
	ShapeManager -> RemoveFromHand(CardInfo, IsMainPlayer);	//移出手牌（显示层面的）
	ShapeManager -> SendtoGrave(CardInfo); //送墓
}

void ACard_GameStateBase::PlayCard_Spawn_Implementation(ACard_Info* CardInfo, ATriggerBox* CardPlace, bool IsMainPlayer)
{
	IsMainPlayer ? Player_1_Mana = Player_1_Mana - CardInfo -> Cost : Player_2_Mana = Player_2_Mana - CardInfo -> Cost;
	ShapeManager -> RemoveFromHand(CardInfo, IsMainPlayer);
	ShapeManager -> AddtoField(CardInfo, CardPlace);
}

void ACard_GameStateBase::Attack_Implementation(ACard_Info* CardInfo, ACard_Info* TargetCardInfo, bool IsMainPlayer)
{
	CardInfo -> StandUp = false; //行动力
	if (CardInfo -> Attack > TargetCardInfo -> Defend)
	{
		ShapeManager -> SendtoGrave(TargetCardInfo);
		TakeDamagez(CardInfo -> Attack - TargetCardInfo -> Defend, TargetCardInfo -> MainPlayerOwning);
	}
	else if (CardInfo -> Attack < TargetCardInfo -> Defend)
	{
		ShapeManager -> SendtoGrave(CardInfo);
		TakeDamagez(TargetCardInfo -> Defend - CardInfo -> Attack, CardInfo -> MainPlayerOwning);
	}
	else
	{
		ShapeManager -> SendtoGrave(CardInfo);
		ShapeManager -> SendtoGrave(TargetCardInfo);
	}
}

void ACard_GameStateBase::AttackDirectly_Implementation(ACard_Info* CardInfo, bool IsMainPlayer)
{
	CardInfo -> StandUp = false;
	TakeDamagez(CardInfo -> Attack, !IsMainPlayer);
}

void ACard_GameStateBase::TakeDamagez_Implementation(int Damage, bool IsMainPlayer)
{
	IsMainPlayer ? Player_1_Health = Player_1_Health - Damage : Player_2_Health = Player_2_Health - Damage;
}

void ACard_GameStateBase::Win_Implementation(bool IsMainPlayer)
{
	
}

void ACard_GameStateBase::NextPhase_Implementation()
{
	switch(Phase)
	{
		case E_Phase::Prepare : Phase = E_Phase::Main;
		if (Player_Turn)
			{for (auto &i : Player_1_Cards) {i -> StandUp = true;}}
		else {for (auto &i : Player_2_Cards) {i -> StandUp = true;}} //准备阶段重置行动力
		 break;
		case E_Phase::Main : TurnCount == 1 ? Phase = E_Phase::End : Phase = E_Phase::Battle; break;
		case E_Phase::Battle : Phase = E_Phase::End; break;
		case E_Phase::End : Phase = E_Phase::Prepare; Turn(); break;	//下一回合
	}
}

void ACard_GameStateBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACard_GameStateBase, Player_1_Health);
	DOREPLIFETIME(ACard_GameStateBase, Player_1_Mana);
	DOREPLIFETIME(ACard_GameStateBase, Player_1_Time);
	DOREPLIFETIME(ACard_GameStateBase, Player_1_Cards);
	DOREPLIFETIME(ACard_GameStateBase, Player_2_Health);
	DOREPLIFETIME(ACard_GameStateBase, Player_2_Mana);
	DOREPLIFETIME(ACard_GameStateBase, Player_2_Time);
	DOREPLIFETIME(ACard_GameStateBase, Player_2_Cards);
	DOREPLIFETIME(ACard_GameStateBase, TurnCount);
};