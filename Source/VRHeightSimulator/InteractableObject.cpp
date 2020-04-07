// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableObject.h"

// Sets default values
AInteractableObject::AInteractableObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	rootMesh = CreateDefaultSubobject<UStaticMeshComponent>("Root Mesh");
	RootComponent = rootMesh;
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
	AttachToComponent(newParent, FAttachmentTransformRules::KeepWorldTransform);
	rootMesh->SetSimulatePhysics(false);
}

void AInteractableObject::Release() {
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	rootMesh->SetSimulatePhysics(true);
}