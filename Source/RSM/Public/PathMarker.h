// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PathMarker.generated.h"


UCLASS()
class RSM_API APathMarker : public AActor{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APathMarker();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
	UPROPERTY(EditAnywhere)
	APathMarker* m_nextTarget;
};

