//���½���

#include <QtWidgets/QtWidgets> //Qt�����ͷ�ļ�
#include "DxImageProc.h" //����������ͼ����ͷ�ļ�
#include "GxIAPI.h" //�����������Ŀ��ƺͲɼ�ͷ�ļ�
#include <iostream> //C++��׼�������ͷ�ļ�
#define cn QString::fromLocal8Bit //����cn����QString::fromLocal8Bit

using namespace std;

GX_DEV_HANDLE hDevice = NULL;//�豸��

//����MyMainWindow��
class MyMainWindow:public QMainWindow
{
	public:
		MyMainWindow(QWidget* parent=nullptr);
		void setImage(QImage image){this->image=image;}
		
	private:
		QMenuBar *menuBar=new QMenuBar; //����˵�������ָ��
		QToolBar *hToolBar = new QToolBar; //����ˮƽ����������ָ��
		
		QToolButton *Start = new QToolButton; //����һ����������ťָ��Start
		QToolButton *Stop = new QToolButton; //����һ����������ťָ��Stop
		
		QImage image;//�����ץȡ��ͼ��ת����QImageͼ��
		
		void paintEvent(QPaintEvent *event) override; //�����ڵ��軭�������Զ�ִ�С�
		void closeEvent(QCloseEvent *event) override;	//�����ڹر�ʱ������
		void saveFile(); //���saveʱ�Ĵ�����
		void stop();//���stopʱ�Ĵ�����
		void start();//���startʱ�Ĵ�����
};

//MyMainWindow�๹�캯��
MyMainWindow::MyMainWindow(QWidget *parent):QMainWindow(parent)
{
	//��������
	this->resize(720,450);
	this->setWindowTitle(cn("ght��΢ͼ�������")); //���ô���ı���
	
	this->setMenuBar(menuBar); //��menuBar����Ϊ�����ڵĲ˵���
	this->addToolBar(Qt::TopToolBarArea, hToolBar); //���toolBar���������������ϲ�
	
	//�˵�����
	QMenu *fileMenu=new QMenu("File"); //����˵�fileMenu
	fileMenu->addAction(QPixmap("save.png"), "Save", this, &saveFile); //���Save�˵��fileMenu
	menuBar->addMenu(fileMenu); //��fileMenu�˵���ӵ��˵���
	
	//hToolBar����������
	
	Start->setIcon(QPixmap("start.png")); //���ð�ť��ͼ��
	Start->setEnabled(false);//����
	Start->setToolTip("Start"); //������ʾ��Ϣ
	connect(Start, &QToolButton::clicked, this, &start); //�����źź����ʹ�����
	
	QToolButton *Stop = new QToolButton; //����һ����������ťָ��Stop
	Stop->setEnabled(true);//����
	Stop->setIcon(QPixmap("stop.png")); //���ð�ť��ͼ��
	Stop->setToolTip("Stop"); //������ʾ��Ϣ
	connect(Stop, &QToolButton::clicked, this, &stop); //�����źź����ʹ�����
	
	hToolBar->setIconSize(QSize(24, 24)); //���ù�����ͼ��ĳߴ磬��λΪ����
	hToolBar->addWidget(Start); //��Start��ӵ�htoolBar
	hToolBar->addWidget(Stop); //��Stop��ӵ�htoolBar
}

//���ɼ�ת����imageͼ��ͨ��paintEvent�����軭��������ͨ��QLabel��ʾͼ��Ҫ������
void MyMainWindow::paintEvent(QPaintEvent *event)
{
	if (image.isNull()) return;
	
	QPainter painter(this);
	//painter.setRenderHints(QPainter::Antialiasing,true);//�����
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true); //ƽ����ͼ
	
	double widthRatio=(double)this->width()/image.width();
	double heightRatio=(double)this->height()/image.height();
	double minRatio=qMin(widthRatio, heightRatio);
	double drawWidth=image.width()*minRatio;
	double drawHeight=image.height()*minRatio;
	double x=(this->width()-drawWidth)/2;
	double y=(this->height()-drawHeight)/2;
	
	painter.drawImage(QRectF(x, y, drawWidth, drawHeight), image); //��image�軭�ھ��η�Χ�ڡ�	
}

