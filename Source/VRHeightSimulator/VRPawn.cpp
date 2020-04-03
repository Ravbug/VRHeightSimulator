// Fill out your copyright notice in the Description page of Project Settings.


#include "VRPawn.h"
#include "Runtime/HeadMountedDisplay/Public/HeadMountedDisplayFunctionLibrary.h"

// Sets default values
AVRPawn::AVRPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	root = CreateDefaultSubobject<UBoxComponent>("VRPawnRoot");
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	RootComponent = root;
	LeftHandController = CreateDefaultSubobject<UMotionControllerComponent>("Left Hand");
	RightHandController = CreateDefaultSubobject<UMotionControllerComponent>("Right Hand");
	LeftHandController->SetupAttachment(root);
	RightHandController->SetupAttachment(root);
	Camera->SetupAttachment(root);

	//show the model for the hands
	LeftHandController->SetShowDeviceModel(true);
	RightHandController->SetShowDeviceModel(true);

	//Set the input source for the left and right hands
	LeftHandController->SetTrackingMotionSource("Left");
	RightHandController->SetTrackingMotionSource("Right");
}

// Called when the game starts or when spawned
void AVRPawn::BeginPlay()
{
	Super::BeginPlay();

	//set tracking origin
	//Note:: be sure to set this pawn to be auto-possessed by player 0!
	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
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

	//connect the controller events to the methods of this class
	PlayerInputComponent->BindAction("GrabLeft", IE_Pressed, this, &AVRPawn::VRControllerGrabLeft);
	PlayerInputComponent->BindAction("GrabRight", IE_Pressed, this, &AVRPawn::VRControllerGrabRight);
	PlayerInputComponent->BindAction("GrabLeft", IE_Released, this, &AVRPawn::VRControllerReleaseLeft);
	PlayerInputComponent->BindAction("GrabRight", IE_Released, this, &AVRPawn::VRControllerReleaseRight);

	PlayerInputComponent->BindAction("TeleportLeft", IE_Released, this, &AVRPawn::VRControllerRequestTeleportLeft);
	PlayerInputComponent->BindAction("TeleportRight", IE_Released, this, &AVRPawn::VRControllerRequestTeleportRight);
	PlayerInputComponent->BindAction("TeleportLeft", IE_Released, this, &AVRPawn::VRControllerConfirmTeleportLeft);
	PlayerInputComponent->BindAction("TeleportRight", IE_Released, this, &AVRPawn::VRControllerConfirmTeleportRight);
}

void AVRPawn::VRControllerStartTeleport(UMotionControllerComponent* controller) {
	
}

void AVRPawn::VRControllerEndTeleport(UMotionControllerComponent* controller) {
	SetActorLocation(FVector(0,-1000,0));
	UE_LOG(LogTemp, Warning, TEXT("c tp"));

}

void AVRPawn::VRControllerStartGrab(UMotionControllerComponent* controller) {

}

void AVRPawn::VRControllerEndGrab(UMotionControllerComponent* controller) {

}


void AVRPawn::VRControllerGrabLeft() {
	UE_LOG(LogTemp, Warning, TEXT("Grab left!"));
	VRControllerStartGrab(LeftHandController);
}

void AVRPawn::VRControllerGrabRight() {
	UE_LOG(LogTemp, Warning, TEXT("Grab right!"));
	VRControllerStartGrab(RightHandController);
}

void AVRPawn::VRControllerReleaseLeft() {
	UE_LOG(LogTemp, Warning, TEXT("Release left!"));
	VRControllerEndGrab(LeftHandController);
}

void AVRPawn::VRControllerReleaseRight() {
	UE_LOG(LogTemp, Warning, TEXT("Release right!"));
	VRControllerEndGrab(RightHandController);
}

void AVRPawn::VRControllerRequestTeleportRight() {
	VRControllerStartTeleport(RightHandController);
}
void AVRPawn::VRControllerRequestTeleportLeft() {
	VRControllerStartTeleport(LeftHandController);
}
void AVRPawn::VRControllerConfirmTeleportRight() {
	VRControllerEndTeleport(RightHandController);

}
void AVRPawn::VRControllerConfirmTeleportLeft() {
	VRControllerEndTeleport(LeftHandController);
}