// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/Card_ShapeManager.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACard_ShapeManager::ACard_ShapeManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Player_1_HandLine = CreateDefaultSubobject<USplineComponent>(TEXT("Player_1_HandLine"));
	Player_2_HandLine = CreateDefaultSubobject<USplineComponent>(TEXT("Player_2_HandLine"));
	Player_1_HandLine -> SetWorldLocationAtSplinePoint(0, FVector(1000.0F, -1100.0F, 20.0F));
	Player_1_HandLine -> SetTangentAtSplinePoint(0, FVector(-1879.385254F,684.040283F,0.000000F), ESplineCoordinateSpace::World);
	Player_1_HandLine -> SetWorldLocationAtSplinePoint(1, FVector(-1000.0F, -1100.0F, 20.0F));
	Player_1_HandLine -> SetTangentAtSplinePoint(1, FVector(-1879.385254F,-684.040283F,0.000000F), ESplineCoordinateSpace::World);
	Player_2_HandLine -> SetWorldLocationAtSplinePoint(0, FVector(-1000.0F, 1100.0F, 20.0F));
	Player_2_HandLine -> SetTangentAtSplinePoint(0, FVector(1879.385254F,-684.040283F,0.000000F), ESplineCoordinateSpace::World);
	Player_2_HandLine -> SetWorldLocationAtSplinePoint(1, FVector(1000.0F, 1100.0F, 20.0F));
	Player_2_HandLine -> SetTangentAtSplinePoint(1, FVector(1879.385254F,684.040283F,0.000000F), ESplineCoordinateSpace::World);
}

// Called when the game starts or when spawned
void ACard_ShapeManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACard_ShapeManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateCardLocationAndRotation_Hand();
}

void ACard_ShapeManager::DrawCard_Implementation(int Player, ACard_Info* CardInfo)
{
	TArray<ACard_Shape*> Hand = Player == 0 ? Player_1_Hand : Player_2_Hand;
	if (Hand.Num() >= 8)
	{
		Banish(CardInfo);
		return;
	}
	FActorSpawnParameters SpawnInfo;
	const FString Path = CardInfo -> Path;
	UClass* CardShapeClass = LoadClass<ACard_Shape>(NULL, *Path);
	ACard_Shape* NewCard = Cast<ACard_Shape>(GetWorld()->SpawnActor(CardShapeClass, 0, 0, SpawnInfo));
	NewCard -> CardState = E_CardState::Attach;
	NewCard -> CardInfo = CardInfo;
	CardInfo -> CardShape = NewCard;
	CardInfo -> CardPos = E_Position::Hand;
	if (Player == 0)
	{
		Player_1_Hand.Insert(NewCard, 0);
		NewCard -> SetActorLocationAndRotation(Player_1_HandLine->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World), FRotator(0.0F, 180.0F, 0.0F));
	}
	else if (Player == 1)
	{
		Player_2_Hand.Insert(NewCard, 0);
		NewCard -> SetActorLocationAndRotation(Player_2_HandLine->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World), FRotator(0.0F, 0.0F, 0.0F));
	}
}

