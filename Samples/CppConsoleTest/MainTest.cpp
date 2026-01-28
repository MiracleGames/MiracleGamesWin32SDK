#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <windows.h>
#include <list>
#include <string>
using namespace std;
static HINSTANCE hDLL;
typedef void(*OnResult)(char* s);// 回调，s是结果字符串
typedef void(*OnPayWindowClose)();

typedef char* (WINAPI *GetStr)(char* s); // dll Test return string，return "hello "+s
typedef void (WINAPI *OpenMgLogin)();// dll登录函数
typedef void(WINAPI *SetupAsync)(char* appkey);// dll初始化函数,传入appkey
typedef void(WINAPI *OpenMgStore)(char* data); // dll支付，传入支付的参数，JSON格式
typedef void(WINAPI *OpenMgLog)(bool isError);// dll输出日志,传入isError,true为开启
typedef void(WINAPI *InitCompleteEvents)(OnResult onInitComplete); // 初始化回调函数，回调返回字符串"1,token=Token",或"0"
typedef void(WINAPI *LoginCompleteEvents)(OnResult onLoginComplete); // 登录完成回调，回调返回字符串"0,userId=xxx,userName=xxx,userToken=xxx"
typedef void(WINAPI *PayWindowCloseEvent)(OnPayWindowClose onPayWindowClose); // 支付列表界面、扫码付款界面，或微软支付界面关闭时的回调
typedef void(WINAPI *AssetChangedJsonEvent)(OnResult onAssetsChange); // 客户端:资产变更回调
typedef void(WINAPI *PushUriEvent)(OnResult onPushUri); // 微软推送所需uri

typedef void (WINAPI *OpenExitAdvert)(); // 开启MG 退屏广告功能
typedef void(WINAPI *LoadMgExitAdvert)(char* advertplaceKey); // 加载退屏广告，传入在mg后台创建后得到的广告主键key值

typedef void (WINAPI *ShowRatingReview)(); // 调起微软评论功能 
typedef void(WINAPI *ShowRatingReviewEvents)(OnResult onShowRatingReview); // 微软评论后事件回调
typedef void (WINAPI *IsRatingReview)(); // 判断用户是否评论过应用（评论次数大于1即为已经评论过）
typedef void(WINAPI *IsRatingReviewEvents)(OnResult onIsRatingReview);  // 是否评论过回调
typedef void(WINAPI *AntiAddictionCloseGameEvents)(OnResult onAntiAddictionCloseGame);  //防沉迷认证失败后的关闭游戏事件
//// sdk
void openMgLogin(HINSTANCE hDLL);// 登录
void openMgStore(HINSTANCE hDLL);// 支付
void showRatingReview(HINSTANCE hDLL);// ShowRatingReview
void isRatingReview(HINSTANCE hDLL);// isRatingReview

void openExitAdvert(HINSTANCE hDLL);// ExitAdvert


//// callback
void onShowRatingReview(char* s) {
	printf("onShowRatingReview:%s\n", s);
	openMgStore(hDLL);
}

void onIsRatingReview(char* s) {
	printf("onIsRatingReview:%s\n", s);
	// s:
	// 1,User has commented
	// 0,The user hasn't commented yet
	if (s[0] == '0')
	{
		showRatingReview(hDLL);
	}
}

// openExitAdvert
void openExitAdvert(HINSTANCE hDLL) {
  OpenExitAdvert func;
  func = (OpenExitAdvert)GetProcAddress(hDLL, "OpenExitAdvert");
  func();
}

// LoadMgExitAdvert
void loadMgExitAdvert(HINSTANCE hDLL) {
  LoadMgExitAdvert func;

  func = (LoadMgExitAdvert)GetProcAddress(hDLL, "LoadMgExitAdvert");
  func("9E4195A126");// Pass in advertplaceKey
}

// PaymentWindowClose callback function
void onPayWindowClose() {
	printf("Payment window has been colsed.");
	openExitAdvert(hDLL);
}

// AssetsChange Callback Function
void onAssetsChange(char* s) {
	printf("onAsset:%s\n", s);
	//example：{\"Assets\":[{\"Id\":\"A5A59D432020211201123313\",\"Comment\":\"badun123456789\",\"DigitalGoods\":{\"Name\":\"test1\",\"Tag\":\"\",\"Count\":1}}],\"EventType\":1}
}

// 初始化完成
void onInitComplete(char* s) {
	printf("onInitComplete:%s\n", s);
	if (s[0] == '1') {
		openMgLogin(hDLL);// 初始化成功后再调用登录
		loadMgExitAdvert(hDLL); // 初始化完成之后加载退屏广告的内容
	}
}

// 登录完成
void onLoginComplete(char* s) {
	printf("onLoginComplete:%s\n", s);
	if (s[0] == '1') {
		// successful login
		openMgStore(hDLL);
		//isRatingReview(hDLL);
	}
}

