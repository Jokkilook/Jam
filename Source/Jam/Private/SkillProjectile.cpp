// Fill out your copyright notice in the Description page of Project Settings.

#include "SkillProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

ASkillProjectile::ASkillProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	// 스피어 충돌 컴포넌트를 루트로 설정
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionSphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	CollisionSphere->SetGenerateOverlapEvents(true);
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ASkillProjectile::OnOverlap);
	RootComponent = CollisionSphere;

	// 프로젝타일 무브먼트 컴포넌트 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 1500.0f;
	ProjectileMovement->MaxSpeed = 1500.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	// 나이아가라 이펙트 컴포넌트 (에디터에서 NiagaraSystem 에셋 할당)
	NiagaraEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraEffect"));
	NiagaraEffect->SetupAttachment(RootComponent);
	NiagaraEffect->bAutoActivate = true;

	// 일정 시간 후 자동 소멸
	InitialLifeSpan = 3.0f;
}

void ASkillProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void ASkillProjectile::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovement->Velocity = ShootDirection.GetSafeNormal() * ProjectileMovement->InitialSpeed;
}

void ASkillProjectile::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                  bool bFromSweep, const FHitResult& SweepResult)
{
	AActor* HitOwner = GetOwner();
	AActor* HitInstigator = GetInstigator();
	if (OtherActor && OtherActor != this && OtherActor != HitOwner && OtherActor != HitInstigator)
	{
		AController* OwnerController = GetInstigator() ? GetInstigator()->GetController() : nullptr;
		UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
	}
	Destroy();
}

