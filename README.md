# RT in weeks

Just simply use whatever buildtool you have(it's Cmake to me) to build and run.

under project folder:

```powershell
cmake . -Bbuild
cmake --build build
```

and you should find the executable in *./build*.

***BE AWARE!!*** Due to my poor coding technics, your PC is much likely to be ***FROZEN*** during the run. Sorry about that :(

Here comes some images rendered from the little Ray Tracer :)

![Many Spheres](./img/readme/readme0.png "Many Spheres, 512 Samplers")

![Noise Sphere with Lighting](./img/readme/readme1.png "Noise Sphere with Lighting, 512 Samplers")

![Cornell Box](./img/readme/readme2.png "Cornell Box, 512 Samplers")

![Cornell Box Smoke](./img/readme/readme3.png "Cornell Box Smoke, 512 Samplers")