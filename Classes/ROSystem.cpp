//
//  ROSystem.cpp
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

#include "ROSystem.h"

namespace RN
{
	namespace oculus
	{
		RNDefineSingleton(System)
		
		System::System()
		{
			ovr_Initialize();
			DetectNewHMDs();
		}
		
		System::~System()
		{
			for(auto hmd : _hmds)
			{
				hmd.second->Release();
			}
			
			ovr_Shutdown();
		}
		
		HMD *System::GetHMD(int index)
		{
			if(_hmds.count(index) == 0)
			{
				ovrHmd ovrhmd = nullptr;
				if(index == -1)
				{
					ovrhmd = ovrHmd_CreateDebug(ovrHmdType::ovrHmd_DK2);
				}
				else
				{
					ovrhmd = ovrHmd_Create(index);
				}
				if(ovrhmd)
				{
					HMD *hmd = new HMD(ovrhmd);
					_hmds.insert(std::pair<int, HMD*>(index, hmd));
				}
				else
				{
					return nullptr;
				}
			}
			
			return _hmds[index];
		}
		
		void System::DetectNewHMDs()
		{
			_connectedCount = ovrHmd_Detect();
		}
		
		int System::GetConnectedCount() const
		{
			return _connectedCount;
		}
		
		void System::RemoveHMD(HMD *hmd)
		{
			for(auto other : _hmds)
			{
				if(other.second == hmd)
				{
					_hmds.erase(other.first);
					return;
				}
			}
		}
	}
}
