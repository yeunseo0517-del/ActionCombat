# UE5 C++ 기반 액션 전투 프로토타입

플레이어, 일반 적, 보스가 서로 다른 방식으로 행동하지만  
동일한 전투/피해 처리 파이프라인을 공유하도록 설계된 구조입니다.

전투는 입력/AI → 스킬/공격 생성 → 판정 → 상태/표현으로 분리되어 있으며  
데이터 교체만으로 전투 스타일이 변경되도록 설계했습니다.

## 목차
* [플레이 영상](#플레이-영상)
* [프로젝트 개요](#프로젝트-개요)
* [조작 방법](#조작-방법)
* [핵심 구현 요약](#핵심-구현-요약)
  * [World-Space Blood Field](#World-Space-Blood-Field)
  * [Combat Slot System](#Combat-Slot-System)
  * [Combat Pipeline](#Combat-Pipeline)
    * [Weapon 기반 근접 공격 통합 구조](#Weapon-기반-근접-공격-통합-구조)
  * [Skill System](#Skill-System)
    * [Runtime 기반 스킬 구성](#Runtime-기반-스킬-구성)
    * [스킬 레이어 간 단방향 흐름](#스킬-레이어-간-단방향-흐름)
* [트러블슈팅](./Docs/TroubleShooting.md)

---

## 플레이 영상

[Youtube](https://youtu.be/bdACHdRWFP0)

---

## 프로젝트 개요

장르: 3D 액션 RPG

개발 인원: 1인

사용 기술: Unreal Engine 5, C++, HLSL

마을을 거점으로 던전에 진입해 전투와 보스전을 진행하고 다시 귀환하는 기본 플레이 루프의 액션 RPG 게임입니다.

시각적 자연스러움을 우선하여 전투 경험 구축했습니다.

---

## 조작 방법

- WASD 입력 이동
- Shift: 달리기
- F: 무기 장착 / 해제
- 왼쪽 클릭: 기본 공격 (무기에 따라 콤보 공격 존재)
- Q, E, R 스킬

---

## 핵심 구현 요약

## World-Space Blood Field

<img width="1074" height="347" alt="image" src="https://github.com/user-attachments/assets/27399ada-110d-4055-a928-cd7b84098f72" />

기존 Decal 방식에서 발생하는 **모서리 왜곡, 굴곡진 표면, 얇은 Mesh 위 표현 한계**를 보완하기 위해 별도의 World-Space Blood Field 시스템을 구현했습니다.

Blood Texture를 작은 단위인 여러 Sample로 분할한 뒤 각 Sample의 실제 표면 위치를 개별적으로 계산하고, 결과를 World-Space 3D Field에 기록합니다. Material은 자신의 World Position을 기준으로 Field를 조회하여 원본 Blood Texture를 다시 Sampling합니다.

### 주요 구현

- Blood Texture를 Grid 기반 Sample로 사전 분석
- Trace를 이용한 Sample별 실제 표면 위치 보정
- Compute Shader 2-Pass로 Voxel별 Winner 선정 및 UV / Pattern 정보 기록
- Material에서 World Position 기반 Blood Field 조회
- Texture2DArray를 이용한 다중 Blood Pattern 지원

> 세부 설계 과정, 실패 사례 및 Compute Shader 구현은 별도 문서에서 확인할 수 있습니다.

#### [상세 구현 문서](https://github.com/yeunseo0517-del/GPUBloodField)

<img width="1214" height="787" alt="image" src="https://github.com/user-attachments/assets/f82fadb7-292a-4dc3-89e0-c43d15401a70" /> <img width="924" height="595" alt="image" src="https://github.com/user-attachments/assets/37f79947-57f8-4b6e-84e7-0b11ec38505c" />

---

## Combat Slot System

> 다수의 적이 플레이어 주변에 자연스럽게 분산되도록 전투 위치를 배정하는 시스템

<img src="https://github.com/yeunseo0517-del/ActionCombat/blob/main/Docs/Images/SurroundSlot_Fin.gif" width="400"> <img src="https://github.com/yeunseo0517-del/ActionCombat/blob/main/Docs/Images/SurroundSlot_Wandering.gif" width="400">

#### 설계 배경

여러 적이 동시에 플레이어를 추적하면 한쪽에 몰리거나 서로 겹치면서 이동 경로가 뒤엉키는 문제가 있었습니다.

이를 해결하기 위해 플레이어 주변에 **적마다의 전투 위치인 Combat Slot**을 배치하고,  
적의 위치와 방향을 고려해 적합한 Slot을 선택하도록 구성했습니다.

#### 주요 구현

- 플레이어 주변에 원형 Combat Slot 구성
- Slot 생성 / 점유 / 배정 / 해제를 Actor Component에서 중앙 관리
- 모든 `Enemy × Free Slot` 조합의 거리·각도 Cost 계산
- 낮은 Cost부터 Greedy하게 Slot 배정
- 공격 가능 시 Slot 도착보다 공격을 우선하도록 전투 규칙 구성

> 세부 배정 방식의 개선 과정, Cost 설계 및 트러블슈팅은 별도 문서에서 확인할 수 있습니다.

#### [상세 설계 문서](Docs/CombatSlot.md)

---

## Combat Pipeline

### Weapon 기반 근접 공격 통합 구조
> 맨손 전투까지 확장한 Weapon Trace Pipeline 재사용 구조

#### 설계 배경

전투 시스템은 공격 방식을 기준으로 `Weapon`, `Projectile`, `Area Attack`으로 역할을 분리했습니다. 이 중 `Weapon`은 근접 Trace 공격을 담당하는 객체로 정의했으며, 맨몸 공격과 Character Mesh에 포함된 무기 역시 Mesh만 없을 뿐 동일한 근접 Trace 공격으로 판단했습니다.

따라서 별도의 처리 흐름을 추가하지 않고 **기존 Weapon 구조를 재사용**하여 처리하도록 구성했습니다.

#### 구현 방법

캐릭터 기반 공격도 근접 Trace 공격으로 판단하여 기존 `Weapon` 구조 안에서 처리했습니다.  
Weapon Mesh가 없는 공격은 `Dummy Weapon Actor`를 사용하고 캐릭터별 Trace 차이는 `OverrideCombatData`로 Data Asset을 교체하는 방식으로 대응했습니다.

#### [상세 설계 문서](Docs/UnarmedTraceIntegration.md)

---

## Skill System

### Runtime 기반 스킬 구성
> 기획 변경이 AI 로직에 영향을 주지 않는 Runtime Skill 구조

<img width="565" height="517" alt="image" src="https://github.com/user-attachments/assets/8e767150-cb48-4828-b5c0-a3c18155d85f" />


#### 설계 배경

거리에 따라 스킬 조합이 달라지고 개발 중 기획이 바뀔 가능성이 높았습니다. 스킬이 늘어나도 **코드 수정 없이** 에디터에서만 구성을 바꿀 수 있도록 **데이터와 실행**을 **분리**해서 설계했습니다.

#### 설계 포인트

| 역할 | 담당 |
| :--- | :--- |
| 선택 | `AI Controller` |
| 타이밍 | `AnimNotify` |
| 실행 | `Combat Component` |

#### [상세 설계 문서](Docs/RuntimeSkill.md)

---

### 스킬 레이어 간 단방향 흐름
> HUD가 스킬을 직접 조회하지 않는 이벤트 기반 실행 구조

<img width="564" height="474" alt="image" src="https://github.com/user-attachments/assets/a7be2175-73a7-423f-b45e-21415262ba3a" />

#### 설계 배경

스킬 실행 이후에는 상태 변경, 쿨타임 관리, UI 갱신이 연쇄적으로 발생합니다. 직접 호출 구조로 연결할 경우, 상위 레이어가 하위 레이어의 구현에 의존하게 되어 레이어 독립성을 유지하기 어려워집니다.

이를 해결하기 위해 스킬 실행 결과를 **StatusComponent에 모으고** HUD와 Character는 **상태 이벤트를 구독해** 각자의 처리를 수행하도록 설계했습니다.

#### [상세 설계 문서](Docs/SkillLayer_UnidirectionalFlow.md)
