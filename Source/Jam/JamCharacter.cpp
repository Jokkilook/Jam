// Copyright Epic Games, Inc. All Rights Reserved.

#include "JamCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "SkillProjectile.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Public/PlayerHUD.h"
#include "Public/StatusComponent.h"
#include "Engine/OverlapResult.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

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

		StatusComponent->OnLevelChanged.AddDynamic(this, &AJamCharacter::LevelUp);
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
	}

	return ActualDamage;
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
	if (!IsCoolZero && StatusComponent->GetCurrentMana() < TeleportManaUse)
	{
		return;
	}

	//쿨타임 전임
	if (GetWorldTimerManager().IsTimerActive(TeleportCoolTimer))
	{
		return;
	}
	
	if (!IsCoolZero) StatusComponent->DecreaseMana(TeleportManaUse);

	if (TeleportSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, TeleportSound, GetActorLocation());
	}
	
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	FHitResult CursorHit;
	// 1. 마우스 커서가 가리키는 월드 위치를 가져옵니다.
	if (PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit))
	{
		FVector StartLocation = GetActorLocation();
		FVector TargetLocation = CursorHit.ImpactPoint;

		SpawnEffect(TeleportEffect, StartLocation);

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
			SpawnEffect(TeleportEffect, FinalTeleportLocation);
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
	if (!IsCoolZero && StatusComponent->GetCurrentMana() < ManaBulletManaUse)
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
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			FHitResult CursorHit;
			PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

			FVector TargetLocation = CursorHit.bBlockingHit ? CursorHit.ImpactPoint : CursorHit.TraceEnd;

			FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 50.0f);
			FVector Direction = (TargetLocation - SpawnLocation).GetSafeNormal();
			Direction.Z = 0.0f;
			Direction.Normalize();
			SpawnLocation += Direction * 70.0f;

			FRotator SpawnRotation = Direction.Rotation();

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = this;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			ASkillProjectile* Projectile = GetWorld()->SpawnActor<ASkillProjectile>(
				ManaBulletProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

			if (Projectile)
			{
				Projectile->FireInDirection(Direction);
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[PLAYER] ManaBullet Used"))

	if (ManaBulletSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ManaBulletSound, GetActorLocation());
	}
	
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
	if (!IsCoolZero && StatusComponent->GetCurrentMana() < FireBallManaUse)
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

	if (FireBallProjectileClass)
	{
		if (FireBallSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireBallSound, GetActorLocation());
		}

		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			FHitResult CursorHit;
			PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

			FVector TargetLocation = CursorHit.bBlockingHit ? CursorHit.ImpactPoint : CursorHit.TraceEnd;

			FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 50.0f);
			FVector Direction = (TargetLocation - SpawnLocation).GetSafeNormal();
			Direction.Z = 0.0f;
			Direction.Normalize();
			SpawnLocation += Direction * 70.0f;

			FRotator SpawnRotation = Direction.Rotation();

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = this;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			ASkillProjectile* Projectile = GetWorld()->SpawnActor<ASkillProjectile>(
				FireBallProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

			if (Projectile)
			{
				Projectile->FireInDirection(Direction);
			}
		}
	}
	
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
	if (!IsCoolZero && StatusComponent->GetCurrentMana() < IceStormManaUse)
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

	if (IceStormSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, IceStormSound, GetActorLocation());
	}
	
	APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;

    FHitResult CursorHit;
    // 1. 마우스 커서의 3D 월드 좌표를 가져옵니다.
    if (PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit))
    {
	    FVector StartLocation = GetActorLocation();
    	FVector TargetLocation = CursorHit.ImpactPoint;

    	// 거리를 정확하게 평면(X, Y) 기준으로만 계산하기 위해 Z축을 맞춰줍니다.
    	TargetLocation.Z = StartLocation.Z;

    	// 2. 캐릭터와 마우스 클릭 지점 사이의 실제 거리 계산
    	float DistanceToTarget = FVector::Dist(StartLocation, TargetLocation);

    	FVector FinalAttackCenter = TargetLocation;

    	// 3. [핵심 조건] 최대 사거리(x)를 벗어났는지 체크
    	if (DistanceToTarget > IceStormMaxRangeRadius)
    	{
    		// 마우스 방향의 단위 벡터(길이 1짜리)를 구합니다.
    		FVector AttackDirection = (TargetLocation - StartLocation).GetSafeNormal();
            
    		// 현재 위치에서 해당 방향으로 '최대 사거리(MaxAttackRange)'만큼만 전진한 위치를 중심점으로 잡습니다.
    		FinalAttackCenter = StartLocation + (AttackDirection * IceStormMaxRangeRadius);
    	}

    	SpawnEffect(IceStormEffect, FinalAttackCenter);
    	
    	// 4. 범위(반지름 n) 내의 타겟들을 감지 (OverlapMultiByChannel 사용)
    	TArray<FHitResult> HitResults;
    	FCollisionShape SphereShape = FCollisionShape::MakeSphere(IceStormAttackRadius); // 반지름 n 짜리 구체 생성
    	FCollisionQueryParams QueryParams;
    	QueryParams.AddIgnoredActor(this); // 시전자 제외

    	// ECC_Pawn(캐릭터/몬스터) 채널을 대상으로 범위 검사
    	bool bHit = GetWorld()->SweepMultiByChannel(
			HitResults,
			FinalAttackCenter,
			FinalAttackCenter, // 시작과 끝을 같게 하면 그 자리에서 구체 검사를 합니다.
			FQuat::Identity,
			ECC_Pawn,
			SphereShape,
			QueryParams
		);

    	// 5. 디버그용 원 그리기 (에디터에서 범위 시각적으로 확인용)
    	UKismetSystemLibrary::DrawDebugCircle(
			GetWorld(),
			FinalAttackCenter,
			IceStormAttackRadius,
			32,
			FLinearColor::Red,
			2.0f, // 2초 동안 표시
			1.0f,
			FVector(0, 0, 1) // 하늘 방향(Z축)을 기준으로 원을 그림
		);

    	// 6. 감지된 적들에게 데미지 적용
    	if (bHit)
    	{
    		for (const FHitResult& Hit : HitResults)
    		{
    			AActor* HitActor = Hit.GetActor();
    			if (HitActor)
    			{
    				UGameplayStatics::ApplyDamage(
    					HitActor,
    					IceStormDamage,
    					GetController(),
    					this,
    					UDamageType::StaticClass());
    			}
    		}
    	}
    }

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
	if (!IsCoolZero && StatusComponent->GetCurrentMana() < EarthQuakeManaUse)
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

	if (EarthQuakeSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EarthQuakeSound, GetActorLocation());
	}

	SpawnEffect(EarthQuakeEffect, GetActorLocation());

	FVector CenterLocation = GetActorLocation();
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(EarthQuakeRadius);
    
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bHasHits = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		CenterLocation,
		FQuat::Identity,
		ECC_Pawn, // 대상을 감지할 콜리전 채널 (보통 몬스터는 Pawn)
		SphereShape,
		QueryParams
	);

	// 4. [디버그용] 에디터 화면에 빨간색 원형으로 범위 그려서 시각화 확인
	UKismetSystemLibrary::DrawDebugCircle(
		GetWorld(),
		CenterLocation,
		EarthQuakeRadius,
		32, // 원을 구성할 선의 개수 (많을수록 부드러움)
		FLinearColor::Red,
		1.5f, // 1.5초 동안 화면에 표시
		2.0f, // 선 두께
		FVector(0, 0, 1) // 평면에 그리기 위해 하늘 방향(Z축) 벡터 지정
	);

	// 5. 데미지 적용하기
	if (bHasHits)
	{
		// 중복 데미지 방지를 위해 이미 데미지를 준 액터들을 체크할 배열 (동일 타겟 중복 감지 방지)
		TArray<AActor*> DamagedActors;

		for (const FOverlapResult& Overlap : OverlapResults)
		{
			AActor* HitActor = Overlap.GetActor();
			if (HitActor && !DamagedActors.Contains(HitActor))
			{
				DamagedActors.Add(HitActor);

				// 언리얼 표준 데미지 시스템으로 주변 적들에게 피해 전달
				UGameplayStatics::ApplyDamage(
					HitActor,
					EarthQuakeDamage,
					GetController(),
					this,
					UDamageType::StaticClass()
				);

				UE_LOG(LogTemp, Warning, TEXT("지진 마법 적중! 대상: %s"), *HitActor->GetName());
			}
		}
	}
	
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
	if (!IsCoolZero && StatusComponent->GetCurrentMana() < BindingManaUse)
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

	if (BindingSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, BindingSound, GetActorLocation());
	}

	// Boss 태그를 가진 액터를 찾아 BT 정지
	TArray<AActor*> BossActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Boss"), BossActors);

	// 나이아가라 컴포넌트 레퍼런스 수집 (종료 시 비활성화용)
	TArray<UNiagaraComponent*> SpawnedEffects;

	for (AActor* Boss : BossActors)
	{
		ACharacter* BossChar = Cast<ACharacter>(Boss);
		if (!BossChar) continue;

		// 보스에게 나이아가라 이펙트 부착 (루핑은 에셋에서 설정)
		if (BindingEffect)
		{
			UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
				BindingEffect,
				BossChar->GetRootComponent(),
				NAME_None,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::SnapToTarget,
				false  // bAutoDestroy = false (수동으로 종료)
			);
			if (NiagaraComp)
			{
				SpawnedEffects.Add(NiagaraComp);
			}
		}

		// 보스 위치에서 피격 사운드 재생
		if (BindingHitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, BindingHitSound, BossChar->GetActorLocation());
		}

		// 이동 정지
		BossChar->GetCharacterMovement()->StopMovementImmediately();
		BossChar->GetCharacterMovement()->DisableMovement();

		// BT 정지
		AAIController* AIC = Cast<AAIController>(BossChar->GetController());
		if (AIC && AIC->GetBrainComponent())
		{
			AIC->GetBrainComponent()->StopLogic("Bound");
		}
	}

	// BindingDuration 후 보스 BT 재개
	GetWorldTimerManager().SetTimer(
		BindingDurationTimer,
		[this, SpawnedEffects]()
		{
			// 나이아가라 이펙트 종료
			for (UNiagaraComponent* Comp : SpawnedEffects)
			{
				if (IsValid(Comp))
				{
					Comp->Deactivate();
					Comp->DestroyComponent();
				}
			}

			TArray<AActor*> BossActors;
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Boss"), BossActors);

			for (AActor* Boss : BossActors)
			{
				ACharacter* BossChar = Cast<ACharacter>(Boss);
				if (!BossChar) continue;

				BossChar->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

				AAIController* AIC = Cast<AAIController>(BossChar->GetController());
				if (AIC && AIC->GetBrainComponent())
				{
					AIC->GetBrainComponent()->RestartLogic();
				}
			}
		},
		BindingDuration,
		false);

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
	if (GodSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, GodSound, GetActorLocation());
	}
	
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
	OnBuffChanged.Broadcast();
}

