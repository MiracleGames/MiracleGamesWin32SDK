#pragma once

#include "resource.h"

// 全局 DLL 句柄
//extern HINSTANCE hDLL;

// 回调函数类型定义
typedef void(*CallbackDelegate)(char* s);
typedef void(*PayWindowCloseDelegate)();

// DLL 函数类型定义
typedef char* (WINAPI* GetStr)(const char* parama, const char* paramb);
typedef void(WINAPI* OpenMgLog)(bool isError);//dll Output Log,Pass in isError,True is on

typedef bool(WINAPI* IsOpenCmp)();
typedef int(WINAPI* OpenCmp)(const char* jsonParam);
typedef void(WINAPI* CmpSizeChangedEvent)(CallbackDelegate onCmpSizeChanged);
typedef void(WINAPI* CmpClosedEvent)(CallbackDelegate onCmpClosed);

typedef void(WINAPI* SetupAsync)(const char* appkey);// dll Initialize the function, passing in the AppKey
typedef void(WINAPI* InitCompleteEvent)(CallbackDelegate onInitComplete);
typedef void(WINAPI* PushUriEvent)(CallbackDelegate onPushUri); // Uri required by Microsoft push
typedef void(WINAPI* AntiAddictionCloseGameEvent)(CallbackDelegate onAntiAddictionCloseGame);

typedef void(WINAPI* OpenMgLogin)();// dll Login function
typedef void(WINAPI* LoginCompleteEvent)(CallbackDelegate onLoginCompleted); // Login complete callback, returns "0,userId=xxx,userName=xxx,userToken=xxx"
typedef bool(WINAPI* IsOpenMgBubble)();
typedef void(WINAPI* OpenMgBubble)();

typedef void(WINAPI* OpenMgStore)(const char* data);
typedef void(WINAPI* PayWindowCloseEvent)(PayWindowCloseDelegate onPayWindowClosed); // Callback when the payment list interface and code scanning payment interface are closed
typedef void(WINAPI* AssetChangedJsonEvent)(CallbackDelegate onAssetsChanged); // Client: Asset change callback
typedef void(WINAPI* PaymentCompleteEvent)(CallbackDelegate onPaymentCompleted); // Client: Payment complete callback
typedef bool(WINAPI* ReportFulfillment)(const char* orderId);

typedef void(WINAPI* IsRatingReview)(); // Judge whether the user has commented on the application (if the number of comments is greater than 1, it is already commented)
typedef void(WINAPI* IsRatingReviewEvents)(CallbackDelegate onIsRatingReview); // Event callback judge whether the user has commented on the application
typedef void(WINAPI* ShowRatingReview)(); // Call up Microsoft comment function
typedef void(WINAPI* ShowRatingReviewEvents)(CallbackDelegate onShowRatingReview); // Event callback after Microsoft comment

typedef void(WINAPI* LoadMgExitAdvert)(const char* advertplaceKey); // Load the back screen advertisement and pass in the advertisement primary key value created in the Mg background
typedef void(WINAPI* OpenExitAdvert)(); // Enable mg back screen advertising function
typedef int(WINAPI* OpenAdvert)(const char* jsonParam);
typedef void(WINAPI* FireCloseAdvertEvents)(CallbackDelegate onFireCloseAdvert);
typedef void(WINAPI* ReportMgRewardFulfillment)(const char* rewardId);

typedef void(WINAPI* PreloadAdvert)(const char* jsonParam);
typedef int(WINAPI* ShowAdvert)(const char* jsonParam);
typedef void(WINAPI* PreloadAdvertEvents)(CallbackDelegate onPreloadAdvertEvents);

// SDK 操作函数声明  
bool isOpenCmp(HINSTANCE hdll);
void openCmp(HINSTANCE hdll, const char* jsonParam);
void setupAsync(HINSTANCE hdll);
void loadMgExitAdvert(HINSTANCE hdll);
void openExitAdvert(HINSTANCE hdll);
void openMGAdvert(const char* s);
void reportMgRewardFulfillment(const char* rewardId);
void openMgLogin();
void openMgStore(int isOnlyMsPay);
void reportFulfillment(const char* s);
void isRatingReview(HINSTANCE hdll);
void showRatingReview(HINSTANCE hdll);
void preloadAdvert(const char* s);
int showAdvert(const char* s);


// 回调函数声明 
void onCmpSizeChangedEvent(char* s);
void onCmpClosedEvent(char* s);
void onInitCompleteEvent(char* s);
void onLoginCompleteEvent(char* s);
void onFireCloseAdvertEvent(char* s);
void onPreloadAdvertEvent(char* s);
void onAssetsChangedEvent(char* s);
void onPaymentCompleteEvent(char* s);
void onIsRatingReviewEvent(char* s);
void onShowRatingReviewEvent(char* s);
void onPushUriEvent(char* s);
void onAntiAddictionCloseGameEvent(char* s);

// 定义所使用控件 ID 
#define ID_TXT_LOG               101
#define ID_BTN_LOGIN             210
#define ID_BTN_MGPAY             310
#define ID_BTN_MSPAY             311
#define ID_BTN_RATING            410 //微软好评 

//1.插屏 2.横幅 3.对联 4.全屏插播 5.开屏 6.退屏 7.激励视频
#define ID_BTN_OPENAD            550 //开屏 
#define ID_BTN_EXITAD            560 //退屏
#define ID_BTN_AD1               510 //插屏
#define ID_BTN_AD2               520 //横幅
#define ID_BTN_AD3               530 //对联
#define ID_BTN_AD4               540 //全屏插播 
#define ID_BTN_AD7               570 //激励视频 
#define ID_BTN_ADPRELOAD10       580 //插屏预缓存
#define ID_BTN_ADPRELOAD11       581 //插屏预缓存展示

#define ID_BTN_CLOSECMP          610 //关闭CMP 