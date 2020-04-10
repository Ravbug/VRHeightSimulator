// Fill out your copyright notice in the Description page of Project Settings.


#include "VRController.h"
#include "Kismet/GameplayStatics.h"
#include "AI/NavigationSystemBase.h"
#include "NavigationSystem.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "DrawDebugHelpers.h"
#include "InteractableObject.h"


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


// Called every frame
void UVRController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (requestStatus == TeleportStatus::Request)
	{
		BeginTeleport();
	}
	else if (requestStatus == TeleportStatus::Confirm) {
		//confirm teleport
		requestStatus = TeleportStatus::None;
		//Teleport.Broadcast(teleportTarget);
	}
}

void UVRController::Init(const FName& trackingSource)
{
	Controller->SetTrackingMotionSource(trackingSource);
}

void UVRController::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto casted = Cast<AInteractableObject>(OtherActor);
	if (casted != nullptr) {
		hoverActor = OtherActor;
		casted->SetHighlightStatus(true);
	}
}

void UVRController::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == hoverActor) {
		hoverActor = nullptr;
		Cast<AInteractableObject>(OtherActor)->SetHighlightStatus(false);
	}
}

void UVRController::Grab()
{
	//attach the hover actor to this component
	if (hoverActor != nullptr) {
		auto casted = Cast<AInteractableObject>(hoverActor);
		casted->PickUp(Controller);
		casted->SetHighlightStatus(false);
	}
}

void UVRController::Release()
{
	if (hoverActor != nullptr) {
		Cast<AInteractableObject>(hoverActor)->Release();
	}
}

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
		LaunchVelocity, true, 0.0, ObjectTypes, false, TArray<AActor*>(), EDrawDebugTrace::ForOneFrame, 1.0, 30.0, 2.0, 0.0)) {

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
				DrawDebugSphere(GetWorld(), hitpos.Location, 5, 2, FColor::Red);
			}
		}
	}
	else {
		teleportTarget = FVector::ZeroVector;
	}
}

void UVRController::EndTeleport()
{
}

