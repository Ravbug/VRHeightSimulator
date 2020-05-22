// Fill out your copyright notice in the Description page of Project Settings.

#include "VRPawn.h"
#include "Runtime/HeadMountedDisplay/Public/HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "AI/NavigationSystemBase.h"
#include "NavigationSystem.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "InteractableObject.h"
#include "DrawDebugHelpers.h"

#define in_increment 1/2.54/12/2

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
	UIWidget->SetVisibility(false);
	UIWidget->SetBlendMode(EWidgetBlendMode::Transparent);
	//set to screenspace (always render on top)
	//UIWidget->SetWidgetSpace(EWidgetSpace::Screen);

	WebcamMesh = CreateDefaultSubobject<UStaticMeshComponent>("Webcam Mesh");
	WebcamMesh->SetupAttachment(Camera);
	WebcamMesh->SetRelativeLocation(FVector(590,0,0));
	WebcamMesh->SetRelativeScale3D(FVector(50,50,50));
	WebcamMesh->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0,180,90)));

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

	UIWidget->SetVisibility(false);
	WebcamMesh->SetVisibility(false, true);

	//set tracking origin
	//Note:: be sure to set this pawn to be auto-possessed by player 0!
	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
}

// Called every frame
void AVRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (UIWidget->IsVisible()) {
		float distance = Camera->GetComponentLocation().Z - GetActorLocation().Z;
		float inches = distance / 2.54;
		float feet = inches / 12;
		inches = (inches - ((int)feet * 12));

		DisplayCurrentHeight(FString::Printf(TEXT("%d ft %d in / %d cm"),(int)feet,(int)inches,(int)distance));
	}
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

	PlayerInputComponent->BindAction("Menu", IE_Pressed, this, &AVRPawn::OnMenu);
	PlayerInputComponent->BindAction("ShowCameras", IE_Pressed, this, &AVRPawn::OnGrip);
}

/**
 * Teleport this VR pawn to the passed location
 * @param newPos the location to teleport to
 */
void AVRPawn::TeleportTo(const FVector& newPos) {
	if (newPos != FVector::ZeroVector) {
		//TODO: set camera fade time

		//teleport center to correct spot by offsetting the camera
		auto campos = Camera->GetRelativeLocation();
		SetActorLocation(FVector(newPos.X - campos.X,newPos.Y - campos.Y, newPos.Z));
	}
}

/**
 * Set the size of this VR pawn. Affects movement speed.
 * @param newScale the new size to uniformly scale the pawn
 */
void AVRPawn::SetScale(float newScale) {
	//drop object that is in hand
	VRControllerReleaseRight();
	VRControllerReleaseLeft();

	root->SetRelativeScale3D(FVector(newScale, newScale, newScale));
}

void AVRPawn::OnMenu() {
	UIWidget->SetVisibility(!UIWidget->IsVisible(),true);
}

void AVRPawn::OnGrip() {
	WebcamMesh->SetVisibility(!WebcamMesh->IsVisible(),true);
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

/**
 * Increase the size of the pawn by approximately 1 inch
 */
void AVRPawn::UMGIncreaseSize()
{
	currentSize += in_increment;
	SetScale(currentSize);
}

/**
 * Decrease the size of the pawn by approximately 1 inch
 */
void AVRPawn::UMGDecreaseSize()
{
	currentSize -= in_increment;
	SetScale(currentSize);
}

/**
 * Set the size of the pawn in float
 */
void AVRPawn::UMGSetHeightPercent(float percent)
{
	currentSize = UKismetMathLibrary::MapRangeClamped(percent,0,1,minSlider,maxSlider);
	SetScale(currentSize);
}