//
//  ROCamera.cpp
//  rayne-oculus
//
//  Copyright 2013 by Überpixel. All rights reserved.
//  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
//  documentation files (the "Software"), to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
//  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
//  PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
//  FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
//  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#include "ROCamera.h"
#include <../Src/OVR_CAPI_GL.h>

namespace RO
{
	RNDefineMeta(Camera, RN::SceneNode)
	
	Camera::Camera(RN::Texture::Format format, RN::Camera::Flags flags)
	: _hmd(nullptr)
	{
		flags &= ~(RN::Camera::Flags::Fullscreen | RN::Camera::Flags::UpdateAspect);
		RN::Vector2 halfScreenSize = RN::Window::GetSharedInstance()->GetSize();
		halfScreenSize.x *= 0.5f;
		
		_head = new RN::SceneNode();
		_head->AddDependency(this);
		//AddChild(_head);
		
		_leftEye = new RN::Camera(halfScreenSize, format, flags | RN::Camera::Flags::NoFlush);
		_leftEye->SceneNode::SetFlags(RN::SceneNode::Flags::NoSave|RN::SceneNode::Flags::HideInEditor);
		_leftEye->SetFrame(RN::Rect(0.0f, 0.0f, halfScreenSize.x, halfScreenSize.y));
		_leftEye->SetRenderingFrame(_leftEye->GetFrame());
		_leftEye->SetBlitMode(RN::Camera::BlitMode::Unstretched);
		_leftEye->SetDebugName("OR::Camera::Left");
		_head->AddChild(_leftEye);
		
		_rightEye = new RN::Camera(halfScreenSize, format, flags | RN::Camera::Flags::NoFlush);
		_rightEye->SceneNode::SetFlags(RN::SceneNode::Flags::NoSave|RN::SceneNode::Flags::HideInEditor);
		_rightEye->SetFrame(RN::Rect(0.0f, 0.0f, halfScreenSize.x, halfScreenSize.y));
		_rightEye->SetRenderingFrame(_leftEye->GetFrame());
		_rightEye->SetBlitMode(RN::Camera::BlitMode::Unstretched);
		_rightEye->SetDebugName("OR::Camera::Right");
		_head->AddChild(_rightEye);

		//Shits broken: the above code somehow changes _hmd...
		_hmd = nullptr;
	}
	
	Camera::~Camera()
	{
		
	}
	
	void Camera::Update(float delta)
	{
		
	}
	
