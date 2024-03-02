# RT in weeks

Just simply use whatever buildtool you have(it's Cmake to me) to build and run.

under project folder:

```powershell
cmake . -Bbuild
cmake --build build
```

and you should find the executable in _./build_.

to run the code directly:

```powershell
RayTracing.exe
```

or with custom samplers amount (taking 512 for an example)

```powershell
RayTracing.exe 512
```

**_BE AWARE!!_** Due to my poor coding technics, your PC is much likely to be **_FROZEN_** during the run. Sorry about that :(

Here comes some images rendered from the little Ray Tracer :)

![Many Spheres](./img/readme/readme0.png "Many Spheres, 512 Samplers")

![Noise Sphere with Lighting](./img/readme/readme1.png "Noise Sphere with Lighting, 512 Samplers")

![Image with all Features](./img/readme/readme4.png "all Features, 10240 Samplers")

![Cornel Box Modified](./img/readme/readme2.png "Cornel Box With Metal and Glass, 256 Samples")
