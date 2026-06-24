// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemySpawner.h"
#include "EnemyBase.h"
#include "JamCharacter.h"
#include "Public/StatusComponent.h"
#include "NavigationSystem.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	// 에디터에서 위치를 잡기 위한 빌보드 (런타임엔 보이지 않음)
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	TryFillSpawnSlots();
}

void AEnemySpawner::TryFillSpawnSlots()
{
	if (EnemyClasses.Num() == 0) return;

	// 죽은 약참조 정리
	SpawnedEnemies.RemoveAll([](const TWeakObjectPtr<AEnemyBase>& Ptr) {
		return !Ptr.IsValid();
	});

	int32 ToSpawn = MaxEnemyCount - SpawnedEnemies.Num();
	for (int32 i = 0; i < ToSpawn; i++)
	{
		SpawnEnemy();
	}
}

void AEnemySpawner::SpawnEnemy()
{
	if (EnemyClasses.Num() == 0) return;

	FVector SpawnLocation;
	if (!GetRandomSpawnLocation(SpawnLocation)) return;

	FRotator SpawnRotation(0.f, FMath::RandRange(0.f, 360.f), 0.f);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// 배열에서 랜덤으로 클래스 선택
	TSubclassOf<AEnemyBase> SelectedClass = EnemyClasses[FMath::RandRange(0, EnemyClasses.Num() - 1)];
	if (!SelectedClass) return;

	AEnemyBase* Spawned = GetWorld()->SpawnActor<AEnemyBase>(SelectedClass, SpawnLocation, SpawnRotation, Params);
	if (!Spawned) return;

	// 스포너에서 설정한 경험치량 적용
	Spawned->ExperienceReward = ExperienceReward;

	SpawnedEnemies.Add(Spawned);

	// 에너미가 파괴될 때 재스폰 예약
	Spawned->OnDestroyed.AddDynamic(this, &AEnemySpawner::OnEnemyDied);
	// 에너미가 죽을 때 플레이어에게 경험치 전달
	Spawned->OnEnemyDied.AddDynamic(this, &AEnemySpawner::OnSpawnedEnemyDied);
}

void AEnemySpawner::OnEnemyDied(AActor* DestroyedActor)
{
	// 약참조 정리는 TryFillSpawnSlots 에서 일괄 처리
	if (!bRespawn) return;

	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &AEnemySpawner::TryFillSpawnSlots, RespawnDelay, false);
}

void AEnemySpawner::OnSpawnedEnemyDied(float ExperienceAmount)
{
	AJamCharacter* Player = Cast<AJamCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Player && Player->StatusComponent)
	{
		Player->StatusComponent->IncreaseExp(ExperienceAmount);
	}
}

bool AEnemySpawner::GetRandomSpawnLocation(FVector& OutLocation) const
{
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSys)
	{
		FNavLocation NavLocation;
		if (NavSys->GetRandomReachablePointInRadius(GetActorLocation(), SpawnRadius, NavLocation))
		{
			OutLocation = NavLocation.Location;
			return true;
		}
		UE_LOG(LogTemp, Warning, TEXT("EnemySpawner: NavMesh에서 스폰 위치를 찾지 못했습니다. NavMeshBoundsVolume이 이 영역을 커버하는지 확인하세요."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemySpawner: NavMesh가 없습니다. 스포너 위치 근처로 폴백합니다."));
	}

	// NavMesh 폴백: 스포너 위치에서 반경 내 랜덤 XY 오프셋
	const FVector Origin = GetActorLocation();
	const float Angle = FMath::RandRange(0.f, 360.f) * (PI / 180.f);
	const float Dist  = FMath::RandRange(0.f, SpawnRadius);
	OutLocation = Origin + FVector(FMath::Cos(Angle) * Dist, FMath::Sin(Angle) * Dist, 0.f);
	return true;
}
