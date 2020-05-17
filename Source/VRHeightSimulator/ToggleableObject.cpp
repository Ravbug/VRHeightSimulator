// Fill out your copyright notice in the Description page of Project Settings.


#include "ToggleableObject.h"

void AToggleableObject::PickUp(USceneComponent* newParent) {
	SetState(!GetState());

	if (isPickupable) {
		Super::PickUp(newParent);
	}
}
