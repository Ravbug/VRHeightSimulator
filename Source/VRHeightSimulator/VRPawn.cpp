// Fill out your copyright notice in the Description page of Project Settings.


#include "VRPawn.h"

// Sets default values
AVRPawn::AVRPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	RootComponent = Camera;
	LeftHandController = CreateDefaultSubobject<UMotionControllerComponent>("Left Hand");
	RightHandController = CreateDefaultSubobject<UMotionControllerComponent>("Right Hand");
	LeftHandController->SetupAttachment(Camera);
	RightHandController->SetupAttachment(Camera);

	//show the model for the hands
	LeftHandController->SetShowDeviceModel(true);
	RightHandController->SetShowDeviceModel(true);
}

// Called when the game starts or when spawned
void AVRPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

