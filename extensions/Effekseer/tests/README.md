# Built-in model renderer tests

Build the WebAssembly test app with `axmol -p wasm -t cpp-tests`. Open it in a browser to exercise shader linking and drawing.

In **cpp-tests → Particles(Effekseer)**:

- The first page still starts with Homing Laser. Use its effect selector to play **Simple Model UV** (the original file-based effect).
- Use the **suite navigation arrows at the bottom** to visit all six model material cases. Each case submits two instances of the same mesh in one `BeginRendering`/`EndRendering` group, with different transforms, world-space depths, and colors. Drag to orbit with the same perspective camera and ground grid as the original gallery. Lit should show shading; advanced color cases use separate color, alpha, and blend textures. Distortion cases displace a known checkerboard; the existing Simple Distortion effect exercises live framebuffer capture. A draw-count failure is shown on screen and logged.
- **Model Wireframe** should show only mesh edges, not filled triangles. **Model failure isolation** deliberately requests a missing archive and, on GL/GLES, an incompatible shader pair. Errors from those two probes are expected. A distortion pass with no background should submit zero draws, then two ordinary arrows and the laser should still render.

**Plain particle wrap and filter** draws four ordinary sprites in the same frame with one texture and UVs from 0 to 2. Repeat on the left should tile; Clamp on the right should extend the edge. The top row uses Nearest and the bottom row Linear.

All scenes use the existing `axmol-sample-assets` content deployed with cpp-tests (`Simple_Model_UV.efkefc`, `Model/Arrow.efkmodel`, `Texture/Arrow.png`, and the laser/distortion resources). No editor or unpacked Effekseer release is needed at runtime.
