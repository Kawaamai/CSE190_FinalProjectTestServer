/************************************************************************************

Authors     :   Bradley Austin Davis <bdavis@saintandreas.org>
Copyright   :   Copyright Brad Davis. All Rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

************************************************************************************/


#include <memory>
#include <exception>

#include <Windows.h>

#define __STDC_FORMAT_MACROS 1
#define APP_ID "958062084316416"

#include "Core.h"

bool checkFramebufferStatus(GLenum target = GL_FRAMEBUFFER) {
	GLuint status = glCheckFramebufferStatus(target);
	switch (status) {
	case GL_FRAMEBUFFER_COMPLETE:
		return true;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		std::cerr << "framebuffer incomplete attachment" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		std::cerr << "framebuffer missing attachment" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		std::cerr << "framebuffer incomplete draw buffer" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		std::cerr << "framebuffer incomplete read buffer" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		std::cerr << "framebuffer incomplete multisample" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
		std::cerr << "framebuffer incomplete layer targets" << std::endl;
		break;

	case GL_FRAMEBUFFER_UNSUPPORTED:
		std::cerr << "framebuffer unsupported internal format or image" << std::endl;
		break;

	default:
		std::cerr << "other framebuffer error" << std::endl;
		break;
	}

	return false;
}

bool checkGlError() {
	GLenum error = glGetError();
	if (!error) {
		return false;
	}
	else {
		switch (error) {
		case GL_INVALID_ENUM:
			std::cerr << ": An unacceptable value is specified for an enumerated argument.The offending command is ignored and has no other side effect than to set the error flag.";
			break;
		case GL_INVALID_VALUE:
			std::cerr << ": A numeric argument is out of range.The offending command is ignored and has no other side effect than to set the error flag";
			break;
		case GL_INVALID_OPERATION:
			std::cerr << ": The specified operation is not allowed in the current state.The offending command is ignored and has no other side effect than to set the error flag..";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			std::cerr << ": The framebuffer object is not complete.The offending command is ignored and has no other side effect than to set the error flag.";
			break;
		case GL_OUT_OF_MEMORY:
			std::cerr << ": There is not enough memory left to execute the command.The state of the GL is undefined, except for the state of the error flags, after this error is recorded.";
			break;
		case GL_STACK_UNDERFLOW:
			std::cerr << ": An attempt has been made to perform an operation that would cause an internal stack to underflow.";
			break;
		case GL_STACK_OVERFLOW:
			std::cerr << ": An attempt has been made to perform an operation that would cause an internal stack to overflow.";
			break;
		}
		return true;
	}
}

void glDebugCallbackHandler(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *msg, GLvoid* data) {
	OutputDebugStringA(msg);
	std::cout << "debug call: " << msg << std::endl;
}


//#include "ExampleApp.hpp"
#include "TestPhysicsApp.hpp"
#include "yojimbo.h"
#include "GameConstants.h"
//#include <thread>

// Execute our example class
//int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
//	int result = -1;
//	try {
//		if (!OVR_SUCCESS(ovr_Initialize(nullptr))) {
//			FAIL("Failed to initialize the Oculus SDK");
//		}
//		result = ExampleApp().run();
//	}
//	catch (std::exception & error) {
//		OutputDebugStringA(error.what());
//		std::cerr << error.what() << std::endl;
//	}
//	ovr_Shutdown();
//	return result;
//}

// Execute our example class
int main(int argc, char **argv) {
	int result = -1;
	try {
		// init ovr
		if (!OVR_SUCCESS(ovr_Initialize(nullptr))) {
			FAIL("Failed to initialize the Oculus SDK");
		}
		ovrAvatar_Initialize(APP_ID);

		// init yojimbo
		std::cout << "Init client" << std::endl;
		if (!InitializeYojimbo()) {
			std::cerr << "Error: failed to initialize Yojimbo!" << std::endl;
			return 1;
		}
		yojimbo_log_level(YOJIMBO_LOG_LEVEL_INFO);

		//yojimbo::Address serverAddress = yojimbo::Address("127.0.0.1", ServerPort);
		yojimbo::Address serverAddress = yojimbo::Address("128.54.70.69", ServerPort);
		{ // need this to properly call yojimbo::BaseClient desctructor
			//TestPhysicsApp client(serverAddress);
			//result = ExampleApp().run();
			//result = TestPhysicsApp().run();
			// TODO: run
			// TODO: disconnect client
			TestPhysicsApp client(serverAddress);
			client.run();
			//std::cout << "starting visual thread" << std::endl;
			//std::thread visualThread(&TestPhysicsApp::run, &client);
			//std::cout << "starting client networking thread" << std::endl;
			//std::thread networkThread(&TestPhysicsApp::Run, &client);
			
			//networkThread.join(); // block on network thread
			std::cerr << "disconnect client" << std::endl;
			client.Disconnect();
		}
	}
	catch (std::exception & error) {
		OutputDebugStringA(error.what());
		std::cerr << error.what() << std::endl;
	}
	std::cerr << "shutdown yojimbo" << std::endl;
	ShutdownYojimbo();
	ovr_Shutdown();
	std::cerr << "shutdown ovrAvatar" << std::endl;
	ovrAvatar_Shutdown();
	return result;
}