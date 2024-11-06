//最新进度

#include <QtWidgets/QtWidgets> //Qt界面库头文件
#include "DxImageProc.h" //包含大恒相机图像处理头文件
#include "GxIAPI.h" //包含大恒相机的控制和采集头文件
#include <iostream> //C++标准输入输出头文件
#define cn QString::fromLocal8Bit //定义cn代表QString::fromLocal8Bit

using namespace std;

GX_DEV_HANDLE hDevice = NULL;//设备号

//定义MyMainWindow类
class MyMainWindow:public QMainWindow
{
	public:
		MyMainWindow(QWidget* parent=nullptr);
		void setImage(QImage image){this->image=image;}
		
	private:
		QMenuBar *menuBar=new QMenuBar; //定义菜单栏对象指针
		QToolBar *hToolBar = new QToolBar; //定义水平工具栏对象指针
		
		QToolButton *Start = new QToolButton; //定义一个工具栏按钮指针Start
		QToolButton *Stop = new QToolButton; //定义一个工具栏按钮指针Stop
		
		QImage image;//从相机抓取的图像转换成QImage图像。
		
		void paintEvent(QPaintEvent *event) override; //本窗口的描画函数，自动执行。
		void closeEvent(QCloseEvent *event) override;	//主窗口关闭时处理函数
		void saveFile(); //点击save时的处理函数
		void stop();//点击stop时的处理函数
		void start();//点击start时的处理函数
};

//MyMainWindow类构造函数
MyMainWindow::MyMainWindow(QWidget *parent):QMainWindow(parent)
{
	//基础设置
	this->resize(720,450);
	this->setWindowTitle(cn("ght显微图像处理软件")); //设置窗体的标题
	
	this->setMenuBar(menuBar); //将menuBar设置为本窗口的菜单栏
	this->addToolBar(Qt::TopToolBarArea, hToolBar); //添加toolBar工具栏到本窗口上部
	
	//菜单设置
	QMenu *fileMenu=new QMenu("File"); //定义菜单fileMenu
	fileMenu->addAction(QPixmap("save.png"), "Save", this, &saveFile); //添加Save菜单项到fileMenu
	menuBar->addMenu(fileMenu); //将fileMenu菜单添加到菜单栏
	
	//hToolBar工具栏设置
	
	Start->setIcon(QPixmap("start.png")); //设置按钮的图标
	Start->setEnabled(false);//禁用
	Start->setToolTip("Start"); //设置提示信息
	connect(Start, &QToolButton::clicked, this, &start); //连接信号函数和处理函数
	
	QToolButton *Stop = new QToolButton; //定义一个工具栏按钮指针Stop
	Stop->setEnabled(true);//启用
	Stop->setIcon(QPixmap("stop.png")); //设置按钮的图标
	Stop->setToolTip("Stop"); //设置提示信息
	connect(Stop, &QToolButton::clicked, this, &stop); //连接信号函数和处理函数
	
	hToolBar->setIconSize(QSize(24, 24)); //设置工具栏图标的尺寸，单位为像素
	hToolBar->addWidget(Start); //将Start添加到htoolBar
	hToolBar->addWidget(Stop); //将Stop添加到htoolBar
}

//将采集转换的image图像通过paintEvent函数描画出来，比通过QLabel显示图像要清晰。
void MyMainWindow::paintEvent(QPaintEvent *event)
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

//主窗口关闭时处理函数
void MyMainWindow::closeEvent(QCloseEvent *event)
{
	if (hDevice) GXSendCommand(hDevice, GX_COMMAND_ACQUISITION_STOP); //发送停止采集命令
	if (hDevice) GXUnregisterCaptureCallback(hDevice); //注销采集回调
	if (hDevice) GXCloseDevice(hDevice); //如果有设备打开，关闭设备
	GXCloseLib(); //在结束的时候调用 GXCLoseLib()释放资源
}

