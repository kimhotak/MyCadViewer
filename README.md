# MyCadViewer

MyCadViewer는 MFC + OpenCascade(OCCT) 기반의 Windows CAD STEP 뷰어 샘플 프로젝트입니다.

## 개발 환경

- Visual Studio 2026 (VC++ toolset)
- Windows 64-bit
- OpenCascade 7.9.3 (Debug 포함 패키지)

## OpenCascade 설치/연결

1. 아래 릴리즈 페이지로 이동합니다.
   - https://dev.opencascade.org/release
2. 다음 ZIP 파일을 다운로드합니다.
   - `Windows package with debug and 3rd-party VC++ 2022 64 bit: opencascade-7.9.3-vc14-64-with-debug-combined.zip`
3. 원하는 경로에 압축을 해제합니다.
4. 환경 변수 `OCCT_ROOT`를 **ZIP을 풀어낸 상위 폴더 경로**(즉, `opencascade-7.9.3-vc14-64-with-debug-combined` 폴더를 포함하고 있는 경로)로 설정합니다.

예시:

- `OCCT_ROOT=C:\libs`  
  (이 경우 OCCT는 `C:\libs\opencascade-7.9.3-vc14-64-with-debug-combined\...` 에 위치해야 합니다.)
## 빌드

1. Visual Studio에서 솔루션(`MyCadViewer.slnx`)을 엽니다.
2. `x64` / `Debug` 또는 `Release` 구성 선택
3. 빌드 후 실행

> 참고: 프로젝트 파일은 `%OCCT_ROOT%\opencascade-7.9.3-vc14-64-with-debug-combined\`를 기준으로 include/lib 경로를 사용합니다.

## 현재 기능

- STEP 파일 열기
- 기본 뷰 조작(회전/이동/줌)
- Wireframe / Shaded 전환
- 2점 거리 측정 표시
