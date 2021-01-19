// Fill out your copyright notice in the Description page of Project Settings.


#include "Circulator.h"
#include "PathMarker.h"


static FQuat _squad(
	FQuat q0,
	FQuat q1,
	FQuat q2,
	float t
)
{
	auto ln0 = (q0.Inverse() * q1).Log();
	auto a0 = ((ln0 + ln0) * -0.25f).Exp();

	auto ln1 = (q1.Inverse() * q2).Log();
	auto a1 = ((ln1 + ln1) * -0.25f).Exp();

	return FQuat::Slerp(FQuat::Slerp(q0, q1, t), FQuat::Slerp(a0, a1, t), 2 * t * (1 - t));
}


// Sets default values
ACirculator::ACirculator()
    :
	m_fPos(0.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	memset(m_pathList, 0, sizeof(m_pathList));
}

// Called when the game starts or when spawned
void ACirculator::BeginPlay(){
	Super::BeginPlay();

	{
		if(m_curPath)
			m_pathList[1] = m_curPath;
		if(m_pathList[1])
			m_pathList[2] = m_pathList[1]->m_nextTarget;
		if(m_pathList[2])
			m_pathList[3] = m_pathList[2]->m_nextTarget;

		m_fPos = 0.f;
	}
}

// Called every frame
void ACirculator::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

	m_fPos += m_fSpeed * DeltaTime;
	m_updatePos();
}


void ACirculator::m_updatePos(){
	if(m_fPos >= 1.f){
        m_pathList[0] = m_pathList[1] ? m_pathList[1] : nullptr;
        m_pathList[1] = m_pathList[2] ? m_pathList[2] : nullptr;
        m_pathList[2] = m_pathList[3] ? m_pathList[3] : nullptr;
        m_pathList[3] = m_pathList[3]->m_nextTarget ? m_pathList[3]->m_nextTarget : nullptr;

		m_fPos = 0.f;
	}
	else{
		FVector vCurPos;
		FQuat qCurQuat;

		if( // only two points given
			m_pathList[1] &&
			m_pathList[2]
			)
		{
			auto v1 = m_pathList[1]->GetActorLocation();
			auto v2 = m_pathList[2]->GetActorLocation();

			vCurPos = FMath::Lerp(v1, v2, m_fPos);

			auto q1 = m_pathList[1]->GetActorRotation().Quaternion();
			auto q2 = m_pathList[2]->GetActorRotation().Quaternion();

			qCurQuat = FQuat::Slerp(q1, q2, m_fPos);
		}
		else if( // begin
			m_pathList[1] &&
			m_pathList[2] &&
			m_pathList[3]
			)
		{
			auto v1 = m_pathList[1]->GetActorLocation();
			auto v2 = m_pathList[2]->GetActorLocation();
			auto v3 = m_pathList[3]->GetActorLocation();

			auto vA = ((1.f - m_fTension) * 0.5f * v1) - (0.5f * v2) + (m_fTension * 0.5f * v3);
			auto vC = (((m_fTension - 3.f) * 0.5f * v1) + (3.f * 0.5f  * v2) - (m_fTension * 0.5f * v3));
			auto vD = v1;

			vCurPos = vD + (vC * m_fPos) + (vA * m_fPos * m_fPos * m_fPos);

			auto q1 = m_pathList[1]->GetActorRotation().Quaternion();
			auto q2 = m_pathList[2]->GetActorRotation().Quaternion();
			auto q3 = m_pathList[3]->GetActorRotation().Quaternion();

			qCurQuat = _squad(q1, q2, q3, m_fPos);
		}
		else if( // middle
			m_pathList[0] &&
			m_pathList[1] &&
			m_pathList[2] &&
			m_pathList[3]
			)
		{
			auto v0 = m_pathList[0]->GetActorLocation();
			auto v1 = m_pathList[1]->GetActorLocation();
			auto v2 = m_pathList[2]->GetActorLocation();
			auto v3 = m_pathList[3]->GetActorLocation();

			auto vA = ((-m_fTension * v0) + ((2.f - m_fTension) * v1) + ((m_fTension - 2.f) * v2) + (m_fTension * v3));
			auto vB = ((2.f * m_fTension * v0) + ((m_fTension - 3.f) * v1) + ((3.f - (2.f * m_fTension)) * v2) - (m_fTension * v3));
			auto vC = ((-m_fTension * v0) + (m_fTension * v2));
			auto vD = v1;

			vCurPos = vD + (vC * m_fPos) + (vB * m_fPos * m_fPos) + (vA * m_fPos * m_fPos * m_fPos);

			auto q1 = m_pathList[1]->GetActorRotation().Quaternion();
			auto q2 = m_pathList[2]->GetActorRotation().Quaternion();
			auto q3 = m_pathList[3]->GetActorRotation().Quaternion();

			qCurQuat = _squad(q1, q2, q3, m_fPos);
		}
		else if( // end
			m_pathList[0] &&
			m_pathList[1] &&
			m_pathList[2]
			)
		{
			auto v0 = m_pathList[0]->GetActorLocation();
			auto v1 = m_pathList[1]->GetActorLocation();
			auto v2 = m_pathList[2]->GetActorLocation();

			auto vA = ((-m_fTension * 0.5f * v0) + (0.5f * v1) + ((m_fTension - 1.f) * 0.5f * v2));
			auto vB = ((3.f * m_fTension * 0.5f * v0) - (3.f * 0.5f * v1));
			auto vC = ((-m_fTension * v0) + (m_fTension * v2));
			auto vD = v1;

			vCurPos = vD + (vC * m_fPos) + (vB * m_fPos * m_fPos) + (vA * m_fPos * m_fPos * m_fPos);

			auto q0 = m_pathList[0]->GetActorRotation().Quaternion();
			auto q1 = m_pathList[1]->GetActorRotation().Quaternion();
			auto q2 = m_pathList[2]->GetActorRotation().Quaternion();

			qCurQuat = _squad(q0, q1, q2, m_fPos);
		}

		SetActorLocation(vCurPos);
		SetActorRotation(qCurQuat);
	}
}

