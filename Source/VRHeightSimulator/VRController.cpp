// Fill out your copyright notice in the Description page of Project Settings.


#include "VRController.h"
#include "ToggleableObject.h"
#include "Kismet/GameplayStatics.h"
#include "AI/NavigationSystemBase.h"
#include "NavigationSystem.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/HUD.h"
#include "InteractableObject.h"

AToggleableObject* castToToggleable(AActor* actor) {
	AToggleableObject* tobj = nullptr;
	if (actor != nullptr) {
		tobj = Cast<AToggleableObject>(actor);
	}
	return tobj;
}


// Sets default values for this component's properties
UVRController::UVRController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Controller = CreateDefaultSubobject<UMotionControllerComponent>("Controller");

	//collision boxes for controllers
	ControllerDetect = CreateDefaultSubobject<UBoxComponent>("Detect Trigger");
	ControllerDetect->SetupAttachment(Controller);
	
	//move the detection box forward and shrink it
	ControllerDetect->SetRelativeLocation(FVector(20,0,0));
	ControllerDetect->SetRelativeScale3D(FVector(0.5,0.5,0.5));

	WidgetInteraction = CreateDefaultSubobject<UWidgetInteractionComponent>("Widget Interaction Component");
	WidgetInteraction->SetupAttachment(Controller);

	WidgetInteraction->bShowDebug = true;

	//show the model for the hands
	Controller->SetShowDeviceModel(true);

	//Collision Events
	ControllerDetect->OnComponentBeginOverlap.AddDynamic(this, &UVRController::OnBeginOverlap);
	ControllerDetect->OnComponentEndOverlap.AddDynamic(this, &UVRController::OnEndOverlap);

	//this may not be necessary
	RegisterComponent();
}


// Called when the game starts
void UVRController::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

/**
 * Draw an array of points as a line
 * @param points the array of points to draw
 * @param lineColor the color to draw the line
 */
void UVRController::ConnectPointsWithLine(const TArray<FVector>& points, const FColor& lineColor, float thickness)
{
	int itr = points.Num() - 1;
	auto controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	auto hud = controller->GetHUD();
	for (int i = 0; i < itr; ++i) {
		//FVector2D p1, p2;
		/*int success = UGameplayStatics::ProjectWorldToScreen(controller, points[i], p1, true) * UGameplayStatics::ProjectWorldToScreen(controller, points[i + 1], p2, true);
		if ((bool)success)
		{
			hud->DrawLine(p1.X, p1.Y, p2.X, p2.Y, lineColor, thickness);
		}
		hud->Draw3DLine(points[i], points[i + 1], lineColor);*/
		DrawDebugLine(GetWorld(), points[i], points[i + 1], lineColor, false, 0.0f, 0.0f, thickness);
	}
}

// Called every frame
void UVRController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//should try to teleport?
	if (requestStatus == TeleportStatus::Request)
	{
		//is this object a controller?
		if (castToToggleable(hoverActor) == nullptr) {
			BeginTeleport();
		}
	}
	//teleport was confirmed? Notify VRPawn
	else if (requestStatus == TeleportStatus::Confirm) {
		//confirm teleport
		requestStatus = TeleportStatus::None;
		Teleport.Broadcast(teleportTarget);
	}
}

/**
 * Initialize this controller. You must call this method after creating an AVRController.
 * @param trackingSource the string representing the source to get tracking data for this controller
 */
void UVRController::Init(const FName& trackingSource)
{
	Controller->SetTrackingMotionSource(trackingSource);
}

/**
* Called when the boxcollider overlaps an object.
* Filters out only objects that are of the type AInteractableObject
* Marks the object as a hoverobject and sets it to highlight itself
*/
void UVRController::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto casted = Cast<AInteractableObject>(OtherActor);
	if (casted != nullptr) {
		hoverActor = OtherActor;
		casted->SetHighlightStatus(true);

	}
}

/**
 * Called when the boxcollider stops overlapping an object
 * Sets that object to stop highlighting itself if it is the hover object
 */
void UVRController::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == hoverActor) {
		hoverActor = nullptr;
		Cast<AInteractableObject>(OtherActor)->SetHighlightStatus(false);
	}
}

/**
 * Pick up the current hover object by signalling it to attach itself to this controller
 */
void UVRController::Grab()
{
	//attach the hover actor to this component
	if (hoverActor != nullptr) {
		auto casted = Cast<AInteractableObject>(hoverActor);
		casted->PickUp(Controller);
		casted->SetHighlightStatus(false);
	}
}

/**
 * Release the hoverobject by signalling it to resume physics and detach itself from this controller
 */
void UVRController::Release()
{
	if (hoverActor != nullptr) {
		Cast<AInteractableObject>(hoverActor)->Release();
	}
}

/**
 * Perform one tick of a VR arc teleport
 * Considers where the ground is using the navigation mesh
 * If the location is invalid, the teleport target is set to FVector::ZeroVector
 */
void UVRController::BeginTeleport()
{
	FHitResult hitpos;
	TArray<FVector> OutPath;
	FVector lastTrace;
	FVector LaunchVelocity(Controller->GetForwardVector() * 900);
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Init(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic), 1);

	//predict a projectile to find arc teleport position
	if (UGameplayStatics::Blueprint_PredictProjectilePath_ByObjectType(GetWorld(), hitpos, OutPath, lastTrace, Controller->GetComponentLocation(),
		LaunchVelocity, true, 0.0, ObjectTypes, false, TArray<AActor*>(), EDrawDebugTrace::None, 1.0, 30.0, 2.0, 0.0)) {

		FVector projectedPoint;
		//determine if the hit point is on the nav mesh by projecting the current point to the navmesh
		UNavigationSystemV1* navSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
		ANavigationData* navData = navSystem->GetNavDataForProps(parentActor->GetNavAgentPropertiesRef());
		TSubclassOf<UNavigationQueryFilter> FilterClass = UNavigationQueryFilter::StaticClass();
		bool navResult = navSystem->K2_ProjectPointToNavigation(GetWorld(), hitpos.Location, projectedPoint, navData, FilterClass);
		//if a new point was successfully calculated
		if (navResult) {
			//if projected point is not too far from the physics hit point
			if (FVector::Distance(hitpos.Location, projectedPoint) <= TeleportMaxProjectionDistance) {
				teleportTarget = projectedPoint;
				ConnectPointsWithLine(OutPath, FColor::Green);
				DrawDebugSphere(GetWorld(), hitpos.Location, 5, 2, FColor::Red);
			}
			else {
				ConnectPointsWithLine(OutPath, FColor::Red);
			}
		}
		else {
			ConnectPointsWithLine(OutPath, FColor::Red);
		}
	}
	else {
		teleportTarget = FVector::ZeroVector;
		ConnectPointsWithLine(OutPath, FColor::Red);
	}
}

void UVRController::RequestTeleport() {
	auto casted = castToToggleable(hoverActor);
	if (casted != nullptr) {
		requestStatus = TeleportStatus::None;
		casted->Interact();
	}
	else {
		requestStatus = TeleportStatus::Request;
	}
}

void UVRController::ConfirmTeleport() {
	auto casted = castToToggleable(hoverActor);
	if (casted != nullptr) {
		requestStatus = TeleportStatus::None;
	}
	else {
		requestStatus = TeleportStatus::Confirm;
	}
}