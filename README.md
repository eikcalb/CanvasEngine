# 700105 - Lab I (Final Exercise) Multiplayer Sketchpad

The purpose of this assessment is to design and implement a network-based game engine and to demonstrate its effectiveness by developing a game using this engine.

## Concept

You are to create a simple "game" using your networked-based game engine for two or more players. The style of the game is a voxel based competitive sketchpad.  

The precise details of the game design are unspecified, but as a minimum, the game should include:

- A canvas consisting of a large number of voxels (3D pixels), a minimum of which is an arrangement of 512 x 512 x 1 voxels, arranged in a regular grid.
- One player per PC

A key requirement of this assessment is to produce a very responsive/smooth sketching programme, with minimal latency when drawing on the canvas across multiple PCs.

## Specification

### Peers

A peer is defined as a single instance of the game, controlled by a single player.

Each peer is hosted on a different PC.

Each peer stores ONLY its own version of the canvas.

As a player draws on their canvas, that information is communicated to all other peers.

A peer-to-peer network infrastructure is required with a minimum of FOUR peers.

Peers can join and leave the game at any time.  The impact on the shared canvas when a player leaves or joins, is unspecified, but is to be handled appropriately.

### Canvas

The canvas is represented as a regular grid of voxels, where each voxel initially starts with a predefined colour and mass.  The initial colour of the voxel is that of the player (each player is assigned a unique colour).  Therefore, each canvas on each PC will start with a different colour.  The mass of all voxels is initially one unit.  

A player draws on the canvas by holding down the left mouse button and moving the mouse across the canvas.  Only voxels directly beneath the mouse are "drawn".

By drawing, a player steals one unit of mass from an equivalently positioned voxel on the canvas of a randomly selected player.  If the losing player ends with a voxel of mass 0, then a hole is created in their canvas (i.e. that user will not have any voxel at that location).  If the "drawn" voxel already exists, then its mass is increased by one unit, and its colour becomes a combination of the original colour and the colour of the stolen voxel.  If the "drawn" voxel did not previous exist, then its mass is set to one unit, and its colour becomes that of the stolen voxel.

How the voxel beneath the mouse is selected, then represented, is left open for you to decide.

How the colours are combined is left open for you to decide.  Possible solutions include striping or mixing the colours.

As the game progresses, players will continue to asynchronously steal voxel masses from each other.

### Visualisation

Each peer has a unique view of the canvas.  The colour of the canvas will be different across all peers, as will the visibility of any voxels that have been migrated to other players

As a minimum, the game should be rendered using a top-down camera in 2D.  Additional marks will be available for a 3D environment.

Details of the camera(s) are left open for you to decide.

### Time

There is no simulation loop per sa, so delta time is not required.

### Integrity check

Over time it is likely that the views of the world will become unsynchronised.  An integrity check is required to show where any errors have occurred.

When the integrity check mode is enabled, all voxels temporarily turn grey.  The system then totals the mass at each location within the canvas.  This is done by querying that location across all peers.  Any voxel with a mass less than the number of players is coloured blue.  Any voxel with a mass greater than the number of players is coloured red.

If the system is working correctly, you should never see a red or blue voxel.

### Head up display

Use the IMGUI to display the following, for debug purposes:

- Number of players
- The total mass of voxels on the current PC
- The actual mass of voxels across all PCs
- The starting mass of voxels across all PCs
- Target frequency of the graphics (in Hz), minimum 1 Hz [changeable]
- Actual frequency of the graphics (in Hz),
- Target frequency of the networking (in Hz), minimum 1 Hz [changeable]
- Actual frequency of the networking (in Hz)

Note that some of these values will be changeable and effect the simulation.

The frequency of the graphics and networking are important for the marking process.  Failure to display these correctly will result in a significant mark penalty.

### Controls

The control scheme uses the mouse and keyboard as follows:

- R to reset the system
- M to toggle the integrity check to display voxel mass
- U, J to increase/decrease the target frequency of the graphics, minimum 1 Hz
- Y, H to increase/decrease the target frequency of the networking, minimum 1 Hz
- Camera controls are unspecified.

