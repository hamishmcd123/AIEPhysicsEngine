# AIEPhysicsEngine

This project is a basic rigid body physics engine. It was made for the "Physics For Games" unit at AIE Melbourne. It is built on top of **Line Renderer**, originally authored by Finn Morgan.

You can download Line Renderer on Finn's [website](https://dinaroozie.neocities.org/). The original project was made for use with Visual Studio, and is available in two flavours: GLFW or SDL.

If you don't use Visual Studio and can't be bothered setting up the CMake configuration yourself, I have a template repo [here](https://github.com/hamishmcd123/LineRendererCMake) using the SDL version. It probably doesn't follow CMake best practices, but it **works**.

The physics engine itself handles OBBs, circles and plane primitives with both linear and rotational collision resolution.

In preparation to make a custom engine for the end-of-year project, I also did a bunch of messing around with ImGUI, serialisation and reflection that don't really do too much.

## Limitations

- Since the project uses discrete collision detections, fast-moving objects are prone to tunnelling. 

- If the mass ratio between two objects is too high, the lighter object is likely to clip through planes.

- To simplify OBB collisions, the collision point is always chosen be the minimum vertex projected onto the separating axis (even for edge-to-edge collisions). As a result, long, thin OBBS will jitter like crazy when resting on surfaces.

- There is no broad-phase collision detection, so things will start to lag like crazy around ~200 physics bodies.

- My implementations for collisions are probably inefficient. Sorry.

## Other Thoughts
Although I probably won't end up using my own physics solution for my end-of-year project, I learnt a lot through making this. I would love to get better at game physics in the future.

If I *were* going to make this again, here are some thing I would like to add: 

- Generic polygon collision using SAT.
- Collision manifolds with OBB and other primitives.
- Contact forces.
- Basic constraints
- Joints

