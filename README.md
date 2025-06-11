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
* Make sure you enable plugins from the settings on the content browser
* ![image](https://github.com/user-attachments/assets/7e53e9a8-4dc4-42d9-ada0-df8aca305e8b)
* Place the **Quad Environment** actor into your level to get started with the simulation.

---
## Drone Configuration

The drone parameters can be customized by editing the `Config/DroneConfig.json` file. This includes parameters such as:

```json
{
	"flight_parameters":
	{
		"max_velocity_bound": 800,
		"max_velocity": 130,
		"max_angle": 15,
		"max_pid_output": 10000,
		"altitude_threshold": 0.60000002384185791,
		"min_altitude_local": 500,
		"acceptable_distance": 50
	},
	"controller":
	{
		"altitude_rate": 400,
		"yaw_rate": 90,
		"min_velocity_for_yaw": 10
	},
	"obstacle_parameters":
	{
		"outer_boundary": 100,
		"inner_boundary": 100,
		"spawn_height": 0
	}
}
```
Customization – All values can be tweaked at runtime through the in‑game ImGui window or offline by editing the JSON file (usually in Content/Config or Saved/Config).

Work in progress – Obstacle parameters tie into the forthcoming ROS 2 / ZeroMQ obstacle‑creation service that will generate dynamic hazards inside the defined spawn region.

Units and semantics

Velocities are in centimetres per second (cm/s).

acceptable_distance is measured in centimetres and tells the AutoWaypoint controller how close the drone must get to a waypoint before it advances to the next one.

altitude_threshold (metres) provides a small dead‑band that prevents oscillation in altitude‑hold mode.
---

### License

This project is licensed under the MIT License. See the LICENSE file for details.

---

**Enjoy realistic quadcopter simulations in Unreal Engine 5.5.4 on Windows!**