void AJamCharacter::CoolZero()
{
	if (CoolZeroSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, CoolZeroSound, GetActorLocation());
	}
	
	IsCoolZero = true;

	GetWorldTimerManager().ClearTimer(TeleportCoolTimer);
	GetWorldTimerManager().ClearTimer(ManaBulletCoolTimer);
	GetWorldTimerManager().ClearTimer(FireBallCoolTimer);
	GetWorldTimerManager().ClearTimer(IceStormCoolTimer);
	GetWorldTimerManager().ClearTimer(EarthQuakeCoolTimer);
	GetWorldTimerManager().ClearTimer(BindingCoolTimer);
	
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
	OnBuffChanged.Broadcast();
}

void AJamCharacter::LevelUp()
{
	int32 CurrentLevel = StatusComponent->GetLevel();
	int32 RandomIndex = 0;

	SpawnEffect(LevelUpEffect, GetActorLocation());
	if (LevelUpSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, LevelUpSound, GetActorLocation());
	}
	
	if (StatusComponent)
	{
		StatusComponent->AddMaxHealth(15.0f);
		StatusComponent->AddMaxMana(15.0f);
		StatusComponent->IncreaseHealth(StatusComponent->GetMaxHealth());
		StatusComponent->IncreaseMana(StatusComponent->GetMaxMana());
	}
	
	switch (CurrentLevel)
	{
		case 2:
			CanFireBall = true;
			break;
		case 3:
			RandomIndex = FMath::RandRange(0, DebuffList.Num() - 1);
			DebuffList.RemoveAt(RandomIndex);
			OnDebuffChanged.Broadcast();
			break;
		case 4:
			CanIceStorm = true;
			break;
		case 5:
			RandomIndex = FMath::RandRange(0, DebuffList.Num() - 1);
			DebuffList.RemoveAt(RandomIndex);
			OnDebuffChanged.Broadcast();
			break;
		case 6:
			break;
		case 7:
			RandomIndex = FMath::RandRange(0, DebuffList.Num() - 1);
			DebuffList.RemoveAt(RandomIndex);
			OnDebuffChanged.Broadcast();
			CanEarthQuake = true;
			break;
		case 8:
			break;
		case 9:
			RandomIndex = FMath::RandRange(0, DebuffList.Num() - 1);
			DebuffList.RemoveAt(RandomIndex);
			OnDebuffChanged.Broadcast();
			break;
		case 10:
			CanBinding = true;
			break;
	}
}

