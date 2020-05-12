// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDActor.h"

// Sets default values
AHUDActor::AHUDActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	UIWidget = CreateDefaultSubobject<UWidgetComponent>("UMG Widget");
	UIWidget->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AHUDActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHUDActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHUDActor::DecrementHit()
{
	DecrementSizeEvt.Broadcast();
}

void AHUDActor::IncrementHit()
{
	IncrementSizeEvt.Broadcast();
}