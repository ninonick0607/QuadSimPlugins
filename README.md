## QuadSimPlugin

**System Requirements**

* **Operating System:** Windows 10 or 11
* **Unreal Engine Version:** 5.5.4

---

### Overview

QuadSimPlugin is a Windows-specific plugin for Unreal Engine 5.5.4 that provides a high-fidelity quadcopter simulation environment. It bridges the gap between simulation and reality by leveraging Unreal Engine's physics system to create realistic drone behavior and integrates with external control systems.

**Key Features:**

* Physically accurate quadcopter model
* PID-based flight control implementation
* Support for multiple flight modes
* Integration with ZeroMQ and ROS2 for external control
* Debug UI for real-time parameter tuning

---

### Installation

1. **Clone or download** the QuadSimPlugin repository to your local machine.
2. **Create** a `Plugins` folder in your Unreal project directory if it doesn't already exist.
3. **Copy** the entire contents of the QuadSimPlugin repository into the `Plugins` folder.
4. **Open** your Unreal Engine 5.5.4 project. The plugin will be detected and loaded automatically.

---

### Usage

* After launching your project, navigate to **Edit > Plugins** and ensure **QuadSimPlugin** is enabled.
* Restart the editor if prompted.
* Place the **QuadPawn** actor into your level to get started with the simulation.

---

### License

This project is licensed under the MIT License. See the LICENSE file for details.

---

**Enjoy realistic quadcopter simulations in Unreal Engine 5.5.4 on Windows!**
