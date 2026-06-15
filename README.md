# UE5 C++ 기반 액션 전투 프로토타입

플레이어, 일반 적, 보스가 서로 다른 방식으로 행동하지만  
동일한 전투/피해 처리 파이프라인을 공유하도록 설계된 구조입니다.

전투는 입력/AI → 스킬/공격 생성 → 판정 → 상태/표현으로 분리되어 있으며  
데이터 교체만으로 전투 스타일이 변경되도록 설계했습니다.

## 목차
* [플레이 영상](#플레이-영상)
* [프로젝트 개요](#프로젝트-개요)
* [조작 방법](#조작-방법)
* [핵심 구현 요약](#핵심-구현-문서)
   * [1.Unified Hit Pipeline](#1.UnifiedHitPipeline)
   * [2.Data-driven 기반 Trace system](2.Data-driven기반Tracesystem)
   * [3.Weapon 기반 근접 공격 통합 구조](3.Weapon기반근접공격통합구조)
   * [4.Runtime 기반 스킬 구성](4.Runtime기반스킬구성)
   * [5.스킬레이어간단방향흐름](5.스킬 레이어간단방향흐름)
* [트러블슈팅]()

---

## 플레이 영상

[Youtube](https://youtu.be/1aM2AIHFSLU)

---

## 프로젝트 개요

장르: 3D 액션 RPG

개발 기간: 2026.03 ~ 2026.05

개발 인원: 1인

사용 기술: Unreal Engine 5, C++

완성형 게임 콘텐츠보다, 액션 전투 시스템의 구조 설계와 구현 역량을 보여주기 위한 프로토타입입니다.

---

## 조작 방법

- WASD 입력 이동
- Shift: 달리기
- F: 무기 장착 / 해제
- 왼쪽 클릭: 기본 공격 (무기에 따라 콤보 공격 존재)
- Q, E, R 스킬

---

## 핵심 구현 요약

## Combat Pipeline

### 1. Unified Hit Pipeline
> 공격 방식과 무관하게 동일한 피격 처리 규칙을 적용하는 구조

<img width="562" height="450" alt="image" src="https://github.com/user-attachments/assets/e5024c9e-0672-4974-b938-265117d8ec74" />


#### 설계 배경

Weapon과 Area Attack에 피격 처리 로직이 각각 존재했습니다.

Projectile을 추가하면서 팀 체크, 데미지 적용, 피격 반응 로직이 중복된다는 걸 발견했고 **중복되는 피격 처리 로직을 Combat Component**로 통합했습니다.

#### 장점
- 피격 로직 중복 제거
- 데미지 / 팀 체크 / 피격 반응 처리 일관화
- 신규 공격 방식 확장 용이

#### [상세 설계 문서](https://github.com/yeunseo0517-del/ActionCombat/blob/main/Docs/UnifiedHitPipeline.md)

---

### 2. Data-driven 기반 Trace system
> Data Asset 기반으로 공격 단위별 Trace 커스터마이징

<img width="546" height="318" alt="image" src="https://github.com/user-attachments/assets/b014c09a-2a08-4b01-93d0-9bebcf026be6" />

#### 설계 배경

공격마다 필요한 판정 범위, 판정 위치 등 정보가 달랐습니다. 이를 코드에서 직접 관리하면 공격 설정 변경 시 판정 로직까지 함께 수정해야 했기 때문에 공격 설정을 **Data Asset**으로 분리했습니다.

#### 장점
- 공격 단위별 Trace 커스터마이징
- 코드 수정 없는 공격 확장

#### [상세 설계 문서](https://github.com/yeunseo0517-del/ActionCombat/blob/main/Docs/DataDrivenTrace.md)

---

### 3. Weapon 기반 근접 공격 통합 구조
> 맨손 전투까지 확장한 Weapon Trace Pipeline 재사용 구조

#### 설계 배경

전투 시스템은 공격 방식을 기준으로 `Weapon`, `Projectile`, `Area Attack`으로 역할을 분리했습니다. 이 중 `Weapon`은 근접 Trace 공격을 담당하는 객체로 정의했으며, 맨몸 공격과 Character Mesh에 포함된 무기 역시 Mesh만 없을 뿐 동일한 근접 Trace 공격으로 판단했습니다.

따라서 별도의 처리 흐름을 추가하지 않고 **기존 Weapon 구조를 재사용**하여 처리하도록 구성했습니다.

#### 구현 방법

캐릭터 기반 공격도 근접 Trace 공격으로 판단하여 기존 `Weapon` 구조 안에서 처리했습니다.  
Weapon Mesh가 없는 공격은 `Dummy Weapon Actor`를 사용하고 캐릭터별 Trace 차이는 `OverrideCombatData`로 Data Asset을 교체하는 방식으로 대응했습니다.

#### [상세 설계 문서](https://github.com/yeunseo0517-del/ActionCombat/blob/main/Docs/UnarmedTraceIntegration.md)

---

## Skill System

### 4. Runtime 기반 스킬 구성
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

#### [상세 설계 문서](https://github.com/yeunseo0517-del/ActionCombat/blob/main/Docs/RuntimeSkill.md)

---

### 5. 스킬 레이어 간 단방향 흐름
> HUD가 스킬을 직접 조회하지 않는 이벤트 기반 실행 구조

<img width="564" height="474" alt="image" src="https://github.com/user-attachments/assets/a7be2175-73a7-423f-b45e-21415262ba3a" />

#### 설계 배경

스킬 실행 이후에는 상태 변경, 쿨타임 관리, UI 갱신이 연쇄적으로 발생합니다. 직접 호출 구조로 연결할 경우, 상위 레이어가 하위 레이어의 구현에 의존하게 되어 레이어 독립성을 유지하기 어려워집니다.

이를 해결하기 위해 스킬 실행 결과를 **StatusComponent에 모으고** HUD와 Character는 **상태 이벤트를 구독해** 각자의 처리를 수행하도록 설계했습니다.

#### [상세 설계 문서](https://github.com/yeunseo0517-del/ActionCombat/blob/main/Docs/SkillLayer_UnidirectionalFlow.md)
