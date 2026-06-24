// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkillProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraComponent;

UCLASS()
class JAM_API ASkillProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ASkillProjectile();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNiagaraComponent* NiagaraEffect;

public:
	// 나이아가라 시스템 에셋 (에디터에서 할당)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|VFX")
	class UNiagaraSystem* NiagaraSystem;
	// 발사체 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float Damage = 10.0f;

	// 발사 방향으로 속도 설정 (SpawnActor 후 호출)
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void FireInDirection(const FVector& ShootDirection);

private:
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	               bool bFromSweep, const FHitResult& SweepResult);
};
