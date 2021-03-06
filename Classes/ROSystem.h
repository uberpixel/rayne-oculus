//
//  ROSystem.h
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

#ifndef __RAYNE_OCULUS_SYSTEM__
#define __RAYNE_OCULUS_SYSTEM__

#include <Rayne/Rayne.h>
#include <OVR.h>

#include "ROHMD.h"

namespace RN
{
	namespace oculus
	{
		class System : public RN::ISingleton<System>
		{
		public:
			friend HMD;
			
			System();
			~System() override;
			
			HMD *GetHMD(int index = 0);
			int GetConnectedCount() const;
			void DetectNewHMDs();
			
		private:
			void RemoveHMD(HMD *hmd);
			
			std::map<int, HMD *> _hmds;
			int _connectedCount;
			
			RNDeclareSingleton(System)
		};
	}
}

#endif //__RAYNE_OCULUS_SYSTEM__
