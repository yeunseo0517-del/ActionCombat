ActionCombact

UE5 C++ 기반 액션 전투 시스템

플레이어 / AI / 보스가 서로 다른 방식으로 행동하지만
동일한 전투 및 피해 처리 파이프라인을 공유하는 구조입니다.

핵심 설계 포인트
Runtime Skill 구조 (Player / Boss 오케스트레이션 분리)
Data-driven Trace 기반 공격 판정
통합 Combat Processing Pipeline
StatusComponent 기반 상태/UI 분리
전체 전투 흐름
핵심 설계
1. Runtime Skill Composition (Player vs Boss)

플레이어와 보스는 스킬 생성 방식이 다르지만
최종 실행은 동일한 인터페이스를 사용합니다.

Player: Weapon → SkillEntries → Runtime Skill
Boss: SkillPool → AI 선택 → Runtime Skill
USkillBase::ActivateSkill(Owner);
결과
스킬 구현은 호출자(Player/Boss)와 분리
AI / Player 전투 구조 통합 가능
스킬 재사용성 증가
2. Data-driven Trace System

공격 판정은 코드가 아니라 데이터로 정의됩니다.

Steps 기반 Socket Pair 구조
ANS가 Index만 변경
WeaponData / OverrideData 지원
결과
콤보 공격 구조 확장 쉬움
무기 1개로 다양한 공격 표현 가능
예외 코드 제거
3. Unified Combat Pipeline

모든 공격은 동일한 후처리 구조를 사용합니다.

Weapon Attack
Skill Attack
Area Attack

→ 모두 HitContext로 수렴

결과
데미지/팀 판정 로직 통합
공격 타입 추가 시 수정 최소화
디버깅 구조 단순화
4. StatusComponent 중심 상태 관리

스킬과 UI를 직접 연결하지 않습니다.

StatusComponent가 상태/쿨다운 관리
UI는 이벤트만 구독
Skill은 효과만 실행
결과
UI와 전투 로직 완전 분리
상태 관리 중앙 집중
확장 비용 감소
기타 구현
AnimNotify 기반 콤보 시스템
Interface 기반 전투 경계 분리
Niagara Trail (Trace 데이터 기반)
구조
Characters
Enemy
Components
 ├ Combat
 ├ Status
 ├ Attribute
Items/Weapons
Interfaces
Types
Animation
HUD