void AJamCharacter::OnDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("[PLAYER] DEAD!!!!!!"))
}

void AJamCharacter::SpawnEffect(UNiagaraSystem* Effect, FVector SpawnLocation)
{
	if (Effect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			Effect,
			SpawnLocation,
			GetActorRotation(),     
			FVector(1.0f),          
			true,                  
			true);
	}
}

void AJamCharacter::ApplyRandomDebuff()
{
	if (DebuffList.Num() == 0) return;

	UE_LOG(LogTemp, Warning, TEXT("[PLAYER] Pick Debuff"))
	int32 RandomIndex = FMath::RandRange(0, DebuffList.Num() - 1);
	UE_LOG(LogTemp, Warning, TEXT("[PLAYER] Debuff %d"), RandomIndex)

	switch (DebuffList[RandomIndex])
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
		OnDebuffActivated.Broadcast(0);
	}
}

void AJamCharacter::ManaDebuff()
{
	if (StatusComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PLAYER] Mana Debuff"));
		StatusComponent->DecreaseMana(ManaDebuffAmount);
		OnDebuffActivated.Broadcast(1);
	}
}

void AJamCharacter::SpeedDebuff()
{
	UE_LOG(LogTemp, Warning, TEXT("[PLAYER] Speed Debuff"));
	MovementEffector = 1.0f - (SpeedDebuffRate / 100.0f);
	OnDebuffActivated.Broadcast(2);

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
	OnDebuffActivated.Broadcast(3);

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
	OnDebuffActivated.Broadcast(4);

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