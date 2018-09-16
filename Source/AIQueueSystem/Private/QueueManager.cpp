// Fill out your copyright notice in the Description page of Project Settings.

#include "QueueManager.h"
#include "QueueCandidateComponent.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"

// Sets default values
AQueueManager::AQueueManager(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	/* Create Spline Component */
	QueueSplineComp = ObjectInitializer.CreateDefaultSubobject<USplineComponent>(this,TEXT("QueueSpline"));
	QueueSplineComp->SetupAttachment(RootComponent);

	/* Default Slot Values */
	SlotNum = 0;
	SlotOffsetLength = 0.0f;
	SlotLength = 100.0f;
	
	/* Generate Queue Slots */
	GenerateQueueSlots();

}

// Called when the game starts or when spawned
void AQueueManager::BeginPlay()
{
	Super::BeginPlay();
}

void AQueueManager::GenerateQueueSlots()
{
	/* Validate Spline Comp */
	if (!IsValid(QueueSplineComp))
	{
		return;
	}

	/* Get Total Length of Spline, End Point does not require Offset */
	const float QueueLength = QueueSplineComp->GetSplineLength() + SlotOffsetLength;

	/* Calculate Slot Length + Offset */
	const float LengthPerSlot = SlotLength + SlotOffsetLength;

	/* Calculate How Many Slot can Fit in */
	SlotNum = QueueLength / LengthPerSlot;
}

bool AQueueManager::TryRegisterCandidate(UQueueCandidateComponent* QueueCandidateComp, const bool bForceRegister /*= false*/)
{

	/* Validate Candidate */
	if (!IsValid(QueueCandidateComp))
	{
		return false;
	}

	/* Whether Queue Can Accept New Candidate */
	if (CanAcceptCandidate() == false)
	{
		return false;
	}

	/* Check If Already in Queue */
	if (bForceRegister == false &&
		QueueCandidates.Contains(QueueCandidateComp))
	{
		return false;
	}
	
	/* Add Candidate to the Queue */
	const int32 CandidateIndex = QueueCandidates.Add(QueueCandidateComp);

	/* Broadcast New Candidate and Return True if Added */
	if (CandidateIndex != -1)
	{
		/* Notify Candidate Of Registration */
		QueueCandidateComp->OnAddedToQueue(this);

		/* Broadcast Queue Candidate Added */
		OnQueueCandidateUpdated.Broadcast(true);

		/* Update Candidate Slot */
		UpdateCandidateSlot(QueueCandidateComp, CandidateIndex);

		return true;
	}

	return false;
}

void AQueueManager::DeregisterCandidate(UQueueCandidateComponent* QueueCandidateComp)
{
	/* Try to Remove Dirty Candidate*/
	if (HandleDirtyCandidate(QueueCandidateComp))
	{
		/* Notify Candidate of Removal */
		QueueCandidateComp->OnRemovedFromQueue();
	}
}

bool AQueueManager::HandleDirtyCandidate(UQueueCandidateComponent* QueueCandidateComp)
{
	/* Validate Queue Candidate */
	if (!IsValid(QueueCandidateComp))
	{
		return false;
	}

	/* Verify Queue Contains this Candidate */
	if (!QueueCandidates.Contains(QueueCandidateComp))
	{
		return false;
	}

	/* Remove the Candidate from the List */
	const bool bWasRemoved = QueueCandidates.Remove(QueueCandidateComp) > 0;

	/* Update and Return True if Removal was Successful */
	if (bWasRemoved)
	{
		/* Broadcast Removal of Candidate from Queue*/
		OnQueueCandidateUpdated.Broadcast(false);

		/* Update Remaining Candidates */
		UpdateQueueCandidatesSlot();

		return true;
	}

	return false;
}

UQueueCandidateComponent* AQueueManager::Dequeue()
{
	/* Validate We have At least On Candidate in the Queue */
	if (!QueueCandidates.IsValidIndex(0))
	{
		return nullptr;
	}

	/* Cache the First Element */
	UQueueCandidateComponent* FirstCandidate = QueueCandidates[0];

	/* De Register Candidate */
	DeregisterCandidate(FirstCandidate);

	return FirstCandidate;
}

int32 AQueueManager::GetCandidateSlotIndex(const UQueueCandidateComponent* QueueCandidateComp) const
{
	/* Iterate Through Return the Index of the Candidate */
	for (int32 i = 0; i < QueueCandidates.Num(); i++)
	{
		if (QueueCandidates[i] == QueueCandidateComp)
		{
			return i;
		}
	}

	return -1;
}

void AQueueManager::UpdateQueueCandidatesSlot()
{
	TArray<UQueueCandidateComponent*> CandidatesToRemove;

	/* Iterate Through the Candidates */
	for (int32 i = 0; i < QueueCandidates.Num(); i++)
	{
		/* Add Candidates Above the Slots to Remove List */
		if (i >= SlotNum)
		{
			CandidatesToRemove.Add(QueueCandidates[i]);
			continue;
		}

		UpdateCandidateSlot(QueueCandidates[i], i);
	}

	/* Remove Candidates that are not fitting to Queue anymore */
	for (int32 r = 0; r < CandidatesToRemove.Num(); r++)
	{
		DeregisterCandidate(CandidatesToRemove[r]);
	}
}

void AQueueManager::UpdateCandidateSlot(UQueueCandidateComponent* UpdatingCandidate, const int32& CandidateIndex)
{
	/* Validate Candidate */
	if (UpdatingCandidate == nullptr)
	{
		return;
	}

	/* Get Candidate New Position */
	FTransform NewTransform = GetTransformAtSlot(CandidateIndex);

	UpdatingCandidate->UpdateQueueSlot(NewTransform, this);
}

FTransform AQueueManager::GetTransformAtSlot(const int32 SlotIndex) const
{
	/** Calculate Length at Index, Should not add Offset as 0 is the Last Slot */
	const float SlotDistance = SlotIndex == 0 ? SlotIndex * SlotLength : SlotIndex * (SlotLength + SlotOffsetLength);

	/* Return Transform at Slot Distance */
	return QueueSplineComp->GetTransformAtDistanceAlongSpline(
		QueueSplineComp->GetSplineLength() - SlotDistance,ESplineCoordinateSpace::World
	);
}

bool AQueueManager::CanAcceptCandidate() const
{
	return SlotNum > QueueCandidates.Num();
}

