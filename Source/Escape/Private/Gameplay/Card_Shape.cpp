// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/Card_Shape.h"
#include "Gameplay/Card_Info.h"
#include "Gameplay/Card_PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACard_Shape::ACard_Shape()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	FrontMaterial = CreateDefaultSubobject<UMaterial>("FrontMaterial");
	BackMaterial = CreateDefaultSubobject<UMaterial>("BackMaterial");
	FrontUIMaterial = CreateDefaultSubobject<UMaterial>("FrontUIMaterial");	//卡牌画面
}

// Called when the game starts or when spawned
void ACard_Shape::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACard_Shape::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(CardState == E_CardState::Free && !HasAuthority())
	{
		FVector Location, Direction;
		UGameplayStatics::GetPlayerControllerFromID(this, 0) -> DeprojectMousePositionToWorld(Location, Direction);
		Location.Z = 150.0F;
		SetActorLocation(UKismetMathLibrary::VLerp(GetActorLocation(), Location, 0.5F));
		Cast<ACard_PlayerController>(UGameplayStatics::GetPlayerControllerFromID(this, 0)) -> MainPlayer ?
		SetActorRotation(FRotator(0.0F, 180.0F, 0.0F)) : SetActorRotation(FRotator(0.0F, 0.0F, 0.0F));
	}
	if(CardState == E_CardState::Freeze && !HasAnim)
	{
		SetActorLocation(UKismetMathLibrary::VLerp(GetActorLocation(), FreezeLocation, 0.5F));
	}
	if(CardState == E_CardState::Freeze && !HasAuthority() && !HasAnim)
	{
		CardInfo -> MainPlayerOwning ? SetActorRotation(FRotator(0.0F, 180.0F, 0.0F)) : SetActorRotation(FRotator(0.0F, 0.0F, 0.0F));
	}
}

void ACard_Shape::LocalCardState(E_CardState NewCardState)
{
	CardState = NewCardState;
}

void ACard_Shape::Attack_Implementation(FVector TargetPosition)
{
	Attack_Anim(TargetPosition);
}

void ACard_Shape::Attack_Anim_Implementation(FVector TargetPosition)
{
	
}

void ACard_Shape::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACard_Shape, CardInfo);
	DOREPLIFETIME(ACard_Shape, CardState);
	DOREPLIFETIME(ACard_Shape, FreezeLocation);
	DOREPLIFETIME(ACard_Shape, HasAnim);
};