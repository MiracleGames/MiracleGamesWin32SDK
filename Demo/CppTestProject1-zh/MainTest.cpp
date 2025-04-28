#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <windows.h>
#include <list>
#include <string>
using namespace std;
static HINSTANCE hDLL;
typedef void(*OnResult)(char* s);// �ص���s�ǽ���ַ���
typedef void(*OnPayWindowClose)();

typedef char* (WINAPI *GetStr)(char* s); // dll Test return string��return "hello "+s
typedef void (WINAPI *OpenMgLogin)();// dll��¼����
typedef void(WINAPI *SetupAsync)(char* appkey);// dll��ʼ������,����appkey
typedef void(WINAPI *OpenMgStore)(char* data); // dll֧��������֧���Ĳ�����JSON��ʽ
typedef void(WINAPI *OpenMgLog)(bool isError);// dll�����־,����isError,trueΪ����
typedef void(WINAPI *InitCompleteEvents)(OnResult onInitComplete); // ��ʼ���ص��������ص������ַ���"1,token=Token",��"0"
typedef void(WINAPI *LoginCompleteEvents)(OnResult onLoginComplete); // ��¼��ɻص����ص������ַ���"0,userId=xxx,userName=xxx,userToken=xxx"
typedef void(WINAPI *PayWindowCloseEvent)(OnPayWindowClose onPayWindowClose); // ֧���б���桢ɨ�븶����棬��΢��֧������ر�ʱ�Ļص�
typedef void(WINAPI *AssetChangedJsonEvent)(OnResult onAssetsChange); // �ͻ���:�ʲ�����ص�
typedef void(WINAPI *PushUriEvent)(OnResult onPushUri); // ΢����������uri

typedef void (WINAPI *OpenExitAdvert)(); // ����MG ������湦��
typedef void(WINAPI *LoadMgExitAdvert)(char* advertplaceKey); // ����������棬������mg��̨������õ��Ĺ������keyֵ

typedef void (WINAPI *ShowRatingReview)(); // ����΢�����۹��� 
typedef void(WINAPI *ShowRatingReviewEvents)(OnResult onShowRatingReview); // ΢�����ۺ��¼��ص�
typedef void (WINAPI *IsRatingReview)(); // �ж��û��Ƿ����۹�Ӧ�ã����۴�������1��Ϊ�Ѿ����۹���
typedef void(WINAPI *IsRatingReviewEvents)(OnResult onIsRatingReview);  // �Ƿ����۹��ص�
typedef void(WINAPI *AntiAddictionCloseGameEvents)(OnResult onAntiAddictionCloseGame);  //��������֤ʧ�ܺ�Ĺر���Ϸ�¼�
//// sdk
void openMgLogin(HINSTANCE hDLL);// ��¼
void openMgStore(HINSTANCE hDLL);// ֧��
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
	//example��{\"Assets\":[{\"Id\":\"A5A59D432020211201123313\",\"Comment\":\"badun123456789\",\"DigitalGoods\":{\"Name\":\"test1\",\"Tag\":\"\",\"Count\":1}}],\"EventType\":1}
}

// ��ʼ�����
void onInitComplete(char* s) {
	printf("onInitComplete:%s\n", s);
	if (s[0] == '1') {
		openMgLogin(hDLL);// ��ʼ���ɹ����ٵ��õ�¼
		loadMgExitAdvert(hDLL); // ��ʼ�����֮�����������������
	}
}

// ��¼���
void onLoginComplete(char* s) {
	printf("onLoginComplete:%s\n", s);
	if (s[0] == '1') {
		// successful login
		openMgStore(hDLL);
		//isRatingReview(hDLL);
	}
}

//������ʧ�ܺ�Ļص�����
void onAntiAddictionCloseGame(char* s) {
	printf("onAntiAddictionCloseGame:%s\n", s);
	//���οͻ���ʵ�ֹر���Ϸ�߼�
	//...
}

void onPushUri(char* s) {
	printf("onPushUri:%s\n", s);
}

// ��ʼ��
void setupAsync(HINSTANCE hDLL) {
	SetupAsync func;

	func = (SetupAsync)GetProcAddress(hDLL, "SetupAsync");
	func("C1F56BC38A");// ����appkey
}

// ��¼
void openMgLogin(HINSTANCE hDLL) {
	OpenMgLogin func;
	func = (OpenMgLogin)GetProcAddress(hDLL, "OpenMgLogin");
	//ͨ������´˲�����0
	func();
}

// ֧��
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

// ע���¼����¼�
void RegisterLoginCompleteEvent(HINSTANCE hDLL) {
	LoginCompleteEvents func;

	func = (LoginCompleteEvents)GetProcAddress(hDLL, "LoginCompleteEvents");
	func(onLoginComplete);// Pass in the callback function
}
// ע���������֤ʧ�ܺ�ر���Ϸ�¼�
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


	FreeLibrary(hDLL);// uninstall the MgSDKCSharpDLL.dll file��
	system("pause");
	return 0;
}
