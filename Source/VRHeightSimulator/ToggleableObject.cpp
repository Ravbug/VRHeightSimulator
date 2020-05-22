// Fill out your copyright notice in the Description page of Project Settings.


#include "ToggleableObject.h"
#include "Kismet/GameplayStatics.h"

AToggleableObject::AToggleableObject() {
	InteractionLabel_B = CreateDefaultSubobject<UWidgetComponent>("Interaction Label");
	InteractionLabel_B->SetupAttachment(rootMesh);
	//InteractionLabel_B->SetWidgetSpace(EWidgetSpace::Screen);
}

void AToggleableObject::PickUp(USceneComponent* newParent) {
	if (isPickupable) {
		Super::PickUp(newParent);
	}
}

void AToggleableObject::SetHighlightStatus(bool newState)
{
	//call hooks if close to the player
	auto player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (FVector::Distance(GetActorLocation(), player->GetActorLocation()) < 50) {
		HighlightChange(newState);
	}
	Super::SetHighlightStatus(newState);
}