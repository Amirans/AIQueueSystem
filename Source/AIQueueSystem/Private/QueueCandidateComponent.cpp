// Fill out your copyright notice in the Description page of Project Settings.

#include "QueueCandidateComponent.h"
#include "QueueManager.h"

// Sets default values for this component's properties
UQueueCandidateComponent::UQueueCandidateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

bool UQueueCandidateComponent::TryRegiserQueue(AQueueManager* NewQueueManager)
{
	/* Validate Queue Manager */
	if (!IsValid(NewQueueManager))
	{
		return false;
	}

	/* Remove From Previous Queue , If Part of Any */
	if (IsValid(QueueManager))
	{
		RemoveFromQueue();
	}

	return NewQueueManager->TryRegisterCandidate(this);
}

void UQueueCandidateComponent::OnAddedToQueue(AQueueManager* NewQueueManager)
{
	/* Validate Queue Manager */
	if (!IsValid(NewQueueManager))
	{
		return;
	}

	/* Set New Queue Manager */
	QueueManager = NewQueueManager;

	/* Broadcast Queue Change Update */
	if (OnCandidateQueueChanged.IsBound())
	{
		OnCandidateQueueChanged.Broadcast();
	}
}

void UQueueCandidateComponent::RemoveFromQueue()
{
	/* Validate Queue Manager */
	if (IsValid(QueueManager))
	{
		/* Deregister Self from the Queue */
		QueueManager->DeregisterCandidate(this);
	}
	else
	{
		/* If Queue Manager is Not Valid Call Remove to Clean up*/
		OnRemovedFromQueue();
	}
}

void UQueueCandidateComponent::OnRemovedFromQueue()
{
	/* Remove Queue Manager */
	QueueManager = nullptr;

	/* Broadcast Delegate */
	if (OnCandidateQueueChanged.IsBound())
	{
		OnCandidateQueueChanged.Broadcast();
	}
}

void UQueueCandidateComponent::UpdateQueueSlot(FTransform NewSlotTransform, const AQueueManager* InstigatorQueue)
{
	/*  Validate We are in the Same Queue */
	if (InstigatorQueue != QueueManager && IsValid(QueueManager))
	{
		/* Mark self Dirty to be Removed */
		QueueManager->HandleDirtyCandidate(this);
		return;
	}

	/* Set New Transform Queue */
	SlotTransform = NewSlotTransform;

	/* Broadcast new Slot Transform */
	if (OnCandidateQueueSlotChanged.IsBound())
	{
		OnCandidateQueueSlotChanged.Broadcast(SlotTransform);
	}

}

FTransform UQueueCandidateComponent::GetSlotTransform() const
{
	if (IsInQueue())
	{
		/* Return Slot Transform If Part of a Queue */
		return SlotTransform;
	}
	else
	{
		/* Return Component Owner Transform if Not Part of a Queue */
		return IsValid(GetOwner()) ? GetOwner()->GetActorTransform() : FTransform();
	}
}

int32 UQueueCandidateComponent::GetQueueSlotIndex() const
{
	return IsValid(QueueManager) ? QueueManager->GetCandidateSlotIndex(this) : -1;
}

// Called when the game starts
void UQueueCandidateComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UQueueCandidateComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	/** Remove Self from Any Queue */
	RemoveFromQueue();

	Super::EndPlay(EndPlayReason);
}

