# SDLSharedContext
Opengl shared context test with SDL.

- ShareSrcTextureApp  
Initialize 단계에서 main thread에서 활용할 SDL_window(main window)와 SDL_GLContext(main context), sub thread에서 활용할 SDL_window(sub window)와 SDL_GLContext(sub context) 생성.
이 때 sub context는 main thread와 자원을 공유하는 shared context로 생성한다.
main context에서 texture(planet texture) 생성 및 초기화.
Run loop에서 main window와 sub window에 같은 source texture(planet texture)를 렌더링한다.

- SingleOffscreenRenderingApp  
SDL_window, SDL_GL_Context만 초기화.
texture(planet texture) 생성 및 초기화.
offscreen buffer에 렌더링 및 해당 framebuffer .ppm파일로 출력.

- ShareDstTextureApp  
Initialize 단계에서 main thread에서 활용할 SDL_window(main window)와 SDL_GLContext(main context), sub thread에서 활용할 SDL_window(sub window)와 SDL_GLContext(sub context) 생성.
이 때 sub context는 main thread와 자원을 공유하는 shared context로 생성한다.
main context에서 texture(planet texture) 생성 및 초기화.
sub thread에서 shared source texture를 offscreen buffer에 렌더링 및 해당 framebuffer .ppm파일로 출력.
