// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "PlayerMovementComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "NetGame/GAS/PlayerAttributeSet.h"
#include "NetGame/GAS/BaseGameplayAbility.h"
#include "NetGame/GAS/GASAbilitySystemComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPlayerMovementComponent>(CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UGASAbilitySystemComponent>(TEXT("Ability Component"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UPlayerAttributeSet>(TEXT("Attributes"));
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

	// PlayerInputComponent->BindAction("Dodge", IE_Pressed, GetPlayerMovementComponent(), &UPlayerMovementComponent::DoDodge);
	PlayerInputComponent->BindAction<UPlayerMovementComponent::FSprintDelegate>
		("Sprint", IE_Pressed, GetPlayerMovementComponent(), &UPlayerMovementComponent::SetSprinting, true);
	PlayerInputComponent->BindAction<UPlayerMovementComponent::FSprintDelegate>
		("Sprint", IE_Released, GetPlayerMovementComponent(), &UPlayerMovementComponent::SetSprinting, false);

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


void APlayerCharacter::InitializeAttributes()
{
	if (AbilitySystemComponent == nullptr)
		return;

	if (!DefaultAttributes)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes for %s. Please fill in the character's Blueprint."), *FString(__FUNCTION__), *GetName());
		return;
	}

	// Can run on Server and Client
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	const FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, 1, EffectContext);
	if (NewHandle.IsValid())
	{
		AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
	}
}

void APlayerCharacter::GiveDefaultAbilities()
{
	if (!HasAuthority() || AbilitySystemComponent == nullptr)
		return;
	
	for (auto& StartupAbility : DefaultAbilities)
	{
		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(StartupAbility, 1, static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));	
	}
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent == nullptr || AttributeSet == nullptr)
		return;
	
		//	Server GAS Init
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	InitializeAttributes();
	GiveDefaultAbilities();

	AttributeSet->SetHealth(AttributeSet->GetMaxHealth());
	AttributeSet->SetStamina(AttributeSet->GetMaxStamina());
}

void APlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	if (AbilitySystemComponent == nullptr || AttributeSet == nullptr)
		return;
	
		//	Client GAS Init
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	InitializeAttributes();
	BindASCInput();
	
	AttributeSet->SetHealth(AttributeSet->GetMaxHealth());
	AttributeSet->SetStamina(AttributeSet->GetMaxStamina());
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
