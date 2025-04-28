[![](https://github.com/MiracleGames/MiracleGamesUWPSDK/raw/master/images/MGLogo.png)](https://www.mguwp.net/index.html)

# MiracleGamesC++SDK

Provide one stop solution for C++ developers!

## Overview

*   Miracle Games SDK (abbreviation: MG SDK) starts its lifespan after the release of Microsoft 10 system. With the mission of serving developers, the SDK fully uses various types of supports and services provided by Microsoft, and combines the self-developed login system, community system, advertising, sharing and other functions to provide a solid foundation for developers to log in to the Microsoft ecosystem.
    
*   MG SDK is developed based on UWP, reserved C++, C#, JS and other languages of the functional interface, so that developers can use the MG SDK in a variety of development environments. For applications developed on different engines and different terminals, we provide developers with corresponding solution to have Android, iOS, H5, Web, Win 32 and other applications quickly ported Win 10 UWP version, and provide them with free technical support services.
    
*   With years of Windows 10 UWP application development technology precipitation and experience accumulation, various functions in MG SDK have become efficient and stable. In the docking process, the whole process of exclusively docking MG SDK has been summarized, which makes it more convenient for cooperative developers to use the MG SDK in applications or games, and easily publish it to the Microsoft Store.
    
*   MG has brought together a large number of technical experts to create a UWP developer community, providing free technical support services and technical training for UWP developers, so that more developers can join the Microsoft ecosystem and contribute to the construction of the Microsoft ecosystem.
    
*   Visit [Miracle Games developer Platform](https://doc.mguwp.net/en/Introduction.html) for more content.
    

## Announcement

This program is an MG SDK functional demonstration which is only used for learning exchanges and is prohibited for commercial use.

## MG SDK existing features:

1.  MG account login function
    
2.  Open-screen ads, banner ads, interstitial ads, full-screen interstitial ads, and back-screen ads
    
3.  Microsoft Payment function
    
4.  Community feature
    
5.  Microsoft comment function  
    

## How to run the program?

### Environmental Requirements:

*   Win10 development system is required
    
*   Visual Studio 2015 and later version
    

### Docking with MG SDK:

[Download](https://doc.mguwp.net/en/c_sdk_update.html) the SDK file and copy it to the sample project. After configuring the development environment in Visual Studio, you can operate on the sample project. This article demonstrates how to integrate different features of the MG SDK into a C++project.

#### 1. Environmental preparation

1.  **Download SDK package**  
    Obtain the latest version of MG SDK development kit and demo project from the [official release page](https://doc.mguwp.net/en/c_sdk_update.html).
    
2.  **Engineering structure configuration**  
    Extract the SDK to the root directory of the sample project. The recommended directory structure is as follows: 
    

```
MyProject/
├── MiracleGamesSDK/            # for SDK DLL
│   ├── resource/               # Images and other materials
│   └── MiracleGamesWin32.dll   # sdk dll
├── src/                        # Demo project source code
│   └── MainTest.cpp            # demo
└── CppTestProject1.sln         # Visual Studio Solution
```

#### 2. Visual Studio  Configuration

1.  **Property Pages**
    
    *   **Build Events → Post-Build Event → Command Line**  
        Add：`xcopy /y "$(ProjectDir)MiracleGamesSDK\$(PlatformTarget)\*.dll" "$(OutDir)"
        
		xcopy /y "$(ProjectDir)MiracleGamesSDK" "$(OutDir)"

		xcopy /yei "$(ProjectDir)MiracleGamesSDK\resource" "$(OutDir)resource"

		xcopy /yei "$(ProjectDir)MiracleGamesSDK\runtimes" "$(OutDir)runtimes"`
        
3.  Architecture matching  
    Ensure that the SDK is consistent with the engineering platform toolset (such as x64/x86).
    

![image.png](https://alidocs.oss-cn-zhangjiakou.aliyuncs.com/res/4j6OJ5w27korDq3p/img/7b158cb9-202d-4275-9b86-2fa259cf0a98.png)

## MGSDK Function Call Code Example（C++）

*   [MG SDK Access and Function Demonstration](https://upload.mguwp.net/doc/en/video/CppSDK_all.mp4)
    

### Initialization

Before accessing other functions of MG SDK, initialize the SDK is needed at first. After the initialization is completed, all functions in the SDK can be accessed in the background system. In general, the SDK initialization interface is called after entering the program. The parameter used in the initialization interface is “APPKEY”, which needs to be created in the MG Developer Management Center system and is only associated identity of the application in MG.

```plaintext
//Initialising the interface
typedef void(WINAPI *SetupAsync)(char* appkey);					
//SDK initialisation
void setupAsync(HINSTANCE hDLL) {
	SetupAsync func;

	func = (SetupAsync)GetProcAddress(hDLL, "SetupAsync");
	func("YOUR_APP_KEY");//pass in appkey
}
int main(int argc, char* argv[])
{
	hDLL = LoadLibrary("MgSDKCSharpDLL.dll");
	setupAsync(hDLL);//SDK initialisation
	FreeLibrary(hDLL);
	return 0;
}
```

### Login

When calling the login function, make sure that the SDK initialization has been successful. The login panel will automatically adapt the language according to the user’s current regional location. The user can log in and register with MG account, or use Microsoft accounts, Google accounts and other third-party accounts to log in. After the account is successfully logged in, information such as Token value and user ID is returned, which developers can use to verify the online status of the account on MG server.

```plaintext
//Login Function Interface
typedef void (WINAPI *OpenMgLogin)();
//1.Open Login
void openMgLogin(HINSTANCE hDLL) {
	OpenMgLogin func;
	func = (OpenMgLogin)GetProcAddress(hDLL, "OpenMgLogin");
	func();
}

//Callback event when login is complete, callback returns the string "0,userId=xxx,userName=xxx,userToken=xxx"
typedef void(WINAPI *LoginCompleteEvents)(OnResult onLoginComplete);
//2.Login completion callback function
void onLoginComplete(char* s) {
	printf("onLoginComplete:%s\n", s);
	if (s[0] == '1') {
		//Login Successful
		//s=1,userId=A9D5042666A3084,userName=zhangyong****,userToken=d5b1bf65-220c-45d5-9381-bc0410f01cd7
	}
}
// Register the login completion event, usually in the main method.
void RegisterLoginCompleteEvent(HINSTANCE hDLL) {
	LoginCompleteEvents func;

	func = (LoginCompleteEvents)GetProcAddress(hDLL, "LoginCompleteEvents");
	func(onLoginComplete);
}
```

### Community

After logging into your account, you can invoke the community code to activate the service and generate a left-side entry icon. Clicking this icon unfolds an integrated community interface that consolidates real-time support, reward pack redemption, official forum access, achievement showcase, customer support contact, and other functional modules into a unified community experience.

```plaintext
typedef bool (WINAPI* IsOpenMgBubble)();
// Determine whether the community access permission is enabled.
bool isOpenMgBubble(HINSTANCE hdll) {
    IsOpenMgBubble func = (IsOpenMgBubble)GetProcAddress(hdll, "IsOpenMgBubble");
    bool res = func();
    if (res)
    { 
        // Display the "MG Bubble" button.
        //...
    }
    return res;
}
// Callback event upon completion of the login process
void onLoginComplete(char* s) {
    if (s[0] == '1') {
        //1, Login successfully
        //... 
         
        isOpenMgBubble(hDLL); 
    }
}
```

### Microsoft Payment

When initiating payment function, an order number is formed in developer’s system. The order number is passed to the payment interface though comment parameter. After the payment process is completed, MG’s server will initiate a callback to the developer, which contains this comment information. The developer can mark the order number as successfully paid through this information. All order status can be checked in the MG Developer Management Center. When the payment window is closed, the order number is used to check whether the payment is successful in the developer’s system. If so, the prop can be sent to the user.

```plaintext
//Payment function interface, pass in parameters for payment, JSON format
typedef void(WINAPI *OpenMgStore)(char* data);	
//open the payment window
void openMgStore(HINSTANCE hDLL)
{
	OpenMgStore func;
	func = (OpenMgStore)GetProcAddress(hDLL, "OpenMgStore");
	func("{\"goodsKey\":\"9731E09C2568064\",\"callback\":\"6934f458-4048-11eb-85f9-f0aa818a325a\",\"comment\":\"adcd-efgserewr\",\"isOnlyMsPay\":\"1\"}");
}
```

### Advertisement

Advertising forms: full-screen ads, banner ads, interstitial ads, couplet ads, full-screen interstitial ads  
Creatives: images, videos, web pages  
Among them, banner ads, interstitial ads, couplet ads, and full-screen interstitial ads have special settings, and the corresponding parameters are set to meet different presentation needs.  

```plaintext
//1.ShowFullScreenAd
//Register Callback event to close the advert
typedef void(WINAPI* FireCloseAdvertEvents)(OnResult onFireClose); 
//callback returns string {"advertplaceKey":"E4F8E422D3","AdvertKey":"xx","AdvertResourceId":"xx"}

void onFireCloseAdverts(char* s) {
	printf("onFireCloseAdverts:%s\n", s); 
	//...
	//Destroy client-created containers here
}
void registerFireCloseAdvertEvents(HINSTANCE hDLL) {
	FireCloseAdvertEvents func = (FireCloseAdvertEvents)GetProcAddress(hDLL, "FireCloseAdvertEvents");
	func(onFireCloseAdverts);//Pass in the callback function
}

//Load MG adverts after SDK initialisation is complete
typedef int(WINAPI* OpenMGAdvert)(HWND parentPtr, char* advertplaceKey); 
void openMGAdvert(HINSTANCE hDLL) {
	OpenMGAdvert func = (OpenMGAdvert)GetProcAddress(hDLL, "OpenAdvert");
	int result = func(parentPtr,"{\"advertplaceKey\":\"E4F8E422D3\",\"parentWidth\":1600,\"parentHeight\":900,\"appType\":1}");
	//parentPtr: you can pass the handle of the whole form, or the handle of the container that spreads over the full screen
	//parentWidth: width of the game window
	//parentHeight: game window height
	//result: 1:Normal; -1:Ads were closed by the background;
}

//2.ShowInterstitialAd
void openMGAdvert(HINSTANCE hDLL) { 
	OpenMGAdvert func = (OpenMGAdvert)GetProcAddress(hDLL, "OpenAdvert");
	int result = func(parentPtr, "{\"advertplaceKey\":\"C6E76462AF\",\"width\":640,\"height\":640,\"appType\":1}");
	//width: the width of the container, can not pass. Default value is 640.
	//height: width of the container, can not be passed. Default value is 640.
}

//3.ShowFullScreenInterstitialAd
void openMGAdvert(HINSTANCE hDLL) {
	OpenMGAdvert func = (OpenMGAdvert)GetProcAddress(hDLL, "OpenAdvert");
	int result = func(parentPtr, "{\"advertplaceKey\":\"72F76D95C0\",\"parentWidth\":1600,\"parentHeight\":900,\"appType\":1}");
	//parentPtr: you can pass the handle of the whole form, or the handle of the container that spreads over the full screen
	//parentWidth: width of the game window
	//parentHeight: game window height
}
		
//4.ShowBannerAd
void openMGAdvert(HINSTANCE hDLL) { 
	OpenMGAdvert func = (OpenMGAdvert)GetProcAddress(hDLL, "OpenAdvert");
	int result = func(parentPtr, "{\"advertplaceKey\":\"0B2D9717A1\",\"appType\":1}");
}

//5.ShowCoupletAd
void openMGAdvert(HINSTANCE hDLL) { 
	OpenMGAdvert func = (OpenMGAdvert)GetProcAddress(hDLL, "OpenAdvert");
	int result = func(parentPtr, "{\"advertplaceKey\":\"3427B3ED71\",\"handle2\":234567,\"appType\":1}");
	//handle2: right container handle converted to int type
}
```

### Rating

After accessing MG SDK, you can implement the Microsoft Rating function by calling the interface provided in the MG SDK and obtain the return information of the rating operation in the callback interface of the method.

```plaintext
//Praise Function Interface
typedef void (WINAPI *ShowRatingReview)();
//open the positive feedback window
void showRatingReview(HINSTANCE hDLL) {
	ShowRatingReview func;
	func = (ShowRatingReview)GetProcAddress(hDLL, "ShowRatingReview");
	func();
}

//Praise Functional Interface Callback Events
typedef void(WINAPI *ShowRatingReviewEvents)(OnResult onShowRatingReview);
//Praise function callback function
void onShowRatingReview(char* s) {
	printf("onShowRatingReview:%s\n", s);
	// s:
	// 1, The user has completed the comment
	// 0, User has not commented
}
//Register the comment callback event, generally put into the main method to register the
void RegisterShowRatingReviewEvent(HINSTANCE hDLL) {
	ShowRatingReviewEvents func;

	func = (ShowRatingReviewEvents)GetProcAddress(hDLL, "ShowRatingReviewEvents");
	func(onShowRatingReview);
}	
```
