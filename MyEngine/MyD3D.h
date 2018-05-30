#ifndef _MYD3D_
#define _MYD3D_

bool InitializeDirect3d11App(HINSTANCE hInstance);
void ReleaseObjects();
bool InitScene(HINSTANCE & hInstance);
void UpdateScene(double time);
void DrawScene();
void CreateLight();

#endif // !_MYD3D_