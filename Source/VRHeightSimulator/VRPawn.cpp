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

	//collision boxes for controllers
	RightControllerDetect = CreateDefaultSubobject<UBoxComponent>("Right Detect");
	LeftControllerDetect = CreateDefaultSubobject<UBoxComponent>("Left Detect");
	RightControllerDetect->SetupAttachment(RightHandController);
	LeftControllerDetect->SetupAttachment(LeftHandController);

	//Collision Events
	RightControllerDetect->OnComponentBeginOverlap.AddDynamic(this, &AVRPawn::OnRightBeginOverlap);
	LeftControllerDetect->OnComponentBeginOverlap.AddDynamic(this, &AVRPawn::OnLeftBeginOverlap);
	RightControllerDetect->OnComponentEndOverlap.AddDynamic(this, &AVRPawn::OnRightEndOverlap);
	LeftControllerDetect->OnComponentEndOverlap.AddDynamic(this, &AVRPawn::OnLeftEndOverlap);
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

	//request teleport
	if (isLeftRequesting == TeleportStatus::Request) {
		VRControllerStartTeleport(LeftHandController);
	}
	else if (isRightRequesting == TeleportStatus::Request) {
		VRControllerStartTeleport(RightHandController);
	}

	//confirm teleport
	if ((isLeftRequesting == TeleportStatus::Confirm || isRightRequesting == TeleportStatus::Confirm)) {
		VRControllerEndTeleport(teleportTarget);
		isLeftRequesting = isRightRequesting = TeleportStatus::None;
	}

	//SCREENPRINT("Actor at %ld is near Left controller ", &(*leftHover));
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

void AVRPawn::VRControllerStartTeleport(UMotionControllerComponent* controller) {
	FHitResult hitpos;
	TArray<FVector> OutPath;
	FVector lastTrace;
	FVector LaunchVelocity(controller->GetForwardVector() * 900);
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Init(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic),1);

	//predict a projectile to find arc teleport position
	if (UGameplayStatics::Blueprint_PredictProjectilePath_ByObjectType(GetWorld(), hitpos, OutPath, lastTrace, controller->GetComponentLocation(),
		LaunchVelocity, true, 0.0, ObjectTypes, false, TArray<AActor*>(), EDrawDebugTrace::ForOneFrame, 1.0, 30.0, 2.0, 0.0)) {

		FVector projectedPoint;
		//determine if the hit point is on the nav mesh by projecting the current point to the navmesh
		UNavigationSystemV1* navSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
		ANavigationData* navData = navSystem->GetNavDataForProps(GetNavAgentPropertiesRef());
		TSubclassOf<UNavigationQueryFilter> FilterClass = UNavigationQueryFilter::StaticClass();
		bool navResult = navSystem->K2_ProjectPointToNavigation(GetWorld(), hitpos.Location, projectedPoint, navData, FilterClass);
		//if a new point was successfully calculated
		if (navResult) {
			//if projected point is not too far from the physics hit point
			if (FVector::Distance(hitpos.Location, projectedPoint) <= TeleportMaxProjectionDistance) {
				teleportTarget = projectedPoint;
				DrawDebugSphere(GetWorld(), hitpos.Location, 5, 2, FColor::Red);
			}
		}
	}
	else {
		teleportTarget = FVector::ZeroVector;
	}

	
}

void AVRPawn::VRControllerEndTeleport(const FVector& newPos) {
	if (newPos != FVector::ZeroVector) {
		//TODO: set camera fade time
		SetActorLocation(newPos);
	}
}


void AVRPawn::OnLeftBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto casted = Cast<AInteractableObject>(OtherActor);
	if (casted != nullptr) {
		leftHover = OtherActor;
		casted->SetHighlightStatus(true);
	}
}

void AVRPawn::OnRightBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto casted = Cast<AInteractableObject>(OtherActor);
	if (casted != nullptr) {
		rightHover = OtherActor;
		casted->SetHighlightStatus(true);
	}
}

void AVRPawn::OnLeftEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == leftHover) {
		leftHover = nullptr;
		Cast<AInteractableObject>(OtherActor)->SetHighlightStatus(false);
	}
}

void AVRPawn::OnRightEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == rightHover) {
		rightHover = nullptr;
		Cast<AInteractableObject>(OtherActor)->SetHighlightStatus(false);
	}
}


void AVRPawn::VRControllerGrabLeft() {
	//attach the hover actor to this component
	if (leftHover != nullptr) {
		auto casted = Cast<AInteractableObject>(leftHover);
		casted->PickUp(LeftHandController);
		casted->SetHighlightStatus(false);
	}
}

void AVRPawn::VRControllerGrabRight() {
	if (rightHover != nullptr) {
		auto casted = Cast<AInteractableObject>(rightHover);
		casted->PickUp(RightHandController);
		casted->SetHighlightStatus(false);
	}
}

void AVRPawn::VRControllerReleaseLeft() {
	if (leftHover != nullptr) {
		Cast<AInteractableObject>(leftHover)->Release();
	}
}

void AVRPawn::VRControllerReleaseRight() {
	if (rightHover != nullptr) {
		Cast<AInteractableObject>(rightHover)->Release();
	}
}

void AVRPawn::VRControllerRequestTeleportRight() {
	isRightRequesting = TeleportStatus::Request;
}
void AVRPawn::VRControllerRequestTeleportLeft() {
	isLeftRequesting = TeleportStatus::Request;
}
void AVRPawn::VRControllerConfirmTeleportRight() {
	isRightRequesting = TeleportStatus::Confirm;

}
void AVRPawn::VRControllerConfirmTeleportLeft() {
	isLeftRequesting = TeleportStatus::Confirm;
}