//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: mtrlAlpha.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Renders a semi transparent teapot using alpha blending.  In this 
//       sample, the alpha is taken from the material's diffuse alpha value.
//       You can increase the opaqueness with the 'A' key and can descrease
//       it with the 'S' key.  
//          
//////////////////////////////////////////////////////////////////////////////////////////////////
#include"d3dUtility.h"
#include "pSystem.h"
#include "camera.h"
#include <cstdlib>
#include <ctime>

IDirect3DDevice9* Device = 0; 

const int Width  = 640;
const int Height = 480;

psys::PSystem* Exp = 0;


Camera TheCamera(Camera::AIRCRAFT);

IDirect3DVertexBuffer9* VB = 0;

IDirect3DTexture9* FloorTex  = 0;
IDirect3DTexture9* WallTex   = 0;
IDirect3DTexture9* MirrorTex = 0;

D3DMATERIAL9 FloorMtrl  = d3d::WHITE_MTRL;
D3DMATERIAL9 WallMtrl   = d3d::WHITE_MTRL;
D3DMATERIAL9 MirrorMtrl = d3d::WHITE_MTRL;

ID3DXMesh* Teapot = 0;
D3DXVECTOR3 TeapotPosition(0.0f, 3.0f, -7.5f);
D3DMATERIAL9 TeapotMtrl = d3d::YELLOW_MTRL;

void RenderScene();
void RenderMirror();
void RenderShadow();

struct Vertex
{
	Vertex(){}
	Vertex(float x, float y, float z, 
		   float nx, float ny, float nz,
		   float u, float v)
	{
		_x  = x;  _y  = y;  _z  = z;
		_nx = nx; _ny = ny; _nz = nz;
		_u  = u;  _v  = v;
	}
	float _x, _y, _z;
	float _nx, _ny, _nz;
	float _u, _v;

	static const DWORD FVF;
};
const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;



void RenderMirror()  //»æÖÆ¾µÃæ
{
	Device->SetRenderState(D3DRS_STENCILENABLE,true);
	Device->SetRenderState(D3DRS_STENCILFUNC,D3DCMP_ALWAYS);
	Device->SetRenderState(D3DRS_STENCILREF,0X1);
	Device->SetRenderState(D3DRS_STENCILMASK,0xffffffff);
	Device->SetRenderState(D3DRS_STENCILWRITEMASK,0xffffffff);
	Device->SetRenderState(D3DRS_STENCILZFAIL,D3DSTENCILOP_KEEP);
	Device->SetRenderState(D3DRS_STENCILFAIL,D3DSTENCILOP_KEEP);
	Device->SetRenderState(D3DRS_STENCILPASS,D3DSTENCILOP_REPLACE);


	Device->SetRenderState(D3DRS_ZWRITEENABLE,false);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
	Device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ZERO);
	Device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);



	Device->SetStreamSource(0,VB,0,sizeof(Vertex));
	Device->SetFVF(Vertex::FVF);
	Device->SetMaterial(&MirrorMtrl);
	Device->SetTexture(0,MirrorTex);
	D3DXMATRIX I;
	D3DXMatrixIdentity(&I);
	Device->SetTransform(D3DTS_WORLD,&I);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST,18,2);

	Device->SetRenderState(D3DRS_ZWRITEENABLE,true);



	Device->SetRenderState(D3DRS_STENCILFUNC,D3DCMP_EQUAL);
	Device->SetRenderState(D3DRS_STENCILPASS,D3DSTENCILOP_KEEP);

	D3DXMATRIX W,T,R;
	D3DXPLANE plane(0.0f,0.0f,1.0f,0.0f);
	D3DXMatrixReflect(&R,&plane);

	D3DXMatrixTranslation(&T,
		TeapotPosition.x,
		TeapotPosition.y,
		TeapotPosition.z);

	W = T * R;

	Device->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	Device->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_DESTCOLOR);
    Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

	
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&TeapotMtrl);
	Device->SetTexture(0, 0);

	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	Teapot->DrawSubset(0);
	
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	Device->SetRenderState( D3DRS_STENCILENABLE, false);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);


}


void RenderScene()
{
	Device->SetMaterial(&TeapotMtrl);
	Device->SetTexture(0,0);
	D3DXMATRIX W;
	D3DXMatrixTranslation(&W,TeapotPosition.x,TeapotPosition.y,TeapotPosition.z);

	Device->SetTransform(D3DTS_WORLD,&W);
	Teapot->DrawSubset(0);


	D3DXMATRIX I;
	D3DXMatrixIdentity(&I);
	Device->SetTransform(D3DTS_WORLD,&I);

	Device->SetStreamSource(0,VB,0,sizeof(Vertex));
	Device->SetFVF(Vertex::FVF);


	Device->SetMaterial(&FloorMtrl);
	Device->SetTexture(0,FloorTex);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST,0,2);

	Device->SetMaterial(&WallMtrl);
	Device->SetTexture(0,WallTex);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST,6,4);

	Device->SetMaterial(&MirrorMtrl);
	Device->SetTexture(0,MirrorTex);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST,18,2);

}


