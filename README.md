# VRHeightSimulator
This is A VR demo game that lets you experience being different heights in different environments, made in Unreal Engine.
The goal of this project is to illustrate many principles of VR games in primarily C++.

This is currently an early work in progress. 

## Controls
1) Touchpad button: Teleport
2) Underside Trigger: Pick up interactable object
3) Menu button: Toggle GUI
4) Grip button: Toggle webcam 

## System requirements
1) A VR System, optionally with a camera for AR Mode (Tested with the Vive Pro)
2) A computer with SteamVR support
3) Unreal Engine 5.1 or later

## How to try it
As of right now, there is no precompiled binary. You will need to build the project from source
1) Ensure your system meets the requirements
2) Clone with `--depth=1` or Download as Zip
3) Open the `.uproject` file
4) Play! 
Currently, there are no finished environments. In the future I will make it easier to integrate your own levels into this game. 

## Contributing
Feel free to make pull requests! To add objects that the player can pick up, subclass `InteractableObject`.
