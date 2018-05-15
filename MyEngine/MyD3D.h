#ifndef _MYD3D_
#define _MYD3D_

bool InitializeDirect3d11App(HINSTANCE hInstance);
void ReleaseObjects();
bool InitScene();
void UpdateScene();
void DrawScene();

#endif // !_MYD3D_