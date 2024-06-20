// Fill out your copyright notice in the Description page of Project Settings.
// 游戏主逻辑层

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "GameFramework/GameStateBase.h"
#include "Card_GameStateBase.generated.h"

class ACard_Info;
class ACard_ShapeManager;
class ACard_EffectSolver;

DECLARE_DELEGATE_OneParam(FDelegate, float)

UENUM(BlueprintType)	//四大阶段
enum class E_Phase : uint8
{
	Prepare,
	Main,
	Battle,
	End
};

UENUM(BlueprintType)	//卡牌位置
enum class E_Position : uint8
{
	Desk,
	Hand,
	Front,
	Back,
	Grave,
	Banished
};

UENUM(BlueprintType)	//卡牌类型
enum class E_Type : uint8
{
	Spell,
	Creature,
	Artifact
};

UENUM(BlueprintType)	//卡牌能力
enum class E_Buff : uint8
{
	Protect, //保护其他生物不被攻击
	LifeSteal,	//战斗回血
	Overkill, //战斗伤害取最高值
	Toxic, //一击必杀
	Double, //一回合双击
	Penetration, //直接攻击
	MagicProof, //不会被法术取对象
	Immune, //不受法术影响
	Constant, //数值不会变化
	Frozen, //无法攻击
	Silent, //效果无效
};

UCLASS()
class ESCAPE_API ACard_GameStateBase : public AGameStateBase
{
	GENERATED_BODY()

	ACard_GameStateBase();
	
	public:

	FDelegate Player_Time;	//计时器代理函数
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY()
	ACard_ShapeManager* ShapeManager;

	UPROPERTY()
	ACard_EffectSolver* EffectSolver;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated, Category="Turn")
	bool Player_Turn = true;	//谁的回合，主玩家=true

	UFUNCTION(BlueprintCallable, Reliable, Server, Category="Turn")
	void InitTurn();	//初始回合无战阶和抽牌
	
	UFUNCTION(BlueprintCallable, Reliable, Server, Category="Turn")
	void Turn();	//正常回合

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated, Category="Turn")
	int TurnCount = 1;	//回合数

	UFUNCTION(BlueprintCallable, Reliable, Server, Category="Turn")
	void React(bool Reset);	//响应计时器轮转

	float TurnTime = 300.0F;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated, Category="Turn")
	float Player_1_Time = TurnTime;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated, Category="Turn")
	float Player_2_Time = TurnTime;

	UFUNCTION(Category="Turn")
	void Player_1_Time_Consume(float DeltaTime);

	UFUNCTION(Category="Turn")
	void Player_2_Time_Consume(float DeltaTime);

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated, Category="Turn")
	E_Phase Phase = E_Phase::Prepare;

	UFUNCTION(BlueprintCallable, Reliable, NetMulticast, Category="Turn")
	void NextPhase(); //下一个阶段，由Character - NextPhase_Cha调用
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere,  ReplicatedUsing = Rep_Player_1_Health)
	int Player_1_Health = 20;

	UFUNCTION()
	void Rep_Player_1_Health(int Health)
	{
		if (Health <= 0)
		{
			Win(false);	//一方生命值归零，另一方赢赢
		}
	}

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere,  ReplicatedUsing = Rep_Player_2_Health)
	int Player_2_Health = 20;

	UFUNCTION()
	void Rep_Player_2_Health(int Health)
	{
		if (Health <= 0)
		{
			Win(true);
		}
	}

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, ReplicatedUsing = Rep_Player_1_Mana)
	int Player_1_Mana = 10;

	UFUNCTION()
	static void Rep_Player_1_Mana(int Mana)
	{
		;
	}

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, ReplicatedUsing = Rep_Player_2_Mana)
	int Player_2_Mana = 10;

	UFUNCTION()
	static void Rep_Player_2_Mana(int Mana)
	{
		;
	}
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) //牌库
	TArray<ACard_Info*> Player_1_Cards;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)	//牌库
	TArray<ACard_Info*> Player_2_Cards;

	UFUNCTION(Reliable, Server, Category="CardAct")
	void GenerateDesk(int Player);	//从GameInstance中提取卡组

	UFUNCTION(BlueprintCallable, Reliable, Server, Category="CardAct")
	void GenerateDesk_Callback(const TArray<TSubclassOf<ACard_Info>>& Card_Info, int Player);
	
	UFUNCTION(Category="CardAct")
	void ShuffleDesk(int Player);	//洗牌
	
	UFUNCTION(BlueprintCallable, Reliable, Server, Category="CardAct")
	void DrawCard(int Player, int Number);	//抽牌

	UFUNCTION(BlueprintCallable, Reliable, Server, Category="CardAct")
	void PlayCard_Grave(ACard_Info* CardInfo, bool IsMainPlayer);	//送墓
	
	UFUNCTION(BlueprintCallable, Reliable, Server, Category="CardAct")
	void PlayCard_Spawn(ACard_Info* CardInfo, ATriggerBox* CardPlace, bool IsMainPlayer);	//进场

	UFUNCTION(BlueprintCallable, Reliable, Server, Category="CardAct")
	void Attack(ACard_Info* CardInfo, ACard_Info* TargetCardInfo, bool IsMainPlayer);	//攻击

	UFUNCTION(BlueprintCallable, Reliable, Server, Category="CardAct")
	void AttackDirectly(ACard_Info* CardInfo, bool IsMainPlayer);	//直接攻击
	
	UFUNCTION(BlueprintCallable, Reliable, Server, Category="Logic")
	void TakeDamagez(int Damage, bool IsMainPlayer);	//造成伤害

	UFUNCTION(BlueprintCallable, Reliable, Server, Category="Logic")
	void Win(bool IsMainPlayer);	//获得胜利

	UFUNCTION(BlueprintCallable, Reliable, Server, Category="Logic")
	void WaitForEffect(ACard_Info* Source, int EffectID, bool IsMainPlayer);	//获得胜利

	UFUNCTION(BlueprintCallable, Reliable, Server, Category="Logic")
	void WaitForEffectCallBack(const TArray<ACard_Info*>& Source, int EffectID);

	UFUNCTION(BlueprintCallable, Reliable, Server, Category="Logic")
	void WaitForResponse(bool IsMainPlayer);	//等待响应

	UFUNCTION(BlueprintCallable, Reliable, Server, Category="Logic")
	void WaitForResponseCallBack();		//响应取消

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated, Category="Turn")
	bool LockUp = false;
};
