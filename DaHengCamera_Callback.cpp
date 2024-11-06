#include <QtWidgets/QtWidgets> //Qt界面库头文件
#include "DxImageProc.h" //包含大恒相机图像处理头文件
#include "GxIAPI.h" //包含大恒相机的控制和采集头文件
#include <iostream> //C++标准输入输出头文件

using namespace std;

GX_DEV_HANDLE hDevice = NULL;//设备号

class MyWidget:public QWidget
{
	public:
		MyWidget(QWidget* parent=nullptr);
		void setImage(QImage image){this->image=image;}
		
	private:
		QImage image;//从相机抓取的图像转换成QImage图像。
		void paintEvent(QPaintEvent *event) override; //本窗口的描画函数，自动执行。
		void closeEvent(QCloseEvent *event) override;	//主窗口关闭时处理函数 
};


MyWidget::MyWidget(QWidget *parent):QWidget(parent)
{
	this->resize(960,600);
}


//将采集转换的image图像通过paintEvent函数描画出来，比通过QLabel显示图像要清晰。
void MyWidget::paintEvent(QPaintEvent *event)
{
	if (image.isNull()) return;
	
	QPainter painter(this);
	//painter.setRenderHints(QPainter::Antialiasing,true);//消锯齿
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true); //平滑绘图
	
	double widthRatio=(double)this->width()/image.width();
	double heightRatio=(double)this->height()/image.height();
	double minRatio=qMin(widthRatio, heightRatio);
	double drawWidth=image.width()*minRatio;
	double drawHeight=image.height()*minRatio;
	double x=(this->width()-drawWidth)/2;
	double y=(this->height()-drawHeight)/2;
	
	painter.drawImage(QRectF(x, y, drawWidth, drawHeight), image); //将image描画在矩形范围内。	
}

void MyWidget::closeEvent(QCloseEvent *event)
{
	if (hDevice) GXSendCommand(hDevice, GX_COMMAND_ACQUISITION_STOP); //发送停止采集命令
	if (hDevice) GXUnregisterCaptureCallback(hDevice); //注销采集回调
	if (hDevice) GXCloseDevice(hDevice); //如果有设备打开，关闭设备
	GXCloseLib(); //在结束的时候调用 GXCLoseLib()释放资源
}

static MyWidget *form=nullptr;

//图像回调处理函数
static void GX_STDC OnFrameCallbackFun(GX_FRAME_CALLBACK_PARAM* pFrame)
{
	//cout<<"OnFrameCallbackFun excuted!"<<endl;
	
	if (pFrame->status == 0)
	{
		//cout<<"pFrame status ==0"<<endl;
		
		BYTE *pRGB24Buf = new BYTE[pFrame->nWidth * pFrame->nHeight * 3]; //输出图像 RGB 数据
		if (pRGB24Buf == NULL) return ;
		else memset(pRGB24Buf,0,pFrame->nWidth * pFrame->nHeight * 3 * sizeof(BYTE)); //缓冲区初始化

		DX_BAYER_CONVERT_TYPE cvtype = RAW2RGB_NEIGHBOUR; //选择插值算法
		DX_PIXEL_COLOR_FILTER nBayerType = BAYERGR; //选择图像 Bayer 格式
		bool bFlip = false;
		VxInt32 DxStatus = DxRaw8toRGB24((BYTE*)pFrame->pImgBuf,pRGB24Buf,pFrame->nWidth,pFrame->nHeight,cvtype,nBayerType,bFlip);
		if (DxStatus != DX_OK)
		{
			//cout<<"DxStatus is not OK!"<<endl;
			if (pRGB24Buf != NULL)
			{
				delete []pRGB24Buf;
				pRGB24Buf = NULL;
			}
			return;
		}
		
		//cout<<"DxStatus is OK!"<<endl;

		QImage image=QImage((uchar*)(pRGB24Buf), pFrame->nWidth, pFrame->nHeight, QImage::Format_BGR888);
		if (form)
		{
			form->setImage(image.copy());
			form->update();	
			//cout<<"setImage and update!"<<endl;			
		}

		delete [] pRGB24Buf;
		pRGB24Buf = NULL;
	}
	
	return;
}

void initializeCamera()
{
	//定义GX_STATUS枚举类型变量status，用于接收以下各函数执行后的结果
	GX_STATUS status = GX_STATUS_SUCCESS; 
	status = GXInitLib(); //初始化大恒相机的设备库
	if (status != GX_STATUS_SUCCESS)
	{
		cout<<"GXInitLib failed: "<<status<<endl;
	}

	uint32_t nDeviceNum = 0;
	status = GXUpdateDeviceList(&nDeviceNum, 2000); //更新设备列表，获得设备数
	cout<<"Device Number = "<<nDeviceNum<<endl;
	
	GX_OPEN_PARAM stOpenParam;
	if (status == GX_STATUS_SUCCESS&&nDeviceNum> 0)
	{
		//打开枚举列表中的第一台设备。
		//假设枚举到了3台可用设备，那么用户可设置stOpenParam参数的pszContent字段为1、2、3
		stOpenParam.accessMode = GX_ACCESS_EXCLUSIVE;
		stOpenParam.openMode = GX_OPEN_INDEX;
		stOpenParam.pszContent =(char*)"1";
		//通过序列号打开设备
		//stOpenParam.openMode = GX_OPEN_SN;
		//stOpenParam.pszContent =(char*)"KB0210030058";
		//大恒水星相机MER-500-14U3C-L型号分辨率2592*1944 pixels
		//像素深度8位，10位，帧率14fps，彩色，图像数据格式：Bayer GR8/Bayer GR10, 清晰度>1556线
		
		status = GXOpenDevice(&stOpenParam, &hDevice); //打开设备
		
		if (status==GX_STATUS_SUCCESS)
		{
			cout<<"Device has been opened successfully!"<<endl;
		
			status = GXRegisterCaptureCallback(hDevice, NULL, OnFrameCallbackFun);
			cout<<"GXRegisterCaptureCallback status="<<status<<endl;
			status = GXSendCommand(hDevice, GX_COMMAND_ACQUISITION_START);
			cout<<"Acquisition Start status="<<status<<endl;
		}
		else
		{
			cout<<"Failed in openning device!"<<endl; //如果没有成功打开设备，显示信息
		}
	}
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	
	MyWidget widget;
	widget.show();
	form=&widget;
	
	initializeCamera();

	return app.exec();
}
