// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableObject.h"

// Sets default values
AInteractableObject::AInteractableObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	rootMesh = CreateDefaultSubobject<UStaticMeshComponent>("Root Mesh");
	RootComponent = rootMesh;

	InteractionLabel_A = CreateDefaultSubobject<UWidgetComponent>("InstructionLabel");
	InteractionLabel_A->SetupAttachment(rootMesh);
	//InteractionLabel_A->SetWidgetSpace(EWidgetSpace::Screen);
}

// Called when the game starts or when spawned
void AInteractableObject::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AInteractableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractableObject::PickUp(USceneComponent* newParent) {
	rootMesh->SetSimulatePhysics(false);
	AttachToComponent(newParent, FAttachmentTransformRules::KeepWorldTransform);
}

void AInteractableObject::Release() {
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	rootMesh->SetSimulatePhysics(true);
}

void AInteractableObject::SetHighlightStatus(bool newStatus) {
	//enable / disable highlighting and set the stencil value if true
	rootMesh->SetRenderCustomDepth(newStatus);
	rootMesh->SetCustomDepthStencilValue(newStatus? 2 : 0);
}