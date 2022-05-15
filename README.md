# Modbus-TCP-Controller
Course work on system software development for the 4th semester

The aim of the course work is to develop software that provides remote control of a peripheral device over a local network, using Modbus TCP as a network protocol.

The software should consist of two parts - server and client.

**The server must:**
- receive data and control commands from the client program via
local area network via a subset of the Modbus TCP protocol
– provide control of the USBHID peripheral device by writing and reading data by commands.

**The client must:**
- interact with the user
– connect to a server in the local network and send commands to it for execution via the Modbus TCP protocol, or show the status of input devices of the debug kit.
