# Hydra IPC PoC

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

> **⚠️ DISCLAIMER: STRICTLY FOR EDUCATIONAL PURPOSES**
> This Proof of Concept (PoC) interacts with runtime memory and uses inline hooking to demonstrate cross-process communication. Modifying game or application memory may violate Terms of Service (ToS) or End User License Agreements (EULA). The author does not condone or support the use of this code for malicious activities, cheating, or gaining an unfair advantage. The author is not responsible for any misuse, account bans, or damages resulting from the use or compilation of this code.

## Overview
This repository serves as a practical, high-stress Proof of Concept for the **[Hydra IPC Library](https://github.com/DIA4A/Hydra-IPC)**. 

Operating on top of a game environment, this PoC seamlessly synchronizes action instances across distinct processes by integrating itself into a core function within the main loop of the process. It is provided strictly to demonstrate the speed, ease of use, and state-sharing capabilities of the underlying Hydra architecture.

## Ingame footage
<img src="assets/hydra-poc.gif" alt="Ingame demonstration of the PoC" width="800"/>

## Build Instructions
This project requires **Visual Studio 2026**, **C++20**, and the core Hydra library in the same parent folder to compile. It targets **x64** architecture exclusively.

1. Clone this repository: `git clone https://github.com/DIA4A/Hydra-RotMG-Exalt-Multibox.git`
2. Ensure you have the Hydra IPC library in the same parent folder as Hydra-RotMG-Exalt-Multibox.
3. Open the solution file in Visual Studio 2026.
4. Set the Build Configuration to **Release** or **Debug** and Platform to **x64**.
5. Build the solution. The compiled binary will be located in `/x64/Release/` (or `/x64/Debug/` respectively).

## Third-Party Libraries
This Proof of Concept relies on the following open-source libraries:
* **[MinHook](https://github.com/TsudaKageyu/minhook)**: Used for inline hooking.
* **[libil2cpp](https://github.com/creaffy/libil2cpp)**: A version of this library was slightly altered and used for dealing with IL2CPP/Unity functions and structures.

## License
This demonstration code is licensed under the [MIT License](LICENSE). Please respect the educational intent of this project.