# MFC_Project 2팀 Github 규칙 
1. main에 작업 금지, main은 병합용입니다. 
2. 각 파트별 브랜치 생성(혹은 팀원별) 후 작업해주세요.
3. commit 메시지 작성 방법에 따라 commit 후 push 하세요.
4. Pull requests 할 때에는 팀원들에게 알리고 진행해주세요.

# Commit 메시지 작성 방법 
"{commit 날짜}, {코드작성자}, {Commit 메시지}"

# 패킷 구조 참고 
바이너리 데이터, 빅엔디안으로 전송
## 헤더 + 바디 구조로 진행 
### 헤더 규칙 
**MSGTYPE** : 메시지 타입, 1 바이트, CHAR
- 1 : 이미지 전송
- 2 : 이미지 전송 응답 (잘받았다)
- 3 : 결과 전송
- 4 : 결과 전송 응답 (잘받았다)

**BODYLEN** : 이미지 크기(buffer에 할당), 4바이트, INT 

**IMGID** : 이미지 식별용 ID , 4바이트, INT 

### 바디 규칙 
- 이미지는 바이너리 데이터로 전송
- 결과는 0 또는 1 로 전송 : 0 - PASS , 1 - FAIL

# CPPClient 빌드시 사전 점검 사항
## 1. 환경변수 설정(openCV)
### : Windows 검색 -> 시스템 환경 변수 편집 -> 고급 -> 환경 변수 -> 시스템 변수 -> 새로 만들기 -> 변수 이름 : OPEN_CV / 변수 값 : C:\Users\mmmz\Downloads\opencv\build(본인 경로에 맞춰 설정, 단 opencv\build 까지 일치해야 함) -> 확인
### : Windows 검색 -> 시스템 환경 변수 편집 -> 고급 -> 환경 변수 -> 시스템 변수 -> Path(변수) 더블클릭 ->  새로 만들기 -> C:\Users\mmmz\Downloads\opencv\build\x64\vc16\bin(본인 경로에 맞춰 설정, 단 vc16\bin 까지 일치해야 함) -> 확인
## 2. VisulStudio 경로 설정
### : 프로젝트 -> 속성 -> C/C++ -> 일반 -> 추가 포함 디렉터리 : $(PYLON_DEV_DIR)\include;$(OPEN_CV)\include\opencv2;%OPEN_CV%\include;%(AdditionalIncludeDirectories)
### : 프로젝트 -> 속성 -> 링커 -> 일반 -> 추가 라이브러리 디렉터리 : $(PYLON_DEV_DIR)\lib\x64;$(OPEN_CV)\x64\vc16\lib;%(AdditionalLibraryDirectories)
### : 프로젝트 -> 속성 -> 링커 -> 입력 -> 추가 종속성 : opencv_world490.lib;opencv_world490d.lib

# C# Server 실행시 점검 사항 
## 1. DB 관련 설정 확인 
- MySQL 용 NuGet 패키지 설치 : Visual Studio에서 프로젝트 - NuGet 패키지 관리 - MySQL.Data 설치 
- Db.cs 파일 내 Db 클래스 - CreateDefault 메서드의 접속 정보 확인
## 2. Py Server 접속 정보 확인 
- ClientToPyServer.cs 내 멤버변수 _pyServerIPAddress , _pyServerPort 값 확인
## 3. C# 서버 포트정보 확인 
- Server.cs 내 StartServerListeningAsync 메서드 확인 port 값과 서버 실행할 컴퓨터의 IP주소를 C++클라이언트에 알려주기
