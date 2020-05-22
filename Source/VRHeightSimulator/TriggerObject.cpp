// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerObject.h"

void ATriggerObject::SetState(bool newState) {
	Super::SetState(newState);

	for (const auto& obj : outputs) {
		obj->SetState(newState);
	}
}

void ATriggerObject::SetHighlightStatus(bool newStatus)
{
	Super::SetHighlightStatus(newStatus);

	//also highlight child objects
	for (const auto& obj : outputs) {
		obj->SetHighlightStatus(newStatus);
	}
}
