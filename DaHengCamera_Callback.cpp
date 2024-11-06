#include <QtWidgets/QtWidgets> //Qt�����ͷ�ļ�
#include "DxImageProc.h" //����������ͼ����ͷ�ļ�
#include "GxIAPI.h" //�����������Ŀ��ƺͲɼ�ͷ�ļ�
#include <iostream> //C++��׼�������ͷ�ļ�

using namespace std;

GX_DEV_HANDLE hDevice = NULL;//�豸��

class MyWidget:public QWidget
{
	public:
		MyWidget(QWidget* parent=nullptr);
		void setImage(QImage image){this->image=image;}
		
	private:
		QImage image;//�����ץȡ��ͼ��ת����QImageͼ��
		void paintEvent(QPaintEvent *event) override; //�����ڵ��軭�������Զ�ִ�С�
		void closeEvent(QCloseEvent *event) override;	//�����ڹر�ʱ������ 
};


MyWidget::MyWidget(QWidget *parent):QWidget(parent)
{
	this->resize(960,600);
}


//���ɼ�ת����imageͼ��ͨ��paintEvent�����軭��������ͨ��QLabel��ʾͼ��Ҫ������
void MyWidget::paintEvent(QPaintEvent *event)
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

void MyWidget::closeEvent(QCloseEvent *event)
{
	if (hDevice) GXSendCommand(hDevice, GX_COMMAND_ACQUISITION_STOP); //����ֹͣ�ɼ�����
	if (hDevice) GXUnregisterCaptureCallback(hDevice); //ע���ɼ��ص�
	if (hDevice) GXCloseDevice(hDevice); //������豸�򿪣��ر��豸
	GXCloseLib(); //�ڽ�����ʱ����� GXCLoseLib()�ͷ���Դ
}

static MyWidget *form=nullptr;

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
		pRGB24Buf = NULL;
	}
	
	return;
}

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
		
			status = GXRegisterCaptureCallback(hDevice, NULL, OnFrameCallbackFun);
			cout<<"GXRegisterCaptureCallback status="<<status<<endl;
			status = GXSendCommand(hDevice, GX_COMMAND_ACQUISITION_START);
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
	QApplication app(argc, argv);
	
	MyWidget widget;
	widget.show();
	form=&widget;
	
	initializeCamera();

	return app.exec();
}