//�����ڹر�ʱ������
void MyMainWindow::closeEvent(QCloseEvent *event)
{
	if (hDevice) GXSendCommand(hDevice, GX_COMMAND_ACQUISITION_STOP); //����ֹͣ�ɼ�����
	if (hDevice) GXUnregisterCaptureCallback(hDevice); //ע���ɼ��ص�
	if (hDevice) GXCloseDevice(hDevice); //������豸�򿪣��ر��豸
	GXCloseLib(); //�ڽ�����ʱ����� GXCLoseLib()�ͷ���Դ
}

//save����
void MyMainWindow::saveFile()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Save", "", "JPG File(*.jpg);;PNG File(*.png);;BMP File(*.bmp);;ICON File(*.ico);;GIF File(*.gif);;TIFF File(*.tif)");
	if (fileName.isEmpty()) return;//����ļ���Ϊ�գ����ء�

	//���湦�ܣ�����ԭͼ���ʽ������ͼ���ʽ���档
	int nIndex = fileName.lastIndexOf('.');//Ѱ�ҡ�.���������ַ����е�id
	nIndex++;
	int nLen = fileName.length() - nIndex;
	QString strSuffix = fileName.right(nLen);//��ȡ��.�����ź�����ַ���������Ϊ�˻�ȡ�û�ѡ����ļ���׺��
	
	image.save(fileName, strSuffix.toUpper().toUtf8()); //����ͼƬ
	
	QString scoreString = cn("����ɹ���"); //��Ҫ��ʾ���ַ���
	QMessageBox::about(this, cn("ͼ��"),"<font color=black>" + scoreString); //����QMessageBox::about()��������ʾ����ɹ���Ϣ
}


//start����
void MyMainWindow::start()
{
	if (hDevice) GXSendCommand(hDevice, GX_COMMAND_ACQUISITION_START); //���Ϳ�������
	
	Start->setEnabled(false);//����
	Stop->setEnabled(true);//����
}

//stop����
void MyMainWindow::stop()
{
	if (hDevice) GXSendCommand(hDevice, GX_COMMAND_ACQUISITION_STOP); //����ֹͣ�ɼ�����
	
	Stop->setEnabled(false);//����
	Start->setEnabled(true);//����
}

///*
//����MyMainWindowGray��
class MyMainWindowGray:public QMainWindow
{
	public:
		MyMainWindowGray(QWidget* parent=nullptr);
		void setImage(QImage image){this->imageGray=image;}
		
	private:
		QMenuBar *menuBar=new QMenuBar; //����˵�������ָ��
		QToolBar *hToolBar = new QToolBar; //����ˮƽ����������ָ��
		
		QToolButton *Start = new QToolButton; //����һ����������ťָ��Start
		QToolButton *Stop = new QToolButton; //����һ����������ťָ��Stop
		
		QImage imageGray;//�����ץȡ��ͼ��ת����QImageͼ��
		
		void paintEvent(QPaintEvent *event) override; //�����ڵ��軭�������Զ�ִ�С�
		void closeEvent(QCloseEvent *event) override;	//�����ڹر�ʱ������
		void saveFile(); //���saveʱ�Ĵ�����
		void stop();//���stopʱ�Ĵ�����
		void start();//���startʱ�Ĵ�����
};

//MyMainWindowGray�๹�캯��
MyMainWindowGray::MyMainWindowGray(QWidget *parent):QMainWindow(parent)
{
	//��������
	this->resize(720,450);
	this->setWindowTitle(cn("ght��΢ͼ����������Ҷȣ�")); //���ô���ı���
	
	this->setMenuBar(menuBar); //��menuBar����Ϊ�����ڵĲ˵���
	this->addToolBar(Qt::TopToolBarArea, hToolBar); //���toolBar���������������ϲ�
	
	//�˵�����
	QMenu *fileMenu=new QMenu("File"); //����˵�fileMenu
	fileMenu->addAction(QPixmap("save.png"), "Save", this, &saveFile); //���Save�˵��fileMenu
	menuBar->addMenu(fileMenu); //��fileMenu�˵���ӵ��˵���
	
	//hToolBar����������
	
	Start->setIcon(QPixmap("start.png")); //���ð�ť��ͼ��
	Start->setEnabled(false);//����
	Start->setToolTip("Start"); //������ʾ��Ϣ
	connect(Start, &QToolButton::clicked, this, &start); //�����źź����ʹ�����
	
	
	Stop->setIcon(QPixmap("stop.png")); //���ð�ť��ͼ��
	Stop->setEnabled(true);//����
	Stop->setToolTip("Stop"); //������ʾ��Ϣ
	connect(Stop, &QToolButton::clicked, this, &stop); //�����źź����ʹ�����
	
	hToolBar->setIconSize(QSize(24, 24)); //���ù�����ͼ��ĳߴ磬��λΪ����
	hToolBar->addWidget(Start); //��Start��ӵ�htoolBar
	hToolBar->addWidget(Stop); //��Stop��ӵ�htoolBar
}

