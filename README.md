# OmniTouch

OmniTouch is an NVIDIA Omniverse extension that makes it possible to grab the rendered texture layer of Omniverse and pass it to external software, such as TouchDesigner, using the Spout2 C++ library.

## How to Run on Windows

This project is designed to be built and run on a Windows machine.

### Prerequisites

Before you begin, ensure you have the following installed:

*   **Microsoft Visual Studio (2019 or 2022):** Make sure to include the **Desktop development with C++** workload during installation.
*   **Windows SDK:** Install this alongside Visual Studio.
*   **Git and Git LFS:** For cloning the repositories.

### Setup and Run

1.  **Clone the repository and initialize submodules:**
    ```bash
    git clone <your-repository-url>
    cd omnitouch
    git submodule update --init --recursive
    ```

2.  **Build the Project:**
    *   Open a command prompt or PowerShell and navigate to the `kit-app-template` directory (which is a submodule within this repository).
    *   Run the following command to build the application and the `omni.spout.sender` extension:
        ```bash
        .\repo.bat build
        ```

3.  **Launch the Application:**
    *   Once the build is successful, launch the application with:
        ```bash
        .\repo.bat launch
        ```
    *   Follow the prompts to select the `omnitouch-app` to launch.

## Project Status and Next Steps

The basic structure of the extension is in place, and Spout2 has been integrated. The core functionality of grabbing the rendered texture from Omniverse and sending it via Spout is still under development.

**Remaining tasks:**

1.  **Implement Texture Capturing:** Implement the logic in `SpoutSenderExtension.cpp` to acquire the rendered texture from Omniverse using the `omni::kit::hydra_texture` C++ API.
2.  **Send Texture via Spout:** Utilize the `m_spoutSender->SendTexture()` method within the `onUpdate()` function to transmit the captured texture.
3.  **Testing:** Thoroughly test the extension with a Spout receiver application (e.g., TouchDesigner) to confirm correct texture transmission.
