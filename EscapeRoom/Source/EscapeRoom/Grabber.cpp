// Fill out your copyright notice in the Description page of Project Settings.

#include "EscapeRoom.h"
#include "Grabber.h"

#define OUT


// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsHandleComponent();
	SetupInputComponent();
}

void UGrabber::GetPlayerViewPoint()
{
	// Get the player viewpoint this tick

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);



	LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
}


void UGrabber::FindPhysicsHandleComponent()
{
	///Look for attached Physics handle
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Has no Physics Handle."), *(GetOwner()->GetName()));
	}
}

void UGrabber::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab Key Pressed!"))

	/// Line trace and see  if we reach any actors with physics body collision channel set
	auto HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent();
	auto ActorHit = HitResult.GetActor();

	/// If we hit something, then attach a physics handle.
	if (ActorHit)
	{
		if (!PhysicsHandle) { return; }
		UE_LOG(LogTemp, Warning, TEXT("ACTOR HIT SUCCESSFULL!"))
		//  attach physics handle
		PhysicsHandle->GrabComponent(ComponentToGrab, NAME_None, ComponentToGrab->GetOwner()->GetActorLocation(), true);

	}
	


}

void UGrabber::Release()
{
	if (!PhysicsHandle) { return; }
	UE_LOG(LogTemp, Warning, TEXT("Grab Key Released!"))

		//  release physics handle
		PhysicsHandle->ReleaseComponent();
}

void UGrabber::ActivateDebug()
{
	

	if (!isDebugActive)
	{
		isDebugActive = true;
		UE_LOG(LogTemp, Warning, TEXT("Debug line is now active"));
	}
	else
	{
		isDebugActive = false;
		UE_LOG(LogTemp, Warning, TEXT("Debug line is now inactive"));
	}

}




void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{		
			/// Bind Input axis
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
		InputComponent->BindAction("DebugLine", IE_Pressed, this, &UGrabber::ActivateDebug);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s has no InputComponent"), *(GetOwner()->GetName()))
	}

	// ...
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PhysicsHandle) { return; }

	GetPlayerViewPoint();
	// if physics handle is attached
	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
	}
	// move the object that we're holding each frame


	if (isDebugActive)
	{
		// Draw a red trace in the world to visualize
		DrawDebugLine(
			GetWorld(),
			PlayerViewPointLocation,
			LineTraceEnd,
			FColor(255, 0, 0),
			false,
			0.f,
			0.f,
			10.f
		);
	}




}


const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	GetPlayerViewPoint();

	// Setup Query parameters.
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());

	// Raycast out to reach distance
	FHitResult Hit;

	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);

	// See what we hit

	AActor* ActorHit = Hit.GetActor();
	if (ActorHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor Hit = %s"), *(ActorHit->GetName()));
	}


	// ...
	return Hit;
}