## Implementation

### Game Engine

You are to implement a game engine framework in C++ from a Component based game engine design. In particular, you will fully implement the following game elements:

- Abstract renderer with a concrete implementation of at least one Windows PC renderer with IMGUI intergrated.
- Resource Manager.
- Input Manager.
- Appropriate Systems/Components/Managers/Messages/Objects that are useful for your game.

The remainder of the game engine beyond this can be left as a skeleton implementation that has sufficient implementation to allow the game engine to be useable. Therefore, you will create the classes in C++ and any methods that they would use, but no actual definition code has to be implemented inside of these methods unless it is required.

A modular design, including engine defines types, and generic C++ code should be used to allow for ease of porting to other platforms.

### Game

You are to implement the competitive sketchpad game described above using your game engine.

### Networking

Only the Winsock 2 and IMGUI libraries are permitted.

This application is a peer-to-peer architecture.  No server of any type is permitted.

TCP or UDP network protocols can be used to transfer the data between peers.  

The minimum requirement is **four** peers on at least **two** separate physical PCs in 177.

A network simulation tool will be inserted between the peers to simulate network latency and packet loss.  Your application must be able to cope with the following worst-case network quality of service:

- Latency: 100ms ± 50ms
- Packet loss: 20%

The IP address and ports should be stored in a configuration file or alternatively, the UDP broadcast mechanism can be used to identify peers on the network.

### Threads

Only C++ 20 threads are permitted.  No other library including OpenMP is permitted.

The major components within each application are required to operate asynchronously.  The choice of threading architecture is undefined.

The i7 processors in lab 177 are considered the target platform.  Threading is to be used to leverage the performance of these processors.

As part of the final assessment, you will be required to run various parts of your system at different target frequencies.  For example, the graphics may be requested to be run at 100 frames per second (Hz), whilst the networking at 10 Hz.

This ability of individual components to run asynchronously is a major part of this assessment. Failure to implement this correctly will greatly reduce your marks.

### Process Affinity

In order to ease debugging and to simplify the assessment process, you will be required to use a very specific thread to processor mapping.

|Logical Processor|Application|
|---|---|
|1|Graphics|
|2|Networking|

You can use any number of threads in your applications provided you have sufficient to meet the processor mapping in the table above.  For example, it is acceptable to use 4 networking threads in the game, provided that they are all located on core 1+.

## Research questions

The following are a few points you may wish to consider:

- What is the visual impact of a remotely drawn voxel, especially in periods of high network latency?
- The game starts with one player.  As new players join, how are the details of the world migrated to all peers?
- As peers leave the game, how does this information get communicated to all other peers?

## Deliverables

### Game Engine (Code)

Specified above.

### Game (Code)

Specified above.

### Lab book entries

1. UML diagrams illustrating your final engine design.  Your design will show all classes that have been used in your framework.  The diagrams will only consider the class relationships (inheritance, associations, singletons, etc.) and should not include any member methods or member data. If your design is complex, then you must create an overview of the whole design, and then include class diagrams of sub-sections.
2. You will include design descriptions and **justifications** of your game engine design - this will include references to the four game engine principles of abstractness, modularity, reusability, and simplicity.
You will include a matrix/table describing each composition of each Object/Entity in terms of components.
3. Networking and Concurrency - you will describe the system architecture of your game engine, specifically where threads and networking have been used.

## Submission

### Software

Your code, in the form of a Visual Studio solution, source code, and working executables/libraries, along with any required assets.

### Lab book

The full lab book containing all labs **A - I**

### Video

A 3 to 5 minute video with audio commentary.  The video should highlight each of the key elements of your implementation.  It is strongly recommended that you work through the list of mandatory and optional features within the specification, showing each in turn.

### Demonstration

You will be required to both demonstrate your solution, and take part in a code review (RESIT only).  The code review will consist of your examiners selecting parts of your code and requiring you to explain the implementation and the rationale behind the design.

All submissions are via Canvas.
