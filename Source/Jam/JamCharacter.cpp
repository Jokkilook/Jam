// Copyright Epic Games, Inc. All Rights Reserved.

#include "JamCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "JamPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Public/PlayerHUD.h"
#include "Public/StatusComponent.h"
#include "Public/SkillProjectile.h"

AJamCharacter::AJamCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	StatusComponent = CreateDefaultSubobject<UStatusComponent>(TEXT("StatusComponent"));

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	Tags.Add("Player");
}

void AJamCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (!PlayerHUD)
	{
		if (PlayerHUDClass)
		{
			PlayerHUD = CreateWidget<UPlayerHUD>(GetWorld(), PlayerHUDClass);
			PlayerHUD->AddToViewport();
		}
	}

	if (StatusComponent)
	{
		StatusComponent->RefreshStatus();

		StatusComponent->OnDeath.AddDynamic(this, &AJamCharacter::OnDeath);
	}

	// 원본 머티리얼 저장
	for (int32 i = 0; i < GetMesh()->GetNumMaterials(); i++)
	{
		OriginalMaterials.Add(GetMesh()->GetMaterial(i));
	}
}

void AJamCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	/*
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		FHitResult HitResult;
        
		if (PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
		{
			FVector TargetLocation = HitResult.ImpactPoint;
			FVector CharacterLocation = GetActorLocation();
			
			TargetLocation.Z = CharacterLocation.Z;

			FRotator TargetRotation = (TargetLocation - CharacterLocation).Rotation();

			SetActorRotation(TargetRotation);
		}
	}
	*/
}

void AJamCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(Movement, ETriggerEvent::Triggered, this, &AJamCharacter::Move);
		EnhancedInputComponent->BindAction(TeleportAction, ETriggerEvent::Started, this, &AJamCharacter::Teleport);
		EnhancedInputComponent->BindAction(ManaBulletAction, ETriggerEvent::Started, this, &AJamCharacter::ManaBullet);
		EnhancedInputComponent->BindAction(FireBallAction, ETriggerEvent::Started, this, &AJamCharacter::FireBall);
		EnhancedInputComponent->BindAction(IceStormAction, ETriggerEvent::Started, this, &AJamCharacter::IceStorm);
		EnhancedInputComponent->BindAction(EarthQuakeAction, ETriggerEvent::Started, this, &AJamCharacter::EarthQuake);
		EnhancedInputComponent->BindAction(BindingAction, ETriggerEvent::Started, this, &AJamCharacter::Binding);
	}
}

void AJamCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	//UE_LOG(LogTemp, Warning, TEXT("MOVE X: %f, Y: %f"), MovementVector.X, MovementVector.Y);

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		AddMovementInput(ForwardDirection, MovementVector.Y * MovementEffector);
		AddMovementInput(RightDirection, MovementVector.X * MovementEffector);
	}
}

void AJamCharacter::Teleport()
{
	if (!CanTeleport) return;
	if (!StatusComponent) return;
	
	//마나 없음
	if (StatusComponent->GetCurrentMana() < TeleportManaUse)
	{
		return;
	}

	//쿨타임 전임
	if (GetWorldTimerManager().IsTimerActive(TeleportCoolTimer))
	{
		return;
	}
	
	StatusComponent->DecreaseMana(TeleportManaUse);
	
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	FHitResult CursorHit;
	// 1. 마우스 커서가 가리키는 월드 위치를 가져옵니다.
	if (PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit))
	{
		FVector StartLocation = GetActorLocation();
		FVector TargetLocation = CursorHit.ImpactPoint;

		// 2. 높이(Z)를 현재 캐릭터 높이로 고정하여 평면상의 방향만 계산합니다.
		TargetLocation.Z = StartLocation.Z;
		FVector TeleportDirection = (TargetLocation - StartLocation).GetSafeNormal();

		// 3. 마우스 거리와 상관없이 무조건 설정한 고정 거리만큼 최종 목적지 계산
		FVector EndLocation = StartLocation + (TeleportDirection * TeleportDistance);

		// 4. 벽 뚫기 방지용 전방 충돌 체크
		FHitResult TraceHit;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this); // 자기 자신은 제외

		// 현재 위치에서 목적지까지 레이저를 쏴서 벽(WorldStatic)이 막고 있는지 확인합니다.
		bool bHitObstacle = GetWorld()->LineTraceSingleByChannel(
			TraceHit, 
			StartLocation, 
			EndLocation, 
			ECC_WorldStatic, 
			QueryParams
		);

		FVector FinalTeleportLocation = EndLocation;

		if (bHitObstacle)
		{
			// 전방에 벽이 있다면 벽에 박히지 않도록 충돌 지점에서 살짝 뒤로 빼줍니다.
			FinalTeleportLocation = TraceHit.ImpactPoint - (TeleportDirection * 20.0f);
			FinalTeleportLocation.Z = StartLocation.Z; // 높이는 무조건 현재 높이 유지
		}

		// 5. 텔레포트 실행 (현재 높이 그대로 이동하므로 캡슐 높이 보정이 필요 없습니다)
		bool bSuccess = TeleportTo(FinalTeleportLocation, GetActorRotation());
        
		if (bSuccess)
		{
			UE_LOG(LogTemp, Warning, TEXT("평면 고정 거리 텔레포트 완료!"));
		}

		GetWorldTimerManager().SetTimer(
			TeleportCoolTimer,
			[](){},
			TeleportCoolTime,
			false);
	}
}

