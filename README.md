# AIEPhysicsEngine

This project is a basic rigid body physics engine. It was made for the "Physics For Games" unit at AIE Melbourne, and is built upon Line Renderer, originally authored by Finn Morgan.

The engine handles OBBs, Circles and plane primitives with both linear and rotational collision resolution.

In preparation to make a custom engine for the end-of-year project, I also did a bunch of messing around with ImGUI, serialisation and reflection that don't really do too much.

## Limitations

- Since the project uses discrete collision detections, fast-moving objects are prone to tunnelling. 

- If the mass-ratio between two objects is too high, the lighter object is likely to clip through planes.

- To simplify OBB collisions, I chose the collision point to always be the minimum vertex projected onto the separating axis (even for edge-to-edge collisions). The result of this is that long, thin OBBS will jitter like crazy when on the ground.

- There is no broad-phase collision checking, so things will start to lag like crazy around ~200 physics bodies.

- My implementations for collisions are probably horribly-inefficient. Sorry.

## Other Thoughts
Although I probably won't end up using my own physics solution for my end-of-year project, I learnt a lot through making this, and would love to get better at game physics in the future.

If I *were* going to make this again, here are some thing I would like to add: 

- Generic polygon collision using SAT.
- Collision manifolds with OBB and other collisions.

