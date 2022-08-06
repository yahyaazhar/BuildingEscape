// Yahya Azhar

#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

#include "Grabber.h"

#define OUT		//definig a macro

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandle();
	SetupInputComponent();
}

//get the PhysicsHandler Componet
void UGrabber::FindPhysicsHandle() {
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle) {
		//do nothing
	}
	else {
		//if component is not attaceg log an error
		UE_LOG(LogTemp, Error, TEXT("%s dose not have UPhysicsHandleComponent!"),
			*(GetOwner()->GetName())
		);
	}
}

//setting up the components
void UGrabber::SetupInputComponent() {
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent) {
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		//action name define in setting, Input event, class ref which is this class, the function address that we want to bind

		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
}

void UGrabber::Grab() {
	UE_LOG(LogTemp, Warning, TEXT("E is pressed!"));

	//getting end of traceline
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	//OUT is black macro doing nothing. for keeping track that following are out parameter
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);
		//draw the line
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;


	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();

	//check that if we hit an actor
	if (HitResult.GetActor()) {
		PhysicsHandle->GrabComponentAtLocation(
			ComponentToGrab,
			NAME_None,
			LineTraceEnd
		);
	}
	
	//if we hit something then attach the physics handle
	//TODO attach physics handle
}
void UGrabber::Release() {
	UE_LOG(LogTemp, Warning, TEXT("E is released!"));
	//TODO remove physics handle
	PhysicsHandle->ReleaseComponent();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	//OUT is black macro doing nothing. for keeping track that following are out parameter
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;

	if (PhysicsHandle->GrabbedComponent) {
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
	}

}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const {
	//get the player viewport
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	//OUT is black macro doing nothing. for keeping track that following are out parameter
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	/*	UE_LOG(LogTemp, Warning, TEXT("Location: %s Rotation: %s "),
			*PlayerViewPointLocation.ToString(),
			*PlayerViewPointRotation.ToString()
		); */

		//draw the line
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;

	DrawDebugLine(
		GetWorld(),
		PlayerViewPointLocation,
		LineTraceEnd,
		FColor(0, 255, 0),
		false,
		0.f,
		0.f,
		5.f
	);


	//ray-cast to a distance
	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);

	//get the hit actor
	AActor* ActorHit = Hit.GetActor();

	if (ActorHit) {
		UE_LOG(LogTemp, Warning, TEXT("Hit %s: "), *(ActorHit->GetName()));
	}
	//see what hits

	return Hit;
}