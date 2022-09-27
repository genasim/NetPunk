// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "PlayerMovementComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "NetGame/GAS/BaseGameplayAbility.h"
#include "NetGame/GAS/GASAbilitySystemComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPlayerMovementComponent>(CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UGASAbilitySystemComponent>(TEXT("Ability Component"));
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(Cast<APlayerController>(Controller));

	LoadSavedParameters();
}

void APlayerCharacter::MoveForward(float InputAxisValue)
{
	InputVector.Y = InputAxisValue;
	if (Controller == nullptr || !InputAxisValue)
		return;
	
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0,Rotation.Yaw,0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, InputAxisValue);
}

void APlayerCharacter::MoveRight(float InputAxisValue)
{
	InputVector.X = InputAxisValue;
	if (Controller == nullptr || !InputAxisValue)
		return;
	
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0,Rotation.Yaw,0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, InputAxisValue);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	BindASCInput();
}


///////////////////////////////////////////////////////
///		Saving System

void APlayerCharacter::OnBeforeSave_Implementation()
{
	ISaveInterface::OnBeforeSave_Implementation();
	if (!IsHostCharacter())
		return;
	
	PlayerTransform = GetActorTransform();
}

void APlayerCharacter::LoadSavedParameters()
{
	 if (!IsHostCharacter())
	 	return;					
	
	if (PlayerTransform.GetLocation() != FVector::ZeroVector)
		SetActorTransform(PlayerTransform);
}

///////////////////////////////////////////////////////
///		GameplayAbilities System

UAbilitySystemComponent* APlayerCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent == nullptr)
		return;
	
		//	Server GAS Init
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	AbilitySystemComponent->ApplyDefaultEffects();
	AbilitySystemComponent->GiveDefaultAbilities();
}

void APlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	if (AbilitySystemComponent == nullptr)
		return;
	
		//	Client GAS Init
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	AbilitySystemComponent->ApplyDefaultEffects();
	BindASCInput();
}

void APlayerCharacter::BindASCInput()
{
	if (AbilitySystemComponent == nullptr || InputComponent == nullptr || bIsASCInputBound)
		return;

	const FGameplayAbilityInputBinds Binds(
		"Confirm",
		"Cancel",
		"EGASAbilityInputID",
		static_cast<int32>(EGASAbilityInputID::Confirm),
		static_cast<int32>(EGASAbilityInputID::Cancel));
	AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, Binds);

	bIsASCInputBound = true;
}

///////////////////////////////////////////////////////
///		Helper Functions

bool APlayerCharacter::IsHostCharacter() const
{
	return GetLocalRole() == ROLE_Authority && IsLocallyControlled();
}

UPlayerMovementComponent* APlayerCharacter::GetPlayerMovementComponent() const
{
	return static_cast<UPlayerMovementComponent*>(GetCharacterMovement());
}