void AJamCharacter::ManaBullet()
{
	//스킬 사용 조건 체크==========
	if (!CanManaBullet) return;
	if (!StatusComponent) return;
	
	//마나 없음
	if (StatusComponent->GetCurrentMana() < ManaBulletManaUse)
	{
		return;
	}

	//쿨타임 전임
	if (GetWorldTimerManager().IsTimerActive(ManaBulletCoolTimer))
	{
		return;
	}
	
	if (!IsCoolZero) StatusComponent->DecreaseMana(ManaBulletManaUse);

	//스킬 사용 로직========= 이 밑으로 구현
	if (ManaBulletProjectileClass)
	{
		// 마우스 커서가 가리키는 월드 위치 구하기
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			FHitResult CursorHit;
			PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

			FVector TargetLocation = CursorHit.bBlockingHit
				? CursorHit.ImpactPoint
				: CursorHit.TraceEnd;

			// 발사 시작 위치: 캐릭터 높이 + 방향으로 캡슐 밖까지 밀어서 스폰
			FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 50.0f);
			FVector Direction = (TargetLocation - SpawnLocation).GetSafeNormal();
			// Z축은 평탄하게 유지 (탑다운 뷰)
			Direction.Z = 0.0f;
			Direction.Normalize();
			// 캡슐 반지름(42) + 발사체 반지름(20) + 여유분만큼 앞으로 밀어서 스폰
			SpawnLocation += Direction * 70.0f;

			FRotator SpawnRotation = Direction.Rotation();

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = this;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			ASkillProjectile* Projectile = GetWorld()->SpawnActor<ASkillProjectile>(
				ManaBulletProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
			
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Class : %s"), Projectile ? *Projectile->GetClass()->GetName() : TEXT("None")));
			if (Projectile)
			{
				Projectile->FireInDirection(Direction);
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Projectile : %s"), *Projectile->GetName()));
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[PLAYER] ManaBullet Used"))

	
	//스킬 사용 후 쿨타임 작동 
	GetWorldTimerManager().SetTimer(
			ManaBulletCoolTimer,
			[](){},
			ManaBulletCoolTime,
			false);
}

void AJamCharacter::FireBall()
{
	//스킬 사용 조건 체크==========
	if (!CanFireBall) return;
	if (!StatusComponent) return;
	
	//마나 없음
	if (StatusComponent->GetCurrentMana() < FireBallManaUse)
	{
		return;
	}

	//쿨타임 전임
	if (GetWorldTimerManager().IsTimerActive(FireBallCoolTimer))
	{
		return;
	}
	
	if (!IsCoolZero) StatusComponent->DecreaseMana(FireBallManaUse);

	//스킬 사용 로직========= 이 밑으로 구현
	UE_LOG(LogTemp, Warning, TEXT("[PLAYER] FireBall Used"))

	StackDiscord();
	
	//스킬 사용 후 쿨타임 작동 
	GetWorldTimerManager().SetTimer(
			FireBallCoolTimer,
			[](){},
			FireBallCoolTime,
			false);
}

void AJamCharacter::IceStorm()
{
	//스킬 사용 조건 체크==========
	if (!CanIceStorm) return;
	if (!StatusComponent) return;
	
	//마나 없음
	if (StatusComponent->GetCurrentMana() < IceStormManaUse)
	{
		return;
	}

	//쿨타임 전임
	if (GetWorldTimerManager().IsTimerActive(IceStormCoolTimer))
	{
		return;
	}
	
	if (!IsCoolZero) StatusComponent->DecreaseMana(IceStormManaUse);

	//스킬 사용 로직========= 이 밑으로 구현
	UE_LOG(LogTemp, Warning, TEXT("[PLAYER] IceStorm Used"))

	StackDiscord();
	
	//스킬 사용 후 쿨타임 작동 
	GetWorldTimerManager().SetTimer(
			IceStormCoolTimer,
			[](){},
			IceStormCoolTime,
			false);
}

void AJamCharacter::EarthQuake()
{
	//스킬 사용 조건 체크==========
	if (!CanEarthQuake) return;
	if (!StatusComponent) return;
	
	//마나 없음
	if (StatusComponent->GetCurrentMana() < EarthQuakeManaUse)
	{
		return;
	}

	//쿨타임 전임
	if (GetWorldTimerManager().IsTimerActive(EarthQuakeCoolTimer))
	{
		return;
	}
	
	if (!IsCoolZero) StatusComponent->DecreaseMana(EarthQuakeManaUse);

	//스킬 사용 로직========= 이 밑으로 구현
	UE_LOG(LogTemp, Warning, TEXT("[PLAYER] EarthQuake Used"))

	StackDiscord();
	
	//스킬 사용 후 쿨타임 작동 
	GetWorldTimerManager().SetTimer(
			EarthQuakeCoolTimer,
			[](){},
			EarthQuakeCoolTime,
			false);
}

void AJamCharacter::Binding()
{
	//스킬 사용 조건 체크==========
	if (!CanBinding) return;
	if (!StatusComponent) return;
	
	//마나 없음
	if (StatusComponent->GetCurrentMana() < BindingManaUse)
	{
		return;
	}

	//쿨타임 전임
	if (GetWorldTimerManager().IsTimerActive(BindingCoolTimer))
	{
		return;
	}
	
	if (!IsCoolZero) StatusComponent->DecreaseMana(BindingManaUse);

	//스킬 사용 로직========= 이 밑으로 구현
	UE_LOG(LogTemp, Warning, TEXT("[PLAYER] Binding Used"))

	StackDiscord();
	
	//스킬 사용 후 쿨타임 작동 
	GetWorldTimerManager().SetTimer(
			BindingCoolTimer,
			[](){},
			BindingCoolTime,
			false);
}

void AJamCharacter::GodMode()
{
	AttackRate = 0.0f;
	
	GetWorldTimerManager().SetTimer(
		GodRemainingTimer,
		this,
		&AJamCharacter::GodModeOff,
		GodRemainingTime);
}

void AJamCharacter::GodModeOff()
{
	AttackRate = 1.0f;

	GetWorldTimerManager().SetTimer(
		GodCoolTimer,
		this,
		&AJamCharacter::GodModeOff,
		GodCoolTime);
}

void AJamCharacter::StartGod()
{
	GodMode();
}

void AJamCharacter::CoolZero()
{
	IsCoolZero = true;
	
	TeleportCoolTime = 0.1f;
	ManaBulletCoolTime = 0.1f;
	FireBallCoolTime = 0.1f;
	IceStormCoolTime = 0.1f;
	EarthQuakeCoolTime = 0.1f;
	BindingCoolTime = 0.1f;

	GetWorldTimerManager().SetTimer(
		CoolZeroRemainingTimer,
		this,
		&AJamCharacter::CoolZeroOff,
		CoolZeroRemainingTime);
}

void AJamCharacter::CoolZeroOff()
{
	IsCoolZero = false;
	
	TeleportCoolTime = 1.0f;
	ManaBulletCoolTime = 0.5f;
	FireBallCoolTime = 5.0f;
	IceStormCoolTime = 15.0f;
	EarthQuakeCoolTime = 25.0f;
	BindingCoolTime = 40.0f;

	GetWorldTimerManager().SetTimer(
		CoolZeroCoolTimer,
		this,
		&AJamCharacter::CoolZeroOff,
		CoolZeroCoolTime);
}

void AJamCharacter::StartCoolZero()
{
	CoolZero();
}

void AJamCharacter::OnDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("[PLAYER] DEAD!!!!!!"))
}

