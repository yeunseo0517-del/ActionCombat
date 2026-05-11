
동영상: https://youtu.be/HC7rAM73Swk

Notion: https://www.notion.so/Action-Combat-Project-Troubleshooting-35d2b62eaa66805fada6fffad00a9de3

# UE5 C++ 기반 액션 전투 프로토타입

플레이어, 일반 적, 보스가 서로 다른 방식으로 행동하지만  
동일한 전투/피해 처리 파이프라인을 공유하도록 설계된 구조입니다.

전투는 입력/AI → 스킬/공격 생성 → 판정 → 상태/표현으로 분리되어 있으며  
데이터 교체만으로 전투 스타일이 변경되도록 설계했습니다.

---

# 핵심 설계 포인트

1. Runtime Skill 구조 (Player / Boss 오케스트레이션 분리)
2. Data-driven Trace 기반 공격 판정 시스템
3. 통합 Combat Processing Pipeline
4. StatusComponent 기반 상태/쿨다운/UI 분리

---

## 1. Runtime Skill Composition (Player vs Boss)

플레이어와 보스는 스킬 생성 방식이 다르지만  
최종 실행 구조는 동일합니다.

- 플레이어는 무기가 SkillEntries를 읽어 스킬을 생성
- 보스는 SkillPool을 읽어 거리 기반으로 스킬을 선택

### Player
Weapon → SkillEntries → Runtime Skill 생성

### Boss
SkillPool → AI 선택 로직 → Runtime Skill 생성

```cpp
USkillBase::ActivateSkill(Owner);
```

```mermaid
flowchart LR
    A["Player Input / AI Decision"] --> B["Orchestration Layer"]
    B --> C["Weapon or Boss Skill Pool"]
    C --> D["Runtime Skill / Attack Selection"]
    D --> E["CombatComponent"]
    E --> F["Trace or Area Attack"]
    F --> G["HitContext"]
    G --> H["Damage / Team Check / GetHit"]

    D --> I["StatusComponent"]
    I --> J["Cooldown / Buff / Broadcast"]
    J --> K["Character Reaction / HUD Update"]
```

캐릭터나 무기는 구체적인 전투 로직을 내장하지 않고, 런타임에 적절한 스킬을 조립하고 선택하는 'Selection Layer' 역할만 수행합니다.


### 핵심 설계 의도
- 관심사 분리
  전투 로직은 캐릭터 본체가 아닌 Skill Class에 고립시켜 캐릭터 코드의 비대화를 방지합니다.
- Interface 기반 추상화
  호출자는 실행될 스킬의 세부 구현을 알 필요가 없습니다.
- 높은 재사용성
  스킬 구현체가 특정 캐릭터 클래스에 묶이지 않습니다. 또한 Player, Boss, Minion 등 모두가 동일한 실행 구조를 공유하므로, 새로운 타입의 캐릭터를 추가하더라도 전투 시스템을 수정 없이 재사용할 수 있습니다.

```cpp
Input / AI / Weapon
        ↓
   Skill Selection
        ↓
   USkillBase::Activate()
        ↓
 Combat Execution Trigger
```

---

## 2. Data-driven Trace System
공격 판정은 코드가 아니라 데이터로 정의됩니다.

- Steps 기반 Socket Pair 구조
- ANS가 Index만 변경
- WeaponData / OverrideData 지원

```mermaid
flowchart TD
    A["CurrentCombatTag + WeaponType + Stance"] --> B["CombatComponent::SetCombatTraceData()"]
    B --> C{"OverrideCombatData 존재?"}
    C -->|Yes| D["OverrideCombatData.AttackSet[Tag]"]
    C -->|No| E["WeaponData.AttackSet[Tag]"]
    D --> F["FCombatTraceData"]
    E --> F
```

<img width="464" height="444" alt="image" src="https://github.com/user-attachments/assets/2c0767bf-fdb6-4376-adeb-240fb56dd9a9" />


각 공격 Step은 다음 정보를 포함합니다:

