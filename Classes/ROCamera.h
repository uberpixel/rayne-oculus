//
//  ROCamera.h
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

#ifndef __RAYNE_OCULUS_CAMERA__
#define __RAYNE_OCULUS_CAMERA__

#include <Rayne/Rayne.h>
#include "ROSystem.h"

namespace RO
{
	class Camera : public RN::SceneNode
	{
	public:
		Camera(RN::Texture::Format format, RN::Camera::Flags flags=RN::Camera::Flags::Defaults);
		~Camera();
		
		void Update(float delta) override;
		
		void SetHMD(HMD *hmd);
		
		void SetAmbientColor(const RN::Color &color);
		void SetBlitShader(RN::Shader *shader);
		RN::Camera *GetLeftCamera();
		RN::Camera *GetRightCamera();
		
		RN::SceneNode *GetHead();
		
	private:
		void InitializeOculus();
		
		RN::Camera *_rightEye;
		RN::Camera *_leftEye;
		RN::SceneNode *_head;
		HMD::Pose _pose;
		
		HMD *_hmd;
		bool _inFrame;
		bool _validFramebuffer;
		
		RNDeclareMeta(Camera)
	};
}

#endif //__RAYNE_OCULUS_CAMERA__