void RenderShadow()
{
	Device->SetRenderState(D3DRS_STENCILENABLE,true);
	Device->SetRenderState(D3DRS_STENCILFUNC,D3DCMP_EQUAL);
	Device->SetRenderState(D3DRS_STENCILREF,0X0);
	Device->SetRenderState(D3DRS_STENCILMASK,0xffffffff);
	Device->SetRenderState(D3DRS_STENCILWRITEMASK,0xffffffff);
	Device->SetRenderState(D3DRS_STENCILZFAIL,D3DSTENCILOP_KEEP);
	Device->SetRenderState(D3DRS_STENCILFAIL,D3DSTENCILOP_KEEP);
	Device->SetRenderState(D3DRS_STENCILPASS,D3DSTENCILOP_INCR);

	D3DXVECTOR4 lightDirection(0.707f,-0.707f,0.707f,0.0f);
	D3DXPLANE groundPnane(0.0f,-1.0f,0.0f,0.0f);

	D3DXMATRIX S;
	D3DXMatrixShadow(
		&S,
		&lightDirection,
		&groundPnane
		);

	D3DXMATRIX T;
	D3DXMatrixTranslation(
		&T,
		TeapotPosition.x,
		TeapotPosition.y,
		TeapotPosition.z
		);

	D3DXMATRIX W = T * S;
	Device -> SetTransform(D3DTS_WORLD,&W);

	Device -> SetRenderState(D3DRS_ALPHABLENDENABLE,true);
	Device -> SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	Device -> SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	D3DMATERIAL9 mtrl = d3d::InitMtrl(d3d::BLACK,d3d::BLACK,d3d::BLACK,d3d::BLACK,0.0f);
	mtrl.Diffuse.a = 0.5f;

	Device -> SetRenderState(D3DRS_ZENABLE,FALSE);

	Device -> SetMaterial(&mtrl);
	Device -> SetTexture(0,0);
	Teapot -> DrawSubset(0);

	Device -> SetRenderState(D3DRS_ZENABLE,true);
	Device -> SetRenderState(D3DRS_ALPHABLENDENABLE,false);
	Device -> SetRenderState(D3DRS_STENCILENABLE,false);

	

}
//
// Framework Functions
//
bool Setup()
{
	srand((unsigned int)time(0));

	D3DXVECTOR3 origin(0.0f, 10.0f, 50.0f);
	Exp = new psys::Firework(&origin, 6000);
	Exp->init(Device, "flare.bmp");

	d3d::DrawBasicScene(Device, 1.0f);
	
	WallMtrl.Specular = d3d::WHITE * 0.2f;

	D3DXCreateTeapot(Device, &Teapot, 0);

	
	Device->CreateVertexBuffer(
		24 * sizeof(Vertex),
		0, 
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&VB,
		0);

	Vertex* v = 0;
	VB->Lock(0, 0, (void**)&v, 0);

	
	v[0] = Vertex(-7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[1] = Vertex(-7.5f, 0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[2] = Vertex( 7.5f, 0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	
	v[3] = Vertex(-7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[4] = Vertex( 7.5f, 0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[5] = Vertex( 7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

	v[6]  = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[7]  = Vertex(-7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[8]  = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	
	v[9]  = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[10] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[11] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);


	v[12] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[13] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[14] = Vertex(7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	
	v[15] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[16] = Vertex(7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[17] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	v[18] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[19] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[20] = Vertex( 2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	
	v[21] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[22] = Vertex( 2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[23] = Vertex( 2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	VB->Unlock();
	
	
	D3DXCreateTextureFromFile(Device, "Checker-Mat.jpg", &FloorTex);
	D3DXCreateTextureFromFile(Device, "BRICK.jpg", &WallTex);
	D3DXCreateTextureFromFile(Device, "ice2.bmp", &MirrorTex);

	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	
	D3DXVECTOR3 lightDir(0.707f, -0.707f, 0.707f);
	D3DXCOLOR color(1.0f, 1.0f, 1.0f, 1.0f);
	D3DLIGHT9 light = d3d::InitDirectionalLight(&lightDir, &color);

	Device->SetLight(0, &light);
	Device->LightEnable(0, true);

	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);

	

	D3DXVECTOR3    pos(-10.0f, 3.0f, -15.0f);
	D3DXVECTOR3 target(0.0, 0.0f, 0.0f);
	D3DXVECTOR3     up(0.0f, 1.0f, 0.0f);

	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &pos, &target, &up);

	Device->SetTransform(D3DTS_VIEW, &V);

	
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
			&proj,
			D3DX_PI / 4.0f, 
			(float)Width / (float)Height,
			1.0f,
			5000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

void Cleanup()
{
	d3d::Release<IDirect3DVertexBuffer9*>(VB);
	d3d::Release<IDirect3DTexture9*>(FloorTex);
	d3d::Release<IDirect3DTexture9*>(WallTex);
	d3d::Release<IDirect3DTexture9*>(MirrorTex);
	d3d::Release<ID3DXMesh*>(Teapot);
	d3d::Delete<psys::PSystem*>( Exp );
	d3d::DrawBasicScene(0, 0.0f);
}

bool Display(float timeDelta)
{
	if( Device )
	{
		/*static float radius = 20.0f;

		if( ::GetAsyncKeyState(VK_LEFT) & 0x8000f )
			TeapotPosition.x -= 8.0f * timeDelta;

		if( ::GetAsyncKeyState(VK_RIGHT) & 0x8000f )
			TeapotPosition.x += 8.0f * timeDelta;

		if( ::GetAsyncKeyState(VK_UP) & 0x8000f )
			radius -= 8.0f * timeDelta;

		if( ::GetAsyncKeyState(VK_DOWN) & 0x8000f )
			radius += 8.0f * timeDelta;


		static float angle = (3.0f * D3DX_PI) / 2.0f;
	
		if( ::GetAsyncKeyState('A') & 0x8000f )
			angle -= 0.5f * timeDelta;

		if( ::GetAsyncKeyState('S') & 0x8000f )
			angle += 0.5f * timeDelta;

		D3DXVECTOR3 position( cosf(angle) * radius, 3.0f, sinf(angle) * radius );
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);*/

		if( ::GetAsyncKeyState('Z') & 0x8000f )
			TeapotPosition.x -= 8.0f * timeDelta;

		if( ::GetAsyncKeyState('X') & 0x8000f )
			TeapotPosition.x += 8.0f * timeDelta;

		if( ::GetAsyncKeyState(VK_UP) & 0x8000f )
			TheCamera.walk(4.0f * timeDelta);

		if( ::GetAsyncKeyState(VK_DOWN) & 0x8000f )
			TheCamera.walk(-4.0f * timeDelta);

		if( ::GetAsyncKeyState(VK_LEFT) & 0x8000f )
			TheCamera.yaw(-1.0f * timeDelta);
		
		if( ::GetAsyncKeyState(VK_RIGHT) & 0x8000f )
			TheCamera.yaw(1.0f * timeDelta);

		if( ::GetAsyncKeyState('N') & 0x8000f )
			TheCamera.strafe(-4.0f * timeDelta);

		if( ::GetAsyncKeyState('M') & 0x8000f )
			TheCamera.strafe(4.0f * timeDelta);

		if( ::GetAsyncKeyState('W') & 0x8000f )
			TheCamera.pitch(1.0f * timeDelta);

		if( ::GetAsyncKeyState('S') & 0x8000f )
			TheCamera.pitch(-1.0f * timeDelta);

		/*D3DXMatrixLookAtLH(&V, &position, &target, &up);
		Device->SetTransform(D3DTS_VIEW, &V);

		Device->Clear(0, 0, 
			D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
			0xff000000, 1.0f, 0L);

		Device->BeginScene();*/

		D3DXMATRIX V;

		TheCamera.getViewMatrix(&V);
		Device->SetTransform(D3DTS_VIEW, &V);

		Exp->update(timeDelta);

		if( Exp->isDead() )
			Exp->reset();

		//
		// Draw the scene:
		//

		Device->SetTransform(D3DTS_VIEW, &V);
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0L);
		Device->BeginScene();

		RenderScene();
		
		RenderMirror();	
		RenderShadow();

		D3DXMATRIX I;
		D3DXMatrixIdentity(&I);
		Device->SetTransform(D3DTS_WORLD, &I);

		d3d::DrawBasicScene(Device, 1.0f);

		Device->SetTransform(D3DTS_WORLD, &I);
		Exp->render();
		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}
	return true;
}


//
// WndProc
//
LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
		
	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE )
			::DestroyWindow(hwnd);
		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance,
				   HINSTANCE prevInstance, 
				   PSTR cmdLine,
				   int showCmd)
{
	if(!d3d::InitD3D(hinstance,
		Width, Height, true, D3DDEVTYPE_HAL, &Device))
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}
		
	if(!Setup())
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop( Display );

	Cleanup();

	Device->Release();

	return 0;
}