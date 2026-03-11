using MiracleGamesWin32;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml.Linq;

namespace CSharpApp
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();

            ApplicationManager.InitCompleteEvents += InitComplete;//初始化回调
            ApplicationManager.LoginCompleteEvents += LoginComplete;
            EventManager.Instance.AssetsChanged += PaymentManager_AssetsChanged; // 资产变更，客户端回调
            ApplicationManager.RatingEvents += ApplicationManager_RatingEvents; //好评回调
            AdvertManager.ClickAdvertEvents += AdvertManager_ClickAdvertEvents;//广告点击事件
            AdvertManager.CloseAdvertEvents += AdvertManager_CloseAdvertEvents;//广告关闭事件
            ApplicationManager.PushUriEvents += ApplicationManager_PushUriEvents;//推送回调事件 
            //ApplicationManager.OpenMgLog(false);//关闭SDK日志，默认是开启状态
        }

        private const string YourAppKey = "8647026706";
        private const string MgGoodsKey = "952D6F5C23C9065";                        //MG商品标识
        private const string MgCallbackId = "14df40e9-f566-11ed-8b41-263ab2cdee42"; //支付回调Id
        private const string SplashAdKey = "E4F8E422D3";                            //开屏:1920 x 1080
        private const string ExitAdKey = "6A1FD1D202";                              //退屏:1920 x 1080
        private const string InterstitialAdKey = "C6E76462AF";                      //插屏:640 x 640
        private const string FullScreenInterstitialAdKey = "72F76D95C0";            //全屏插播:768 x 432 
        private const string BannerAdKey = "0B2D9717A1";                            //横幅:728 x 90 
        private const string CoupletAdKey = "3427B3ED71";                           //对联:300 x 600
        private const string RewardedAdKey = "B6B030D76C";                          //激励广告:768 x 432 

        #region//回调
        /// <summary>
        /// 初始化回调
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        private void InitComplete(object sender, MiracleGamesWin32.Result.InitResult args)
        {
            if (args.IsCompleted)
            {
                ShowMessage("初始化完成，Token=" + args.Token);
                //this.Invoke(new Action(() =>
                //{
                //    AdvertManager.OpenAdvert(this, SplashAdKey); //开屏广告
                //}));
            }
        }
        /// <summary>
        /// 登录完成之后回调
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        private void LoginComplete(object sender, MiracleGamesWin32.Result.LoginResult args)
        {
            if (ApplicationManager.IsOpenBubble())
            {
                ApplicationManager.ShowBubble(this);
            }

            ShowMessage("登录结果：" + args.ToJsonString());
        }
        /// <summary>
        /// 支付回调,这里是演示。支付结果以游戏服务器端通知为准
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void PaymentManager_AssetsChanged(object sender, MiracleGamesWin32.Interface.AssetsChangedEventArgs e)
        {
            this.Invoke(new Action(() =>
            {
                foreach (var asset in e.Assets)
                {
                    JObject json = new JObject();
                    json.Add("result", "success");
                    json.Add("comment", asset.Comment);
                    json.Add("orderKey", asset.Id);
                    json.Add("price", asset.DigitalGoods.Price);
                    json.Add("eventType", e.EventType);
                    json.Add("goodsNum", asset.DigitalGoods.Count.ToString());
                    json.Add("goodsName", asset.DigitalGoods.Name);
                    json.Add("goodsTag", asset.DigitalGoods.Tag);

                    ShowMessage("支付结果：" + json.ToString());

                    //... 游戏下放道具

                    PaymentManager.ReportFulfillmentAsync(asset.Id);//游戏下放道具之后，向MG核销资产。否则每次初始化都会在客户端提示未核销的订单信息。
                }
            }));
        }
        /// <summary>
        /// 微软好评
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ApplicationManager_RatingEvents(object sender, string e)
        {
            if (e != null)
            {
                ShowMessage("好评结果：" + e);
            }
        }
        //推送Uri回调事件 
        private void ApplicationManager_PushUriEvents(object sender, MiracleGamesWin32.Result.PushUriResult e)
        {
            string pushUri = e.UriResult;
            //...
        }
        /// <summary>
        /// 广告点击事件
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        /// <exception cref="NotImplementedException"></exception>
        private void AdvertManager_ClickAdvertEvents(object sender, string e)
        {
            if (e != null)
            {
                ShowMessage("广告位被点击：" + e);
            }
        }
        /// <summary>
        /// 广告关闭事件
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        /// <exception cref="NotImplementedException"></exception>
        private void AdvertManager_CloseAdvertEvents(object sender, string e)
        {
            if (e != null)
            {
                JObject jsonObj = JObject.Parse(e);
                string advertplaceKey = jsonObj.SelectToken("advertplaceKey").ToString();


                if (advertplaceKey == RewardedAdKey)//激励视频广告
                {
                    ShowMessage("激励视频广告位被关闭：" + e);
                    if (jsonObj.SelectToken("CompleteStatus").ToString() == "1")
                    {
                        string comment = jsonObj.SelectToken("Comment").ToString();
                        string rewardId = jsonObj.SelectToken("RewardId").ToString();

                        //...游戏内发奖励

                        AdvertManager.RewardAdvertUse(rewardId);//向MG核销
                    }
                }
                else
                {
                    ShowMessage("广告位被关闭：" + e);
                }

                /*广告回调参数
                 {"advertplaceKey":"C6E76462AF","AdvertKey":"","AdvertResourceId":"0","AdvertStatus":3}
                
                *激励视频广告的回调参数
                1.广告被关闭             {"advertplaceKey":"B62E9C3961","AdvertKey":"","AdvertResourceId":"0","AdvertStatus":3,"CompleteStatus":0,"Comment":"abc%2c123%2c%e8%ae%a2%e5%8d%95%e5%8f%b7%2c%e9%87%91%e9%a2%9d","RewardId":""}
                2.广告未播放完毕即被关闭 {"advertplaceKey":"B62E9C3961","AdvertKey":"09CA3680D7","AdvertResourceId":"14221","AdvertStatus":1,"CompleteStatus":0,"Comment":"abc%2c123%2c%e8%ae%a2%e5%8d%95%e5%8f%b7%2c%e9%87%91%e9%a2%9d","RewardId":"6303E1E4DEB24D120250922150213339"}
                3.广告播放完毕           {"advertplaceKey":"B62E9C3961","AdvertKey":"09CA3680D7","AdvertResourceId":"14219","AdvertStatus":1,"CompleteStatus":1,"Comment":"abc%2c123%2c%e8%ae%a2%e5%8d%95%e5%8f%b7%2c%e9%87%91%e9%a2%9d","RewardId":"5CC688A8D92B43F20250922150006060"}
                */
            }
        }
        #endregion

        private void Form1_Load(object sender, EventArgs e)
        {
            try
            {
                //1.调用CMP
                ApplicationManager.OpenCmp(this, YourAppKey);
                //2.初始化SDK
                ApplicationManager.SetupAsync(YourAppKey);

                //退屏广告需要两步：1.此处加载退屏广告ID  2.Form1_FormClosed事件中调用退屏广告
                AdvertManager.ExitAdvertKey = ExitAdKey;
            }
            catch (Exception)
            {
            }
        }

        private void ShowMessage(string message)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new Action(() => ShowMessage(message)));
            }
            else
            {
                StringBuilder sb = new StringBuilder(textBox1.Text);
                sb.Append("\r\n" + message);
                textBox1.Text = sb.ToString();
            }
        }

        //登录
        private void btnLogin_Click(object sender, EventArgs e)
        {
            try
            {
                foreach (Control control in this.Controls.OfType<MiracleGamesWin32.UI.ucBubble1>().ToList())
                {
                    this.Controls.Remove(control);
                }
            }
            catch { }
            ApplicationManager.OpenMgLogin();
        }

        //MG支付
        private void btnMgPay_Click(object sender, EventArgs e)
        {
            try
            {
                dynamic jsonObj = new
                {
                    goodsKey = MgGoodsKey,
                    callback = MgCallbackId,
                    comment = "Win32%3Aa%3D1%7Cb%3D2%24c%3D3",//透传参数，开发者需进行urlEncode
                    isOnlyMsPay = 0 //1:只使用微软支付；0:MG支付
                };
                string json = JsonConvert.SerializeObject(jsonObj);
                ApplicationManager.OpenMgStore(json);
            }
            catch (Exception)
            {
            }
        }

        //微软支付
        private void btnMsPay_Click(object sender, EventArgs e)
        {
            try
            {
                dynamic jsonObj = new
                {
                    goodsKey = MgGoodsKey,
                    callback = MgCallbackId,
                    comment = "Win32%3Aa%3D1%7Cb%3D2%24c%3D3",//透传参数，开发者需进行urlEncode
                    isOnlyMsPay = 1 //1:只使用微软支付；0:MG支付
                };
                string json = JsonConvert.SerializeObject(jsonObj);
                ApplicationManager.OpenMgStore(json);
            }
            catch (Exception)
            {
            }
        }

        //插屏广告  
        private void btnAd_Click(object sender, EventArgs e)
        {
            try
            {
                dynamic jsonObj = new
                {
                    advertplaceKey = InterstitialAdKey,
                    isOnlyPic = true,//可设置仅显示图片广告
                    width = panelAd.Width,
                    height = panelAd.Height,
                };
                string json = JsonConvert.SerializeObject(jsonObj);
                AdvertManager.OpenAdvert(this.panelAd, json);
            }
            catch (Exception)
            {
            }
        }

        //插屏
        private void btnAd1_Click(object sender, EventArgs e)
        {
            try
            {
                dynamic jsonObj = new
                {
                    advertplaceKey = InterstitialAdKey,
                    tag = "MGSDKADKEY_TEST1"  //开发者可以此来删掉对应的广告位
                };
                string json = JsonConvert.SerializeObject(jsonObj);
                AdvertManager.OpenAdvert(this, json);
            }
            catch (Exception)
            {
            }
        }

        //全屏插播
        private void btnAd2_Click(object sender, EventArgs e)
        {
            try
            {
                AdvertManager.OpenAdvert(this, FullScreenInterstitialAdKey);
            }
            catch (Exception)
            {
            }
        }

        //Banner
        private void btnAd3_Click(object sender, EventArgs e)
        {
            try
            {
                AdvertManager.OpenAdvert(this, BannerAdKey);
            }
            catch (Exception)
            {
            }
        }

        //对联
        private void btnAd4_Click(object sender, EventArgs e)
        {
            try
            {
                AdvertManager.OpenAdvert(this, CoupletAdKey);
            }
            catch (Exception)
            {
            }
        }

        //激励视频
        private void btnAd5_Click(object sender, EventArgs e)
        {
            try
            {
                string comment = "abc,123,订单号,金额";
                comment = Uri.EscapeDataString(comment);

                dynamic jsonObj = new
                {
                    advertplaceKey = RewardedAdKey,
                    comment = comment
                };
                string json = JsonConvert.SerializeObject(jsonObj);
                AdvertManager.OpenAdvert(this, json);
            }
            catch (Exception)
            {
            }
        }

        private void btnAdDelete_Click(object sender, EventArgs e)
        {
            for (int i = panelAd.Controls.Count; i > 0; i--)//删除指定容器内的广告
            {
                Control item = panelAd.Controls[0];
                item.Dispose();
                panelAd.Controls.Remove(item);
            }
            for (int i = this.Controls.Count; i > 0; i--)//删除页面上所有MG的广告
            {
                Control item = this.Controls[0];
                if (item.CompanyName == "MiracleGamesWin32.Controls")
                {
                    item.Dispose();
                    this.Controls.Remove(item);
                }
            }
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            //AdvertManager.openExitAdvert();
        }

        #region //预加载示例
        private async void btn_getAd1_Click(object sender, EventArgs e)
        {
            Button btnGetAd = (Button)sender;
            string adKey = btnGetAd.Tag.ToString();
            ShowMessage($"广告位[{adKey}] [{DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss")}] 开始预缓存...");

            dynamic jsonObj = new
            {
                advertplaceKey = adKey,
                left = adKey == BannerAdKey ? 100 : 0,
                Top = adKey == BannerAdKey ? 10 : 0
            };
            string json = JsonConvert.SerializeObject(jsonObj); 
            int result = await AdvertManager.PreloadAdvert(json);
            if (result == 1)
            {
                //btn_showAd1.Enabled = true;
                EnableAdBtn(adKey);
                ShowMessage($"广告位[{adKey}] [{DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss")}] 素材准备完毕");
            }
            else
            {
                ShowMessage($"广告位[{adKey}] [{DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss")}] 预加载失败，返回结果：[{result}]");
            }
        } 
        private void btn_showAd1_Click(object sender, EventArgs e)
        {
            Button btn = (Button)sender;
            string adKey = btn.Tag.ToString().Replace("SHOW_", "");
            dynamic jsonObj = new
            {
                advertplaceKey = adKey
            };
            string json = JsonConvert.SerializeObject(jsonObj);
            int result = AdvertManager.ShowAdvert(this, json);
            if (result == 1)//广告已经展示
                btn.Enabled = false;
        }

        private void EnableAdBtn(string adKey)
        { 
            foreach (var item in groupBox1.Controls)
            {
                if (item is Button button)
                {
                    if (button.Tag.ToString() == $"SHOW_{adKey}")
                    {
                        button.Enabled = true;
                        return;
                    }
                }
            }
            foreach (var item in groupBox2.Controls)
            {
                if (item is Button button)
                {
                    if (button.Tag.ToString() == $"SHOW_{adKey}")
                    {
                        button.Enabled = true;
                        break;
                    }
                }
            }
        }
         
        private const string Google_BannerAdKey = "3F17565084";                            //横幅:728 x 90
        private const string Google_InterstitialAdKey = "A62A9F2BDF";                      //插屏:640 x 640
        private const string Google_FullScreenInterstitialAdKey = "AFA16C53D4";            //全屏插播:768 x 432 
        private const string Google_CoupletAdKey = "E1C984846E";                           //对联:300 x 600
        private const string Google_RewardedAdKey = "B62E9C3961";                          //激励广告:768 x 432 
        private async void btn_gogGetAd1_Click(object sender, EventArgs e)
        { 
            Button btn = (Button)sender;
            string adKey = btn.Tag.ToString();
            ShowMessage($"广告位[{adKey}] [{DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss")}] 开始预缓存...");

            dynamic jsonObj = new
            {
                advertplaceKey = adKey
            };
            string json = JsonConvert.SerializeObject(jsonObj); 
            int result = await AdvertManager.PreloadAdvert( json);
            if (result == 1)
            {
                //btn_gogShowAd1.Enabled = true;
                EnableAdBtn(adKey);
                ShowMessage($"广告位[{adKey}] [{DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss")}] 素材准备完毕");
            }
            else
            {
                ShowMessage($"广告位[{adKey}] [{DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss")}] 预加载失败，返回结果：[{result}]");
            }
        } 
        private void btn_gogShowAd1_Click(object sender, EventArgs e)
        {
            Button btn = (Button)sender;
            string adKey = btn.Tag.ToString().Replace("SHOW_", "");
            dynamic jsonObj = new
            {
                advertplaceKey = adKey
            };
            string json = JsonConvert.SerializeObject(jsonObj);
            int result = AdvertManager.ShowAdvert(this, json);
            if (result == 1)
                btn.Enabled = false;
        } 
        #endregion
    }
}