//防沉迷失败后的回调函数
void onAntiAddictionCloseGame(char* s) {
	printf("onAntiAddictionCloseGame:%s\n", s);
	//由游客户端实现关闭游戏逻辑
	//...
}

void onPushUri(char* s) {
	printf("onPushUri:%s\n", s);
}

// 初始化
void setupAsync(HINSTANCE hDLL) {
	SetupAsync func;

	func = (SetupAsync)GetProcAddress(hDLL, "SetupAsync");
	func("C1F56BC38A");// 传入appkey
}

// 登录
void openMgLogin(HINSTANCE hDLL) {
	OpenMgLogin func;
	func = (OpenMgLogin)GetProcAddress(hDLL, "OpenMgLogin");
	//通常情况下此参数传0
	func();
}

// 支付
void openMgStore(HINSTANCE hDLL)
{
	OpenMgStore func;
	func = (OpenMgStore)GetProcAddress(hDLL, "OpenMgStore");
	func("{\"goodsKey\":\"9731E09C2568064\",\"callback\":\"6934f458-4048-11eb-85f9-f0aa818a325a\",\"comment\":\"adcd-efgserewr\",\"isOnlyMsPay\":\"0\"}");
}

// Output Log
void openMgLog(HINSTANCE hDLL) {
	OpenMgLog func;

	func = (OpenMgLog)GetProcAddress(hDLL, "OpenMgLog");
	func(true);//Pass in true
}


void showRatingReview(HINSTANCE hDLL) {
	ShowRatingReview func;
	func = (ShowRatingReview)GetProcAddress(hDLL, "ShowRatingReview");
	func();
}

void isRatingReview(HINSTANCE hDLL) {
	IsRatingReview func;
	func = (IsRatingReview)GetProcAddress(hDLL, "IsRatingReview");
	func();
}


void RegisterShowRatingReviewEvent(HINSTANCE hDLL) {
	ShowRatingReviewEvents func;

	func = (ShowRatingReviewEvents)GetProcAddress(hDLL, "ShowRatingReviewEvents");
	func(onShowRatingReview);
}

void RegisterIsRatingReviewEvent(HINSTANCE hDLL) {
	IsRatingReviewEvents func;

	func = (IsRatingReviewEvents)GetProcAddress(hDLL, "IsRatingReviewEvents");
	func(onIsRatingReview);
}


void RegisterInitCompleteEvent(HINSTANCE hDLL) {
	InitCompleteEvents func;

	func = (InitCompleteEvents)GetProcAddress(hDLL, "InitCompleteEvents");
	func(onInitComplete);// Pass in the callback function
}

// 注册登录完成事件
void RegisterLoginCompleteEvent(HINSTANCE hDLL) {
	LoginCompleteEvents func;

	func = (LoginCompleteEvents)GetProcAddress(hDLL, "LoginCompleteEvents");
	func(onLoginComplete);// Pass in the callback function
}
// 注册防沉迷认证失败后关闭游戏事件
void RegisterAntiAddictionCloseGameEvent(HINSTANCE hDLL) {
	AntiAddictionCloseGameEvents func; 
	func = (AntiAddictionCloseGameEvents)GetProcAddress(hDLL, "AntiAddictionCloseGameEvent");
	func(onAntiAddictionCloseGame);// Pass in the callback function
}
// PaymentWindowClose
void RegisterPayWindowCloseEvent(HINSTANCE hDLL) {
	PayWindowCloseEvent func;

	func = (PayWindowCloseEvent)GetProcAddress(hDLL, "PayWindowCloseEvent");
	func(onPayWindowClose);
}

// AssetsChange
void RegisterAssetsChangeEvent(HINSTANCE hDLL) {
	AssetChangedJsonEvent func;

	func = (AssetChangedJsonEvent)GetProcAddress(hDLL, "AssetChangedJsonEvent");
	func(onAssetsChange);
}

void RegisterPushUri(HINSTANCE hDLL) {
	PushUriEvent func;

	func = (PushUriEvent)GetProcAddress(hDLL, "PushUriEvent");
	func(onPushUri);
}

void getstr(HINSTANCE hDLL)
{
	GetStr func;
	func = (GetStr)GetProcAddress(hDLL, "GetStr");
	printf(func("test"));
}

int main(int argc, char* argv[])
{
	for (int i = 0; i < argc; i++)
		cout << argv[i] << endl;

	hDLL = LoadLibrary("MgSDKCSharpDLL.dll");

	RegisterPayWindowCloseEvent(hDLL);
	RegisterInitCompleteEvent(hDLL);
	RegisterLoginCompleteEvent(hDLL);
	//RegisterAssetsChangeEvent(hDLL);
	//RegisterPushUri(hDLL);
	RegisterShowRatingReviewEvent(hDLL);
	RegisterIsRatingReviewEvent(hDLL);

	openMgLog(hDLL);
	setupAsync(hDLL);


	FreeLibrary(hDLL);// uninstall the MgSDKCSharpDLL.dll file；
	system("pause");
	return 0;
}