void AJamCharacter::ApplyRandomDebuff()
{
	if (DebuffList.Num() == 0) return;

	UE_LOG(LogTemp, Warning, TEXT("[PLAYER] Pick Debuff"))
	int32 RandomIndex = FMath::RandRange(0, DebuffList.Num() - 1);
	UE_LOG(LogTemp, Warning, TEXT("[PLAYER] Debuff %d"), RandomIndex)

	switch (RandomIndex)
	{
		case 0:
			HealthDebuff();
			break;
		case 1:
			ManaDebuff();
			break;
		case 2:
			SpeedDebuff();
			break;
		case 3:
			AttackDebuff();
			break;
		case 4:
			DamageDebuff();
			break;
		default:
			break;
	}
}

void AJamCharacter::HealthDebuff()
{
	if (StatusComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PLAYER] Health Debuff"));
		StatusComponent->DecreaseHealth(HealthDebuffAmount);
	}
}

void AJamCharacter::ManaDebuff()
{
	if (StatusComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PLAYER] Mana Debuff"));
		StatusComponent->DecreaseMana(ManaDebuffAmount);
	}
}

void AJamCharacter::SpeedDebuff()
{
	UE_LOG(LogTemp, Warning, TEXT("[PLAYER] Speed Debuff"));
	MovementEffector = 1.0f - (SpeedDebuffRate / 100.0f);

	GetWorldTimerManager().SetTimer(
		SpeedDebuffTimer,
		this,
		&AJamCharacter::RestoreSpeed,
		DebuffRemainingTime);
}

