#include <QtWidgets\QtWidgets> //����QtWidgetsͷ�ļ�
class MyWidget:public QWidget //����һ��MyWidget�࣬�̳���QWidget������
{
	public:
	MyWidget(QWidget *parent = 0); //���캯���������Ǹ�����ָ�룬Ĭ����0����û�и�����
	QLabel *label=new QLabel; 	//����һ��QLabel����ָ��label��ǩ
};
//���幹�캯���������Ǹ�����ָ�룬�ȵ��û��๹�캯��QWidget(parent)
MyWidget::MyWidget(QWidget *parent):QWidget(parent)
{
	this->setWindowTitle("My First Qt Program!"); //���ô���ı���
	this->resize(860,520); 		//�������ߴ�
	
	QHBoxLayout *hLayout=new QHBoxLayout; //����һ��QHBoxLayout����������ָ��hLayout���������Ǳ����󴰿�
	hLayout->setContentsMargins(10,10,10,10);//���ñ߽�����10������
	
	this->setLayout(hLayout); //���ñ����ڶ���Ĳ�������ˮƽ������hLayout
	
	label->setAlignment(Qt::AlignCenter); //���ñ�ǩ�ϵ����ֻ�ͼ�������ʾ
	label->setFont(QFont("Arial", 36, QFont::Bold,false)); 	//���ó���Ĭ������
	label->setStyleSheet("color:red; background-color:rgb(242,255,242);"); //���ñ�ǩlabel��������ɫ�ͱ�����ɫ
	label->setText("I Love C++ and Qt!"); 	//���ñ�ǩlabel������
	hLayout->addWidget(label); //��label��ǩ���뵽hLayout�������С�
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv); //��������app

	MyWidget widget; //����һ��MyWidget��Ķ���widget
	widget.show(); //��ʾsampleWindow���󴰿�

	return app.exec(); //����������Ϣѭ��
}