//save函数
void MyMainWindow::saveFile()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Save", "", "JPG File(*.jpg);;PNG File(*.png);;BMP File(*.bmp);;ICON File(*.ico);;GIF File(*.gif);;TIFF File(*.tif)");
	if (fileName.isEmpty()) return;//如果文件名为空，返回。

	//保存功能，按照原图像格式或者新图像格式保存。
	int nIndex = fileName.lastIndexOf('.');//寻找‘.’符号在字符串中的id
	nIndex++;
	int nLen = fileName.length() - nIndex;
	QString strSuffix = fileName.right(nLen);//截取‘.’符号后面的字符串，这是为了获取用户选择的文件后缀名
	
	image.save(fileName, strSuffix.toUpper().toUtf8()); //保存图片
	
	QString scoreString = cn("保存成功！"); //需要显示的字符串
	QMessageBox::about(this, cn("图像"),"<font color=black>" + scoreString); //调用QMessageBox::about()函数，提示保存成功信息
}


//start函数
void MyMainWindow::start()
{
	if (hDevice) GXSendCommand(hDevice, GX_COMMAND_ACQUISITION_START); //发送开采命令
	
	Start->setEnabled(false);//禁用
	Stop->setEnabled(true);//启用
}

//stop函数
void MyMainWindow::stop()
{
	if (hDevice) GXSendCommand(hDevice, GX_COMMAND_ACQUISITION_STOP); //发送停止采集命令
	
	Stop->setEnabled(false);//禁用
	Start->setEnabled(true);//启用
}

///*
//定义MyMainWindowGray类
class MyMainWindowGray:public QMainWindow
{
	public:
		MyMainWindowGray(QWidget* parent=nullptr);
		void setImage(QImage image){this->imageGray=image;}
		
	private:
		QMenuBar *menuBar=new QMenuBar; //定义菜单栏对象指针
		QToolBar *hToolBar = new QToolBar; //定义水平工具栏对象指针
		
		QToolButton *Start = new QToolButton; //定义一个工具栏按钮指针Start
		QToolButton *Stop = new QToolButton; //定义一个工具栏按钮指针Stop
		
		QImage imageGray;//从相机抓取的图像转换成QImage图像。
		
		void paintEvent(QPaintEvent *event) override; //本窗口的描画函数，自动执行。
		void closeEvent(QCloseEvent *event) override;	//主窗口关闭时处理函数
		void saveFile(); //点击save时的处理函数
		void stop();//点击stop时的处理函数
		void start();//点击start时的处理函数
};

//MyMainWindowGray类构造函数
MyMainWindowGray::MyMainWindowGray(QWidget *parent):QMainWindow(parent)
{
	//基础设置
	this->resize(720,450);
	this->setWindowTitle(cn("ght显微图像处理软件（灰度）")); //设置窗体的标题
	
	this->setMenuBar(menuBar); //将menuBar设置为本窗口的菜单栏
	this->addToolBar(Qt::TopToolBarArea, hToolBar); //添加toolBar工具栏到本窗口上部
	
	//菜单设置
	QMenu *fileMenu=new QMenu("File"); //定义菜单fileMenu
	fileMenu->addAction(QPixmap("save.png"), "Save", this, &saveFile); //添加Save菜单项到fileMenu
	menuBar->addMenu(fileMenu); //将fileMenu菜单添加到菜单栏
	
	//hToolBar工具栏设置
	
	Start->setIcon(QPixmap("start.png")); //设置按钮的图标
	Start->setEnabled(false);//禁用
	Start->setToolTip("Start"); //设置提示信息
	connect(Start, &QToolButton::clicked, this, &start); //连接信号函数和处理函数
	
	
	Stop->setIcon(QPixmap("stop.png")); //设置按钮的图标
	Stop->setEnabled(true);//启用
	Stop->setToolTip("Stop"); //设置提示信息
	connect(Stop, &QToolButton::clicked, this, &stop); //连接信号函数和处理函数
	
	hToolBar->setIconSize(QSize(24, 24)); //设置工具栏图标的尺寸，单位为像素
	hToolBar->addWidget(Start); //将Start添加到htoolBar
	hToolBar->addWidget(Stop); //将Stop添加到htoolBar
}

//将采集转换的image图像通过paintEvent函数描画出来，比通过QLabel显示图像要清晰。
void MyMainWindowGray::paintEvent(QPaintEvent *event)
{
	if (imageGray.isNull()) return;
	
	QPainter painter(this);
	//painter.setRenderHints(QPainter::Antialiasing,true);//消锯齿
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true); //平滑绘图
	
	double widthRatio=(double)this->width()/imageGray.width();
	double heightRatio=(double)this->height()/imageGray.height();
	double minRatio=qMin(widthRatio, heightRatio);
	double drawWidth=imageGray.width()*minRatio;
	double drawHeight=imageGray.height()*minRatio;
	double x=(this->width()-drawWidth)/2;
	double y=(this->height()-drawHeight)/2;
	
	painter.drawImage(QRectF(x, y, drawWidth, drawHeight), imageGray); //将image描画在矩形范围内。	
}

