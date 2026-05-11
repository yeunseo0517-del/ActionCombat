# ActionCombact

UE5 C++ 기반 액션 전투 프로토타입

플레이어, 일반 적, 보스가 서로 다른 방식으로 행동하지만  
동일한 전투/피해 처리 파이프라인을 공유하도록 설계된 구조입니다.

전투는 입력/AI → 스킬/공격 생성 → 판정 → 상태/표현으로 분리되어 있으며  
데이터 교체만으로 전투 스타일이 변경되도록 설계했습니다.

---

# 핵심 설계 포인트

- Runtime Skill 구조 (Player / Boss 오케스트레이션 분리)
- Data-driven Trace 기반 공격 판정 시스템
- 통합 Combat Processing Pipeline
- StatusComponent 기반 상태/쿨다운/UI 분리

---

# 전체 전투 흐름

```mermaid
flowchart LR
A[Input / AI] --> B[Orchestration Layer]
B --> C[Skill / Weapon 선택]
C --> D[CombatComponent]
D --> E[HitContext 생성]
E --> F[Damage / Team Check]
F --> G[StatusComponent]
G --> H[UI / Character Reaction]

---
