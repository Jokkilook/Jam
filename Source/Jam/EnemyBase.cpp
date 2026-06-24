// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "StatusComponent.h"
#include "Kismet/GameplayStatics.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Tags.Add("Enemy");
	
	StatusComponent = CreateDefaultSubobject<UStatusComponent>(TEXT("StatusComponent"));

}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	// 원본 머티리얼 저장
	for (int32 i = 0; i < GetMesh()->GetNumMaterials(); i++)
	{
		OriginalMaterials.Add(GetMesh()->GetMaterial(i));
	}
}

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyBase::ExecuteAttack_Implementation()
{
	if (AttackSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
	}
}


float AEnemyBase::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (StatusComponent && ActualDamage > 0)
	{
		StatusComponent->DecreaseHealth(ActualDamage);
		
		if (StatusComponent->GetCurrentHealth() <= 0)
		{
			bIsDead = true;
			Die();
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
				GetWorldTimerManager().SetTimer(HitFlashTimerHandle, this, &AEnemyBase::RestoreOriginalMaterials, 0.1f, false);
			}
		}
	}

	return ActualDamage;
}

void AEnemyBase::Die()
{
	// 공격 몽타주 중단
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->StopAllMontages(0.2f);
	}

	// AIC + BT 정지
	AAIController* AIC = Cast<AAIController>(GetController());
	if (AIC && AIC->GetBrainComponent())
	{
		AIC->GetBrainComponent()->StopLogic("Dead");
	}

	// 캡슐 콜리전 비활성화
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 메시 콜리전 비활성화 (죽는 애니메이션 재생 유지)
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 경험치 델리게이트 브로드캐스트
	OnEnemyDied.Broadcast(ExperienceReward);

	// 보스 태그가 있으면 페이드아웃
	if (ActorHasTag("Boss"))
	{
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (PC && PC->PlayerCameraManager)
		{
			PC->PlayerCameraManager->StartCameraFade(0.0f, 1.0f, 2.0f, FLinearColor::Black, false, true);
		}
	}

	// 3초 뒤 시체 제거 (Destroy가 EndPlay 자동 호출)
	FTimerHandle DeathTimerHandle;
	GetWorldTimerManager().SetTimer(DeathTimerHandle, this, &AEnemyBase::OnDeathFinished, 3.0f, false);
}

void AEnemyBase::OnDeathFinished()
{
	Destroy();
}

void AEnemyBase::RestoreOriginalMaterials()
{
	for (int32 i = 0; i < OriginalMaterials.Num(); i++)
	{
		GetMesh()->SetMaterial(i, OriginalMaterials[i]);
	}
}