//主窗口关闭时处理函数
void MyMainWindowGray::closeEvent(QCloseEvent *event)
{
	if (hDevice) GXSendCommand(hDevice, GX_COMMAND_ACQUISITION_STOP); //发送停止采集命令
	if (hDevice) GXUnregisterCaptureCallback(hDevice); //注销采集回调
	if (hDevice) GXCloseDevice(hDevice); //如果有设备打开，关闭设备
	GXCloseLib(); //在结束的时候调用 GXCLoseLib()释放资源
}

//save函数
void MyMainWindowGray::saveFile()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Save", "", "JPG File(*.jpg);;PNG File(*.png);;BMP File(*.bmp);;ICON File(*.ico);;GIF File(*.gif);;TIFF File(*.tif)");
	if (fileName.isEmpty()) return;//如果文件名为空，返回。
	
	//保存功能，按照原图像格式或者新图像格式保存。
	int nIndex = fileName.lastIndexOf('.');//寻找‘.’符号在字符串中的id
	nIndex++;
	int nLen = fileName.length() - nIndex;
	QString strSuffix = fileName.right(nLen);//截取‘.’符号后面的字符串，这是为了获取用户选择的文件后缀名
	
	imageGray.save(fileName, strSuffix.toUpper().toUtf8()); //保存图片
	
	QString scoreString = cn("保存成功！"); //需要显示的字符串
	QMessageBox::about(this, cn("图像"),"<font color=black>" + scoreString); //调用QMessageBox::about()函数，提示保存成功信息
}

//start函数
void MyMainWindowGray::start()
{
	if (hDevice) GXSendCommand(hDevice, GX_COMMAND_ACQUISITION_START); //发送开采命令
	
	Start->setEnabled(false);//禁用
	Stop->setEnabled(true);//启用
}

//stop函数
void MyMainWindowGray::stop()
{
	if (hDevice) GXSendCommand(hDevice, GX_COMMAND_ACQUISITION_STOP); //发送停止采集命令
	
	Stop->setEnabled(false);//禁用
	Start->setEnabled(true);//启用
}

//声明一个单例类窗口并初始化指向空指针？
static MyMainWindowGray *formGray=nullptr;
//*/

//声明一个单例类窗口并初始化指向空指针？
static MyMainWindow *form=nullptr;

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
		pRGB24Buf = NULL;//释放内存
		///*
		QImage imageGray=image.copy(QRect(0,0,image.width(),image.height())); //从image复制一份对象
		if (formGray)
		{
			int width=imageGray.width(); //图像宽
 			int height=imageGray.height(); //图像高
			
			QColor color;
			uchar grayLevel; //范围0~255
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					color=imageGray.pixelColor(i,j); //获取第i行第j个像素的颜色
					grayLevel=(color.red()*30+color.green()*59+color.blue()*11)/100; //计算灰度值
					imageGray.setPixelColor(i,j,QColor(grayLevel,grayLevel,grayLevel,color.alpha())); //设置新颜色
				}
			}
			
			formGray->setImage(imageGray.copy());
			formGray->update();	
			//cout<<"setImage and update!"<<endl;
		}
		
		delete [] pRGB24Buf;
		pRGB24Buf = NULL;//释放内存
		//*/
	}
	
	return;
}

//相机运行函数
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
		
			status = GXRegisterCaptureCallback(hDevice, NULL, OnFrameCallbackFun);//注册图像处理回调函数
			cout<<"GXRegisterCaptureCallback status="<<status<<endl;
			status = GXSendCommand(hDevice, GX_COMMAND_ACQUISITION_START);//发送开采命令
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
	QApplication app(argc, argv);//创建新程序
	
	MyMainWindow widget;//创建窗口对象
	MyMainWindowGray widgetGray;//创建窗口对象
	
	widget.show();//显示窗口对象
	widgetGray.show();//显示窗口对象
	
	form=&widget;
	formGray=&widgetGray;
	
	initializeCamera();

	return app.exec();//开启程序消息循环
}