void ACard_ShapeManager::UpdateCardLocationAndRotation_Hand()
{
	TArray<ACard_Shape*> Pure_1_Hand;
	TArray<ACard_Shape*> Pure_2_Hand;
	for (int n = 0; n < Player_1_Hand.Num(); n++)
	{
		if(Player_1_Hand[n] && Player_1_Hand[n] -> CardState == E_CardState::Attach)
		{
			Pure_1_Hand.Emplace(Player_1_Hand[n]); 
		}
	}
	for (int n = 0; n < Player_2_Hand.Num(); n++)
	{
		if(Player_2_Hand[n] && Player_2_Hand[n] -> CardState == E_CardState::Attach)
		{
			Pure_2_Hand.Add(Player_2_Hand[n]); 
		}
	}
	for (int n = 1; n <= Pure_1_Hand.Num(); n++)
	{
		if(Pure_1_Hand[n-1])
		{
			const FVector CurLocation = Pure_1_Hand[n-1] -> GetActorLocation();
			const float Distance = 0.5 * (Player_1_HandLine -> GetSplineLength() - (Pure_1_Hand.Num() - 1) * 240.0F);
			const FVector LerpLocation = Player_1_HandLine -> GetLocationAtDistanceAlongSpline(Distance + (n - 1) * 240.0F, ESplineCoordinateSpace::World) +
				FVector(0.0F, 0.0F, n);
			Pure_1_Hand[n-1] -> SetActorLocation(UKismetMathLibrary::VLerp(CurLocation, LerpLocation, 0.2F));
			const FRotator Rotation = Player_1_HandLine -> GetRotationAtDistanceAlongSpline(Distance + (n - 1) * 240.0F, ESplineCoordinateSpace::World);
			Pure_1_Hand[n-1] -> SetActorRotation(Rotation);
		}
	}
	for (int n = 1; n <= Pure_2_Hand.Num(); n++)
	{
		if(Pure_2_Hand[n-1])
		{
			const FVector CurLocation = Pure_2_Hand[n-1] -> GetActorLocation();
			const float Distance = 0.5 * (Player_2_HandLine -> GetSplineLength() - (Pure_2_Hand.Num() - 1) * 240.0F);
			const FVector LerpLocation = Player_2_HandLine -> GetLocationAtDistanceAlongSpline(Distance + (n - 1) * 240.0F, ESplineCoordinateSpace::World) +
				FVector(0.0F, 0.0F, n);
			Pure_2_Hand[n-1] -> SetActorLocation(UKismetMathLibrary::VLerp(CurLocation, LerpLocation, 0.2F));
			const FRotator Rotation = Player_2_HandLine -> GetRotationAtDistanceAlongSpline(Distance + (n - 1) * 240.0F, ESplineCoordinateSpace::World);
			Pure_2_Hand[n-1] -> SetActorRotation(Rotation);
		}
	}
}

void ACard_ShapeManager::RemoveFromHand_Implementation(ACard_Info* CardInfo, bool IsMainPlayer)
{
	if (IsMainPlayer)
	{
		int n;
		for (n = 0; n < Player_1_Hand.Num(); n++)
		{
			if(Player_1_Hand[n] -> CardInfo == CardInfo)
			{
				break;
			}
		}
		Player_1_Hand[n] -> CardState = E_CardState::Free;
		Player_1_Hand.RemoveAt(n);
	}
	else
	{
		int n;
		for (n = 0; n < Player_2_Hand.Num(); n++)
		{
			if(Player_2_Hand[n] -> CardInfo == CardInfo)
			{
				break;
			}
		}
		Player_2_Hand[n] -> CardState = E_CardState::Free;
		Player_2_Hand.RemoveAt(n);
	}
}

void ACard_ShapeManager::AddtoField_Implementation(ACard_Info* CardInfo, ATriggerBox* CardPlace)
{
	CardInfo -> CardShape -> CardState = E_CardState::Freeze;
	if (UKismetSystemLibrary::GetDisplayName(CardPlace).RightChop(2) == "2" || UKismetSystemLibrary::GetDisplayName(CardPlace).RightChop(2) == "3")
	{
		CardInfo -> CardPos = E_Position::Front;
	}
	else
	{
		CardInfo -> CardPos = E_Position::Back;
	}
	CardInfo -> CardShape -> FreezeLocation = CardPlace -> GetActorLocation() + FVector(0.0F, 0.0F, 60.0F);
	CardInfo -> CardShape -> SetActorEnableCollision(true);
}

void ACard_ShapeManager::SendtoGrave_Implementation(ACard_Info* CardInfo)
{
	CardInfo -> CardShape ->Destroy();
	CardInfo -> CardPos = E_Position::Grave;
}

void ACard_ShapeManager::Banish_Implementation(ACard_Info* CardInfo)
{
	if (CardInfo -> CardShape)
	{
		CardInfo -> CardShape->Destroy();
	}
	CardInfo -> CardPos = E_Position::Banished;
}

void ACard_ShapeManager::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACard_ShapeManager, Player_1_Hand);
	DOREPLIFETIME(ACard_ShapeManager, Player_2_Hand);
};

