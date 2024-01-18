# Yeager Engine 
> 'Yeager', inspired by the character Eren Yeager from the Shingeki no Kyojin manga and anime 

3D/2D Renderer engine, made with OpenGL3.3. This project is current at early development stage, lacking performace and safe code. 
Future implementations are working autonomous modules, a safe engine core, proper UI librarie, and custom compiler. 
Current working toward asnyc and thread compartibily. 

```
cmake -DCMAKE_CXX_COMPILER:FILEPATH=<G++ Path> -S ./YeagerEngine -B ./YeagerEngine/build -G Ninja
cmake --build YeagerEngine/build --config Debug --target all --
```

# Libraries and Third Party Integrations
- Serialization : [yaml-cpp](https://github.com/jbeder/yaml-cpp)
- Window Manager : [glfw](https://github.com/glfw/glfw)
- OpenGL Math : [glm](https://github.com/g-truc/glm)
- Music and Sound Effect : [irrklang](https://www.ambiera.com/irrklang/)
- UI Interface : [imgui](https://github.com/ocornut/imgui)
- GL/GLES/EGL/GLX/WGL Loader : [glad](https://glad.dav1d.de/)
- GPU acceleration : [Nvidia Cuda](https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html)
- Log System : [loguru](https://github.com/Delgan/loguru)
- Model Loading : [assimp](https://github.com/assimp/assimp)
- Image Loading : [std_image](https://github.com/nothings/stb)
- File Manager Opening : [portable-file-dialogs](https://github.com/samhocevar/portable-file-dialogs)