- Start / End socket pair (또는 단일 socket)
- Character mesh / Weapon mesh source 선택
- Trace 방식(Box / Sphere / Sweep)

애니메이션 Notify State(ANS_SwitchSocket)를 통해
현재 콤보 Step index를 갱신하고, 해당 Step의 TraceData로 자동 전환됩니다.

Unarmed 및 모든 근접 공격을 단일 Weapon 파이프라인으로 통합하고,
공격 콤보는 Trace Step 단위로 정의된 socket sampling 규칙을 교체하는 방식으로 처리했습니다.

콤보 진행에 따라 Trace Step이 변경되며,
각 Step은 Left/Right Punch에 해당하는 socket pair를 교체하는 방식으로 구성됩니다.

이를 통해:

- 좌/우 주먹 공격을 별도 무기 없이 처리
- 무기 시스템과 동일한 Trace pipeline 재사용
- 콤보 확장을 데이터 기반으로 처리

## 결과
- Socket 개수/구조 변화가 코드 수정 없이 데이터로 해결됨
- 콤보 확장 시 Weapon Class 변경 필요 없음

---

## 3. 통합 Combat Processing Pipeline

평타, 스킬, 범위 공격은 서로 다른 방식으로 히트 대상을 생성하지만,
최종 피해 처리 단계는 하나의 공통 파이프라인으로 수렴하도록 설계했습니다.

- 기본 공격: Weapon Trace 기반 Hit 대상 생성
- 스킬: SkillHitContext 기반 Hit 대상 생성
- 범위 공격: AreaEffect Actor 기반 Hit 대상 생성

  이후 모든 공격은 동일한 흐름을 따릅니다:
  
```cpp
  Target Detection
→ HitContext 생성
→ Enemy Validation (Team / Tag / Rule)
→ Damage Calculation
→ ApplyDamage
→ Hit Reaction (Animation / Effect / State)
```

### 구조적 이점

1. 피해 처리 로직의 중앙화

공격 생성 방식과 무관하게 동일한 피해 규칙을 사용하므로
팀 판정, 강화 데미지, 히트 이펙트, GetHit 호출 규칙이 모두 한곳에서 관리됩니다.
2. 확장 비용 감소

새로운 공격 타입(투사체, 환경 공격 등)을 추가하더라도
Hit 생성 부분만 추가하면 되며, 피해 처리 로직은 재구현이 필요하지 않습니다.

3. 일관된 전투 규칙 유지

평타 / 스킬 / 범위 공격 간 처리 방식이 분리되지 않기 때문에
전투 규칙이 시스템 전체에서 동일하게 유지됩니다.

4. 디버깅 효율 향상

모든 피해 흐름이 CombatComponent 기준으로 수렴하므로 버그 발생 시 단일 파이프라인에서 추적이 가능합니다.

---

## 4. StatusComponent 기반 상태/쿨다운/UI 분리

StatusComponent를 중심으로 스킬 상태(쿨다운/버프/상태 플래그)를 중앙화하고, UI 및 전투 시스템과 이벤트 기반으로 분리된 구조입니다.

```mermaid
flowchart TD

%% ======================
%% Skill Execution Layer
%% ======================
A[Player / AI Input] --> B[Skill Selection]
B --> C[USkillBase::Activate]

%% ======================
%% Combat Layer
%% ======================
C --> D[Combat Pipeline Trigger]
D --> E[Weapon / Skill / Area Attack]

E --> F[HitContext 생성]

%% ======================
%% Status System
%% ======================
C --> G[StatusComponent]
G --> G1[Cooldown Management]
G --> G2[Buff / Debuff State]
G --> G3[State Flags]

%% ======================
%% Combat Resolution
%% ======================
F --> H[Combat Resolution Pipeline]
H --> I[Damage Calculation]
I --> J[ApplyDamage]
J --> K[Hit Reaction]

%% ======================
%% UI Layer
%% ======================
G --> L[Event Broadcast]
L --> M[UI System]
M --> M1[Cooldown UI]
M --> M2[Buff Indicator]
M --> M3[State Display]

%% ======================
%% Relations
%% ======================
K --> L
```
