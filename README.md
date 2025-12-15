# Round-Robin Thread Scheduler (User-level)
광운대학교 시스템소프트웨어
2차 과제 (개인 프로젝트)

# 📌 프로젝트 개요
본 프로젝트는 PThread 기반 사용자 수준(User-level) Round-Robin 스레드 스케줄러를 구현한 과제입니다.
운영체제 커널 스케줄러를 사용하는 것이 아니라, 라이브러리 레벨에서 스레드 관리 및 스케줄링을 직접 구현하는 것을 목표로 합니다.

# ⚙️시스템 아키텍처
## 전체구조
<img width="320" height="178" alt="image" src="https://github.com/user-attachments/assets/83e821de-9307-4d0b-9691-754bf6842b01" />

## 주요 컴포넌트
 <img width="499" height="218" alt="image" src="https://github.com/user-attachments/assets/05281da3-ff63-4eaa-baa0-18f31ae4b6d9" />

# 🧪 테스트 케이스 결과
## TestCase 1
기본 스레드 생성 및 스케줄링 동작 검증

<img width="387" height="475" alt="image" src="https://github.com/user-attachments/assets/6206b2ed-a28a-49e0-9bf8-6dfb3efbc3d5" />

## TestCase 2
다중 스레드 생성
thread_join을 통한 종료 동기화 및 반환값 검증

<img width="412" height="1015" alt="image" src="https://github.com/user-attachments/assets/ddd1d8d3-7277-4d1d-ad78-02d17e0dfbb9" />

## TestCase 3
thread_suspend / thread_resume 동작 검증
Wait Queue / Ready Queue 상태 전이 확인

<img width="357" height="923" alt="image" src="https://github.com/user-attachments/assets/10be14be-1375-4127-9c12-0613069af0d0" />
