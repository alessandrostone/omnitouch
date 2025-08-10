# OmniTouch: Omniverse to Spout Bridge

## Project Description

OmniTouch is an NVIDIA Omniverse extension that makes it possible to grab the rendered texture layer of Omniverse and pass it to external software, such as TouchDesigner, using the Spout2 C++ library. This enables real-time video-rate texture sharing between Omniverse and other creative applications.

## Project Objectives

*   **Create an Omniverse Extension:** Develop a robust extension for NVIDIA Omniverse.
*   **Integrate Spout2:** Integrate the Spout2 C++ library to handle the texture sharing.
*   **Texture Capturing:** Implement functionality to capture the rendered texture from a specified layer within an Omniverse scene.
*   **Spout Sender:** Create a Spout sender within the extension to broadcast the captured texture.
*   **External Software Integration:** Ensure that the texture can be received by external software that supports Spout (e.g., TouchDesigner, Resolume, Max/MSP).

## Libraries to Use

*   **Spout2 C++ Library:** The core library for real-time video sharing between applications on Windows.

## Repositories to Clone

To get started with the development, you will need to clone the following repositories:

*   **Spout2:** The official Spout2 repository contains the necessary source code and examples.
    *   URL: `https://github.com/leadedge/Spout2`
*   **Omniverse App Template:** A template for building standalone Omniverse applications.
    *   URL: `https://github.com/NVIDIA-Omniverse/kit-app-template`
*   **Omniverse C++ Extension Template:** A template for creating C++ extensions for Omniverse Kit.
    *   URL: `https://github.com/NVIDIA-Omniverse/kit-extension-template-cpp`

## Running on Windows and Next Steps

This project is configured to be built on a Windows machine with the necessary development tools installed. Due to platform constraints, the C++ extension cannot be built on macOS.

### Windows Setup

1.  **Install Prerequisites:**
    *   Install **Microsoft Visual Studio (2019 or 2022)** with the **Desktop development with C++** workload.
    *   Install the **Windows SDK** from the Visual Studio Installer.

2.  **Build the Project:**
    *   Open a command prompt or PowerShell and navigate to the `kit-app-template` directory.
    *   Run the following command to build the application and the `omni.spout.sender` extension:
        ```bash
        .\repo.bat build
        ```

3.  **Launch the Application:**
    *   Once the build is successful, launch the application with:
        ```bash
        .\repo.bat launch
        ```

### What is Left to Do

The basic structure of the extension is in place, and Spout2 has been integrated. The final steps to achieve the project's objective are:

1.  **Implement Texture Capturing:**
    *   The core logic for grabbing the rendered texture from Omniverse needs to be implemented in `SpoutSenderExtension.cpp`.
    *   This will involve using the `omni::kit::hydra_texture` C++ API to get access to the render target of the viewport.

2.  **Send the Texture with Spout:**
    *   In the `onUpdate()` method of the `SpoutSenderExtension` class, the captured texture resource needs to be sent using the `m_spoutSender->SendTexture()` method.

3.  **Testing:**
    *   The extension needs to be tested with a Spout receiver application (like TouchDesigner or the Spout Receiver example) to verify that the texture is being sent correctly.