	void Camera::SetHMD(HMD *hmd)
	{
		if(_hmd == hmd)
			return;
		
		if(_hmd)
		{
			RN::OpenGLQueue::GetSharedInstance()->SubmitCommand([this] {
				ovrHmd_ConfigureRendering(_hmd->GetHMD(), nullptr, NULL, NULL, NULL);
			});
			
			RN::MessageCenter::GetSharedInstance()->RemoveObserver(this);
		}
		
		_hmd = hmd;
		
		if(_hmd)
		{
			RN::OpenGLQueue::GetSharedInstance()->SubmitCommand([this] {
				ovrGLConfig cfg;
				
				cfg.OGL.Header.API = ovrRenderAPI_OpenGL;
				cfg.OGL.Header.RTSize.w = _hmd->GetResolution().x;
				cfg.OGL.Header.RTSize.h = _hmd->GetResolution().y;
				cfg.OGL.Header.Multisample = 0;
#if defined(RN_PLATFORM_WINDOWS)
				cfg.OGL.Window = RN::Window::GetSharedInstance()->GetCurrentWindow();
				cfg.OGL.DC = RN::Window::GetSharedInstance()->GetCurrentDC();
#endif
				ovrEyeRenderDesc eyeRenderDesc[2];
				if(ovrHmd_ConfigureRendering(_hmd->GetHMD(), &cfg.Config, ovrDistortionCap_Chromatic | ovrDistortionCap_TimeWarp | ovrDistortionCap_Overdrive, _hmd->GetHMD()->DefaultEyeFov, eyeRenderDesc))
				{
					_leftEye->SetPosition(-RN::Vector3(eyeRenderDesc[HMD::Eye::Left].HmdToEyeViewOffset.x, eyeRenderDesc[HMD::Eye::Left].HmdToEyeViewOffset.y, eyeRenderDesc[HMD::Eye::Left].HmdToEyeViewOffset.z));
					_rightEye->SetPosition(-RN::Vector3(eyeRenderDesc[HMD::Eye::Right].HmdToEyeViewOffset.x, eyeRenderDesc[HMD::Eye::Right].HmdToEyeViewOffset.y, eyeRenderDesc[HMD::Eye::Right].HmdToEyeViewOffset.z));
					
					ovrMatrix4f ovrLeftProj = ovrMatrix4f_Projection(eyeRenderDesc[0].Fov, 0.01f, 500.0f, true);
					ovrMatrix4f ovrRightProj = ovrMatrix4f_Projection(eyeRenderDesc[1].Fov, 0.01f, 500.0f, true);
					
					RN::Matrix leftProj;
					RN::Matrix rightProj;
					
					leftProj.m[0] = ovrLeftProj.M[0][0];
					leftProj.m[1] = ovrLeftProj.M[1][0];
					leftProj.m[2] = ovrLeftProj.M[2][0];
					leftProj.m[3] = ovrLeftProj.M[3][0];
					
					leftProj.m[4] = ovrLeftProj.M[0][1];
					leftProj.m[5] = ovrLeftProj.M[1][1];
					leftProj.m[6] = ovrLeftProj.M[2][1];
					leftProj.m[7] = ovrLeftProj.M[3][1];
					
					leftProj.m[8] = ovrLeftProj.M[0][2];
					leftProj.m[9] = ovrLeftProj.M[1][2];
					leftProj.m[10] = ovrLeftProj.M[2][2];
					leftProj.m[11] = ovrLeftProj.M[3][2];
					
					leftProj.m[12] = ovrLeftProj.M[0][3];
					leftProj.m[13] = ovrLeftProj.M[1][3];
					leftProj.m[14] = ovrLeftProj.M[2][3];
					leftProj.m[15] = ovrLeftProj.M[3][3];
					
					
					rightProj.m[0] = ovrRightProj.M[0][0];
					rightProj.m[1] = ovrRightProj.M[1][0];
					rightProj.m[2] = ovrRightProj.M[2][0];
					rightProj.m[3] = ovrRightProj.M[3][0];
					
					rightProj.m[4] = ovrRightProj.M[0][1];
					rightProj.m[5] = ovrRightProj.M[1][1];
					rightProj.m[6] = ovrRightProj.M[2][1];
					rightProj.m[7] = ovrRightProj.M[3][1];
					
					rightProj.m[8] = ovrRightProj.M[0][2];
					rightProj.m[9] = ovrRightProj.M[1][2];
					rightProj.m[10] = ovrRightProj.M[2][2];
					rightProj.m[11] = ovrRightProj.M[3][2];
					
					rightProj.m[12] = ovrRightProj.M[0][3];
					rightProj.m[13] = ovrRightProj.M[1][3];
					rightProj.m[14] = ovrRightProj.M[2][3];
					rightProj.m[15] = ovrRightProj.M[3][3];
					
					_leftEye->SetProjectionMatrix(leftProj);
					_rightEye->SetProjectionMatrix(rightProj);
				}
			});
			
			RN::MessageCenter::GetSharedInstance()->AddObserver(kRNKernelDidBeginFrameMessage, [this](RN::Message *message){
				RN::OpenGLQueue::GetSharedInstance()->SubmitCommand([this] {
					ovrHmd_BeginFrame(_hmd->GetHMD(), 0);
					
					_pose = _hmd->GetPose();
					_head->SetWorldPosition(GetWorldPosition()+_pose.position);
					_head->SetWorldRotation(GetWorldRotation()*_pose.rotation);
				});
			}, this);
			
			RN::Window::GetSharedInstance()->SetFlushProc([this]{
				static int counter = 0;

				ovrPosef renderPose[2];
				renderPose[0].Position.x = _pose.position.x;
				renderPose[0].Position.y = _pose.position.y;
				renderPose[0].Position.z = _pose.position.z;
				renderPose[0].Orientation.x = _pose.rotation.x;
				renderPose[0].Orientation.y = _pose.rotation.y;
				renderPose[0].Orientation.z = _pose.rotation.z;
				renderPose[0].Orientation.w = _pose.rotation.w;
				renderPose[1] = renderPose[0];
				
				ovrGLTexture eyeTexture[2];
				
				eyeTexture[0].OGL.Header.API = ovrRenderAPI_OpenGL;
				eyeTexture[0].OGL.Header.TextureSize.w = _leftEye->GetFrame().GetSize().x;
				eyeTexture[0].OGL.Header.TextureSize.h = _leftEye->GetFrame().GetSize().y;
				eyeTexture[0].OGL.Header.RenderViewport.Size = eyeTexture[0].OGL.Header.TextureSize;
				eyeTexture[0].OGL.Header.RenderViewport.Pos.x = 0;
				eyeTexture[0].OGL.Header.RenderViewport.Pos.y = 0;
				if(_leftEye->GetPostProcessingPipelines().size() == 0)
				{
					eyeTexture[0].OGL.TexId = _leftEye->GetRenderTarget()->GetName();
				}
				else
				{
					eyeTexture[0].OGL.TexId = _leftEye->GetPostProcessingPipelines().back()->GetLastStage()->GetCamera()->GetRenderTarget()->GetName();
				}
				
				eyeTexture[1].OGL.Header.API = ovrRenderAPI_OpenGL;
				eyeTexture[1].OGL.Header.TextureSize.w = _rightEye->GetFrame().GetSize().x;
				eyeTexture[1].OGL.Header.TextureSize.h = _rightEye->GetFrame().GetSize().y;
				eyeTexture[1].OGL.Header.RenderViewport.Size = eyeTexture[1].OGL.Header.TextureSize;
				eyeTexture[1].OGL.Header.RenderViewport.Pos = eyeTexture[0].OGL.Header.RenderViewport.Pos;
				if(_rightEye->GetPostProcessingPipelines().size() == 0)
				{
					eyeTexture[1].OGL.TexId = _rightEye->GetRenderTarget()->GetName();
				}
				else
				{
					eyeTexture[1].OGL.TexId = _rightEye->GetPostProcessingPipelines().back()->GetLastStage()->GetCamera()->GetRenderTarget()->GetName();
				}
				
				if(counter > 100)
					ovrHmd_EndFrame(_hmd->GetHMD(), renderPose, (ovrTexture *)(eyeTexture));

				counter += 1;
			});
		}
	}
	
	void Camera::SetAmbientColor(const RN::Color &color)
	{
		_leftEye->SetAmbientColor(color);
		_rightEye->SetAmbientColor(color);
	}
	
	void Camera::SetBlitShader(RN::Shader *shader)
	{
		_leftEye->SetBlitShader(shader);
		_rightEye->SetBlitShader(shader);
	}
	
	RN::Camera *Camera::GetLeftCamera()
	{
		return _leftEye;
	}
	
	RN::Camera *Camera::GetRightCamera()
	{
		return _rightEye;
	}
	
	void Camera::DidUpdate(RN::SceneNode::ChangeSet changeSet)
	{
		RN::SceneNode::DidUpdate(changeSet);
		if(changeSet == RN::SceneNode::ChangeSet::Position)
		{
			_tempHeadPosition = GetWorldPosition();
			_tempHeadRotation = GetWorldRotation();
		}
	}
}
