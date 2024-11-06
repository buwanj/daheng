#include <QtWidgets\QtWidgets> //包含QtWidgets头文件
class MyWidget:public QWidget //定义一个MyWidget类，继承自QWidget窗口类
{
	public:
	MyWidget(QWidget *parent = 0); //构造函数，参数是父窗体指针，默认是0，即没有父窗口
	QLabel *label=new QLabel; 	//创建一个QLabel对象指针label标签
};
//定义构造函数，参数是父窗体指针，先调用基类构造函数QWidget(parent)
MyWidget::MyWidget(QWidget *parent):QWidget(parent)
{
	this->setWindowTitle("My First Qt Program!"); //设置窗体的标题
	this->resize(860,520); 		//变更窗体尺寸
	
	QHBoxLayout *hLayout=new QHBoxLayout; //创建一个QHBoxLayout布局器对象指针hLayout，父窗体是本对象窗口
	hLayout->setContentsMargins(10,10,10,10);//设置边界宽度是10个像素
	
	this->setLayout(hLayout); //设置本窗口对象的布局器是水平布局器hLayout
	
	label->setAlignment(Qt::AlignCenter); //设置标签上的文字或图像居中显示
	label->setFont(QFont("Arial", 36, QFont::Bold,false)); 	//设置程序默认字体
	label->setStyleSheet("color:red; background-color:rgb(242,255,242);"); //设置标签label的文字颜色和背景颜色
	label->setText("I Love C++ and Qt!"); 	//设置标签label的文字
	hLayout->addWidget(label); //将label标签加入到hLayout布局器中。
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv); //创建程序app

	MyWidget widget; //创建一个MyWidget类的对象widget
	widget.show(); //显示sampleWindow对象窗口

	return app.exec(); //开启程序消息循环
}

