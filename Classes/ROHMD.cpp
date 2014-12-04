//
//  ROHMD.cpp
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

#include "ROHMD.h"
#include "ROSystem.h"

namespace RO
{
	HMD::HMD(ovrHmd hmd)
	: _hmd(hmd)
	{
		// Start the sensor which provides the Rift’s pose and motion.
		ovrHmd_ConfigureTracking(_hmd, ovrTrackingCap_Orientation |
								 ovrTrackingCap_MagYawCorrection |
								 ovrTrackingCap_Position, 0);
	}
	
	HMD::~HMD()
	{
		System::GetSharedInstance()->RemoveHMD(this);
		ovrHmd_Destroy(_hmd);
	}
	
	RN::Vector2 HMD::GetResolution()
	{
		return RN::Vector2(_hmd->Resolution.w, _hmd->Resolution.h);
	}
	
	RN::Vector4 HMD::GetDefaultFOV(Eye eye)
	{
		return RN::Vector4(_hmd->DefaultEyeFov[eye].LeftTan, _hmd->DefaultEyeFov[eye].RightTan, _hmd->DefaultEyeFov[eye].UpTan, _hmd->DefaultEyeFov[eye].DownTan);
	}
	
	void HMD::SetAsDisplay(bool captureMain)
	{
#if RN_PLATFORM_MAC_OS
		if(captureMain)
			CGDisplayCapture(CGMainDisplayID());
		
		CGDirectDisplayID RiftDisplayId = (CGDirectDisplayID)_hmd->DisplayId;
		
		RN::Screen *screen = RN::Window::GetSharedInstance()->GetScreenWithID(RiftDisplayId);
#elif RN_PLATFORM_WINDOWS
		RN::Screen *screen = RN::Window::GetSharedInstance()->GetMainScreen();
#endif
		if(screen)
		{
			RN::WindowConfiguration *configuration = new RN::WindowConfiguration(_hmd->Resolution.w, _hmd->Resolution.h, screen);
			RN::Window::GetSharedInstance()->ActivateConfiguration(configuration, RN::Window::Mask::Fullscreen|RN::Window::Mask::VSync);
			
			configuration->Release();

#if RN_PLATFORM_WINDOWS
			ovrHmd_AttachToWindow(_hmd, RN::Window::GetSharedInstance()->GetCurrentWindow(), NULL, NULL);
#endif
		}
		
		RN::Window::GetSharedInstance()->HideCursor();
		RN::Timer::ScheduledTimerWithDuration(std::chrono::seconds(5), [this] {
			ovrHmd_DismissHSWDisplay(_hmd);
		}, false);
	}
	
	HMD::Pose HMD::GetPose()
	{
		Pose pose;
		ovrTrackingState ts  = ovrHmd_GetTrackingState(_hmd, ovr_GetTimeInSeconds());
		if (ts.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked)) {
			ovrPosef ovrpose = ts.HeadPose.ThePose;
			
			pose.position = RN::Vector3(ovrpose.Position.x, ovrpose.Position.y, ovrpose.Position.z);
			pose.rotation = RN::Quaternion(ovrpose.Orientation.x, ovrpose.Orientation.y, ovrpose.Orientation.z, ovrpose.Orientation.w);
		}
		
		return pose;
	}
}