//���ɼ�ת����imageͼ��ͨ��paintEvent�����軭��������ͨ��QLabel��ʾͼ��Ҫ������
void MyMainWindowGray::paintEvent(QPaintEvent *event)
{
	if (imageGray.isNull()) return;
	
	QPainter painter(this);
	//painter.setRenderHints(QPainter::Antialiasing,true);//�����
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true); //ƽ����ͼ
	
	double widthRatio=(double)this->width()/imageGray.width();
	double heightRatio=(double)this->height()/imageGray.height();
	double minRatio=qMin(widthRatio, heightRatio);
	double drawWidth=imageGray.width()*minRatio;
	double drawHeight=imageGray.height()*minRatio;
	double x=(this->width()-drawWidth)/2;
	double y=(this->height()-drawHeight)/2;
	
	painter.drawImage(QRectF(x, y, drawWidth, drawHeight), imageGray); //��image�軭�ھ��η�Χ�ڡ�	
}

//�����ڹر�ʱ������
void MyMainWindowGray::closeEvent(QCloseEvent *event)
{
	if (hDevice) GXSendCommand(hDevice, GX_COMMAND_ACQUISITION_STOP); //����ֹͣ�ɼ�����
	if (hDevice) GXUnregisterCaptureCallback(hDevice); //ע���ɼ��ص�
	if (hDevice) GXCloseDevice(hDevice); //������豸�򿪣��ر��豸
	GXCloseLib(); //�ڽ�����ʱ����� GXCLoseLib()�ͷ���Դ
}

//save����
void MyMainWindowGray::saveFile()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Save", "", "JPG File(*.jpg);;PNG File(*.png);;BMP File(*.bmp);;ICON File(*.ico);;GIF File(*.gif);;TIFF File(*.tif)");
	if (fileName.isEmpty()) return;//����ļ���Ϊ�գ����ء�
	
	//���湦�ܣ�����ԭͼ���ʽ������ͼ���ʽ���档
	int nIndex = fileName.lastIndexOf('.');//Ѱ�ҡ�.���������ַ����е�id
	nIndex++;
	int nLen = fileName.length() - nIndex;
	QString strSuffix = fileName.right(nLen);//��ȡ��.�����ź�����ַ���������Ϊ�˻�ȡ�û�ѡ����ļ���׺��
	
	imageGray.save(fileName, strSuffix.toUpper().toUtf8()); //����ͼƬ
	
	QString scoreString = cn("����ɹ���"); //��Ҫ��ʾ���ַ���
	QMessageBox::about(this, cn("ͼ��"),"<font color=black>" + scoreString); //����QMessageBox::about()��������ʾ����ɹ���Ϣ
}

//start����
void MyMainWindowGray::start()
{
	if (hDevice) GXSendCommand(hDevice, GX_COMMAND_ACQUISITION_START); //���Ϳ�������
	
	Start->setEnabled(false);//����
	Stop->setEnabled(true);//����
}

//stop����
void MyMainWindowGray::stop()
{
	if (hDevice) GXSendCommand(hDevice, GX_COMMAND_ACQUISITION_STOP); //����ֹͣ�ɼ�����
	
	Stop->setEnabled(false);//����
	Start->setEnabled(true);//����
}

//����һ�������ര�ڲ���ʼ��ָ���ָ�룿
static MyMainWindowGray *formGray=nullptr;
//*/

//����һ�������ര�ڲ���ʼ��ָ���ָ�룿
static MyMainWindow *form=nullptr;

