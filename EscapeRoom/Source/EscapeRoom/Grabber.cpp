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
	GetFirstPhysicsBodyInReach();
}

void UGrabber::GetPlayerViewPoint()
{
	// Get the player viewpoint this tick

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	// Log for testing
	///UE_LOG(LogTemp, Warning, TEXT("Location = %s, Rotation =%s"), 
	///	*PlayerViewPointLocation.ToString(), 
	///	*PlayerViewPointRotation.ToString());

	LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
}

void UGrabber::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab Key Pressed!"))

	/// Line trace and see  if we reach any actors with physics body collision channel set
	GetFirstPhysicsBodyInReach();

	/// If we hit something, then attach a physics handle.
	// TODO attach physics handle
}

void UGrabber::Release()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab Key Released!"))

	// TODO release physics handle
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



void UGrabber::FindPhysicsHandleComponent()
{
	///Look for attached Physics handle
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle)
	{
		// Physics handle is found
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s Has no Physics Handle."), *(GetOwner()->GetName()));
	}
}

void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		// Input Component is found
		UE_LOG(LogTemp, Warning, TEXT("InputComponent Found!"))
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

	GetPlayerViewPoint();

	if (isDebugActive)
	{
		// Draw a red trace in the world to visualize

		// TODO challenge: add debug line as a function that can be enabled or disabled by the player
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


	// if physics handle is attached
		// move the object that we're holding each frame

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
	return FHitResult();
}
