// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Circulator.generated.h"


class APathMarker;


UCLASS()
class RSM_API ACirculator : public AActor{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACirculator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:
	void m_updatePos();


public:
	UPROPERTY(EditAnywhere)
	float m_fSpeed;

	UPROPERTY(EditAnywhere)
	float m_fTension;

public:
    UPROPERTY(EditAnywhere)
	APathMarker* m_curPath;

private:
	float m_fPos;

private:
	APathMarker* m_pathList[4];
};