//ͼ��ص�������
static void GX_STDC OnFrameCallbackFun(GX_FRAME_CALLBACK_PARAM* pFrame)
{
	//cout<<"OnFrameCallbackFun excuted!"<<endl;
	
	if (pFrame->status == 0)
	{
		//cout<<"pFrame status ==0"<<endl;
		
		BYTE *pRGB24Buf = new BYTE[pFrame->nWidth * pFrame->nHeight * 3]; //���ͼ�� RGB ����
		if (pRGB24Buf == NULL) return ;
		else memset(pRGB24Buf,0,pFrame->nWidth * pFrame->nHeight * 3 * sizeof(BYTE)); //��������ʼ��

		DX_BAYER_CONVERT_TYPE cvtype = RAW2RGB_NEIGHBOUR; //ѡ���ֵ�㷨
		DX_PIXEL_COLOR_FILTER nBayerType = BAYERGR; //ѡ��ͼ�� Bayer ��ʽ
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
		pRGB24Buf = NULL;//�ͷ��ڴ�
		///*
		QImage imageGray=image.copy(QRect(0,0,image.width(),image.height())); //��image����һ�ݶ���
		if (formGray)
		{
			int width=imageGray.width(); //ͼ���
 			int height=imageGray.height(); //ͼ���
			
			QColor color;
			uchar grayLevel; //��Χ0~255
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					color=imageGray.pixelColor(i,j); //��ȡ��i�е�j�����ص���ɫ
					grayLevel=(color.red()*30+color.green()*59+color.blue()*11)/100; //����Ҷ�ֵ
					imageGray.setPixelColor(i,j,QColor(grayLevel,grayLevel,grayLevel,color.alpha())); //��������ɫ
				}
			}
			
			formGray->setImage(imageGray.copy());
			formGray->update();	
			//cout<<"setImage and update!"<<endl;
		}
		
		delete [] pRGB24Buf;
		pRGB24Buf = NULL;//�ͷ��ڴ�
		//*/
	}
	
	return;
}

//������к���
void initializeCamera()
{
	//����GX_STATUSö�����ͱ���status�����ڽ������¸�����ִ�к�Ľ��
	GX_STATUS status = GX_STATUS_SUCCESS; 
	status = GXInitLib(); //��ʼ�����������豸��
	if (status != GX_STATUS_SUCCESS)
	{
		cout<<"GXInitLib failed: "<<status<<endl;
	}

	uint32_t nDeviceNum = 0;
	status = GXUpdateDeviceList(&nDeviceNum, 2000); //�����豸�б�����豸��
	cout<<"Device Number = "<<nDeviceNum<<endl;
	
	GX_OPEN_PARAM stOpenParam;
	if (status == GX_STATUS_SUCCESS&&nDeviceNum> 0)
	{
		//��ö���б��еĵ�һ̨�豸��
		//����ö�ٵ���3̨�����豸����ô�û�������stOpenParam������pszContent�ֶ�Ϊ1��2��3
		stOpenParam.accessMode = GX_ACCESS_EXCLUSIVE;
		stOpenParam.openMode = GX_OPEN_INDEX;
		stOpenParam.pszContent =(char*)"1";
		//ͨ�����кŴ��豸
		//stOpenParam.openMode = GX_OPEN_SN;
		//stOpenParam.pszContent =(char*)"KB0210030058";
		//���ˮ�����MER-500-14U3C-L�ͺŷֱ���2592*1944 pixels
		//�������8λ��10λ��֡��14fps����ɫ��ͼ�����ݸ�ʽ��Bayer GR8/Bayer GR10, ������>1556��
		
		status = GXOpenDevice(&stOpenParam, &hDevice); //���豸
		
		if (status==GX_STATUS_SUCCESS)
		{
			cout<<"Device has been opened successfully!"<<endl;
		
			status = GXRegisterCaptureCallback(hDevice, NULL, OnFrameCallbackFun);//ע��ͼ����ص�����
			cout<<"GXRegisterCaptureCallback status="<<status<<endl;
			status = GXSendCommand(hDevice, GX_COMMAND_ACQUISITION_START);//���Ϳ�������
			cout<<"Acquisition Start status="<<status<<endl;
		}
		else
		{
			cout<<"Failed in openning device!"<<endl; //���û�гɹ����豸����ʾ��Ϣ
		}
	}
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);//�����³���
	
	MyMainWindow widget;//�������ڶ���
	MyMainWindowGray widgetGray;//�������ڶ���
	
	widget.show();//��ʾ���ڶ���
	widgetGray.show();//��ʾ���ڶ���
	
	form=&widget;
	formGray=&widgetGray;
	
	initializeCamera();

	return app.exec();//����������Ϣѭ��
}
