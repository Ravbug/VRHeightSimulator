// Fill out your copyright notice in the Description page of Project Settings.


#include "VRPawn.h"
#include "Runtime/HeadMountedDisplay/Public/HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "AI/NavigationSystemBase.h"
#include "NavigationSystem.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "InteractableObject.h"
#include "DrawDebugHelpers.h"

// Sets default values
AVRPawn::AVRPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	root = CreateDefaultSubobject<UBoxComponent>("VRPawnRoot");
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	RootComponent = root;
	LeftHandController = CreateDefaultSubobject<UVRController>("Left Hand");
	RightHandController = CreateDefaultSubobject<UVRController>("Right Hand");
	LeftHandController->SetupAttachment(root);
	RightHandController->SetupAttachment(root);
	Camera->SetupAttachment(root);

	UIWidget = CreateDefaultSubobject<UWidgetComponent>("UMG Widget");
	UIWidget->SetupAttachment(Camera);
	UIWidget->SetDrawSize(FVector2D(1000,1000));
	UIWidget->SetRelativeScale3D(FVector(0.2,0.2,0.2));
	UIWidget->SetRelativeLocation(FVector(320,0,70));
	UIWidget->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0,0,180)));

	//Set the input source for the left and right hands
	LeftHandController->Init("Left");
	RightHandController->Init("Right");

	LeftHandController->Teleport.AddDynamic(this, &AVRPawn::TeleportTo);
	RightHandController->Teleport.AddDynamic(this, &AVRPawn::TeleportTo);
}

// Called when the game starts or when spawned
void AVRPawn::BeginPlay()
{
	Super::BeginPlay();

	//set tracking origin
	//Note:: be sure to set this pawn to be auto-possessed by player 0!
	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);

	//bind hud events
	/*HudActor->IncrementSizeEvt.AddDynamic(this, &AVRPawn::UMGIncreaseSize);
	HudActor->DecrementSizeEvt.AddDynamic(this, &AVRPawn::UMGDecreaseSize);*/

	//SetScale(2.0);
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

	PlayerInputComponent->BindAction("TeleportLeft", IE_Pressed, this, &AVRPawn::VRControllerRequestTeleportLeft);
	PlayerInputComponent->BindAction("TeleportRight", IE_Pressed, this, &AVRPawn::VRControllerRequestTeleportRight);
	PlayerInputComponent->BindAction("TeleportLeft", IE_Released, this, &AVRPawn::VRControllerConfirmTeleportLeft);
	PlayerInputComponent->BindAction("TeleportRight", IE_Released, this, &AVRPawn::VRControllerConfirmTeleportRight);
}

/**
 * Teleport this VR pawn to the passed location
 * @param newPos the location to teleport to
 */
void AVRPawn::TeleportTo(const FVector& newPos) {
	if (newPos != FVector::ZeroVector) {
		//TODO: set camera fade time
		SetActorLocation(newPos);
	}
}

/**
 * Set the size of this VR pawn. Affects movement speed.
 * @param newScale the new size to uniformly scale the pawn
 */
void AVRPawn::SetScale(float newScale) {
	root->SetRelativeScale3D(FVector(newScale, newScale, newScale));
	//SetActorScale3D(FVector(newScale,newScale,newScale));
}

/// ================================= PlayerInput wrapping functions ================================
void AVRPawn::VRControllerGrabLeft() {
	LeftHandController->WidgetInteraction->PressPointerKey(EKeys::LeftMouseButton);
	LeftHandController->Grab();
}
void AVRPawn::VRControllerGrabRight() {
	RightHandController->WidgetInteraction->PressPointerKey(EKeys::LeftMouseButton);
	RightHandController->Grab();
}
void AVRPawn::VRControllerReleaseLeft() {
	LeftHandController->WidgetInteraction->ReleasePointerKey(EKeys::LeftMouseButton);
	LeftHandController->Release();
}
void AVRPawn::VRControllerReleaseRight() {
	RightHandController->WidgetInteraction->ReleasePointerKey(EKeys::LeftMouseButton);
	RightHandController->Release();
}
void AVRPawn::VRControllerRequestTeleportRight() {
	RightHandController->RequestTeleport();
}
void AVRPawn::VRControllerRequestTeleportLeft() {
	LeftHandController->RequestTeleport();
}
void AVRPawn::VRControllerConfirmTeleportRight() {
	RightHandController->ConfirmTeleport();

}
void AVRPawn::VRControllerConfirmTeleportLeft() {
	LeftHandController->ConfirmTeleport();
}

/// ================================== UMG Events =====================================

void AVRPawn::UMGIncreaseSize()
{
	SCREENPRINT2("Increase size");
	currentSize += 0.1;
	SetScale(currentSize);
}

void AVRPawn::UMGDecreaseSize()
{
	SCREENPRINT2("Decrease size");
	currentSize -= 0.1;
	SetScale(currentSize);
}
