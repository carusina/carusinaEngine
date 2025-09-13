<div align="center">

#  carusinaEngine ✨

<p>
  <strong>A real-time rendering engine built with C++ and Direct3D 11</strong>
</p>
<p>
  This project is a personal study to implement modern rendering techniques such as PBR, advanced shadows, and post-processing from the ground up.
</p>

![Language](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![API](https://img.shields.io/badge/Direct3D_11-76B900?style=for-the-badge&logo=windows&logoColor=white)

</div>

---

## 📸 Demo

(ScreenShot 추가 예정)

![carusinaEngine Demo](https://via.placeholder.com/800x450.png?text=Your+Best+Rendering+Result+Here)

---

## ✨ 주요 기능 (Features)


-   **렌더링 파이프라인 (Rendering Pipeline)**
    -   Direct3D 11 기반의 Rendering 파이프라인 구축
    -   HDR 렌더링 및 톤 매핑(Tone Mapping)

-   **물리 기반 렌더링 (Physically Based Rendering)**
    -   Cook-Torrance BRDF 기반의 Metallic-Roughness 워크플로우
    -   이미지 기반 라이팅 (Image-Based Lighting, IBL)을 활용한 환경광
        -   Irradiance Map 및 Pre-filtered Specular Map 생성

-   **조명 및 그림자 (Lighting & Shadows)**
    -   Directional, Point, Spot, Sphere Lights
    -   Shadow Mapping
        -   PCF (Percentage-Closer Filtering)를 이용한 부드러운 그림자 경계 처리
        -   PCSS 그림자 기법

-   **지오메트리 처리 (Geometry Processing)**
    -   테셀레이션 (Tessellation)을 이용한 동적 LOD (Level of Detail)
    -   지오메트리 셰이더 (Geometry Shader) 활용
        -   빌보드(Billboard), 노멀 벡터(Normal Vector) 시각화

-   **텍스처링 및 재질 (Texturing & Materials)**
    -   노멀 맵핑 (Normal Mapping)
    -   큐브맵 (Cubemapping)을 이용한 반사 및 스카이박스

-   **후처리 (Post-Processing)**
    -    Bloom
    -    Fog

-   **사용자 상호작용 (User Interaction)**
    -   1인칭 시점 카메라 (First-person camera)
    -   마우스 피킹 (Mouse Picking)을 통한 오브젝트 선택

---

## 🛠️ 기술 스택 (Built With)

-   **언어**: `C++`
-   **그래픽스 API**: `Direct3D 11`
-   **수학**: `SimpleMath`
-   **UI**: [Dear ImGui](https://github.com/ocornut/imgui) - 실시간 디버깅 및 파라미터 조정을 위한 GUI
-   **모델 로딩**: [Assimp](https://github.com/assimp/assimp) (Open Asset Import Library)
-   **텍스처 로딩**: [DirectXTex](https://github.com/microsoft/DirectXTex)

---

## 🎯 앞으로의 목표 (Roadmap)

-   [ ] 물리 시스템 연동
-   [ ] 애니메이션 구현