void AJamCharacter::AttackDebuff()
{
	UE_LOG(LogTemp, Warning, TEXT("[PLAYER] Attacking Debuff"));
	AttackRate = 1.0f - (AttackDebuffRate / 100.0f);

	GetWorldTimerManager().SetTimer(
		AttackDebuffTimer,
		this,
		&AJamCharacter::RestoreAttack,
		DebuffRemainingTime);
}

void AJamCharacter::DamageDebuff()
{
	UE_LOG(LogTemp, Warning, TEXT("[PLAYER] Damage Debuff"));
	DamageRate = 1.0f - (DamageDebuffRate / 100.0f);

	GetWorldTimerManager().SetTimer(
		DamageDebuffTimer,
		this,
		&AJamCharacter::RestoreDamage,
		DebuffRemainingTime);
}

void AJamCharacter::RestoreSpeed()
{
	UE_LOG(LogTemp, Warning, TEXT("[PLAYER] Restore Speed"));
	MovementEffector = 1.0f;
}

void AJamCharacter::RestoreAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Restore Attacking"))
	AttackRate = 1.0f;
}

void AJamCharacter::RestoreDamage()
{
	UE_LOG(LogTemp, Warning, TEXT("Restore Damage"))
	DamageRate = 1.0f;
}

void AJamCharacter::StackDiscord()
{
	if (++DiscordStack == 3)
	{
		DiscordStack = 0;
		ApplyRandomDebuff();
	}
}

float AJamCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (StatusComponent && ActualDamage > 0)
	{
		StatusComponent->DecreaseHealth(ActualDamage);

		if (StatusComponent->GetCurrentHealth() <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player is dead"));
		}
		else
		{
			// 타격 플래시 - HitMaterial로 교체 후 0.1초 뒤 복구
			if (HitMaterial)
			{
				for (int32 i = 0; i < GetMesh()->GetNumMaterials(); i++)
				{
					GetMesh()->SetMaterial(i, HitMaterial);
				}
				GetWorldTimerManager().SetTimer(HitFlashTimerHandle, this, &AJamCharacter::RestoreOriginalMaterials, 0.1f, false);
			}
		}
	}

	return ActualDamage;
}

void AJamCharacter::RestoreOriginalMaterials()
{
	for (int32 i = 0; i < OriginalMaterials.Num(); i++)
	{
		GetMesh()->SetMaterial(i, OriginalMaterials[i]);
	}
}
