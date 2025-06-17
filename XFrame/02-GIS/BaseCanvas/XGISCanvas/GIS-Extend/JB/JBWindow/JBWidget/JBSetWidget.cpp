#include "JBSetWidget.h"

#include <QHBoxLayout>
#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QSpacerItem>
#include "DrawJBWidget.h"
#include "./../../JBPixmapHelper.h"
#include "JBType.h"
#include "./../../QtDrawJBPainter.h"
#include "QColorDialog"
//#include <QApplication>

ByyJBSetDialog::ByyJBSetDialog(QT_GROUP* jbGroup, QWidget *parent /*= 0*/) : QDialog(parent)
    , m_jbGroup(jbGroup),
    m_nSize(200),
    m_nLineWidth(2)
{
    createUI();
	m_spiSize->setValue(200);
    m_spiLineWidth->setValue(2);

	connect(m_spiSize,SIGNAL(valueChanged(int)),this,SLOT(slotSizeChange(int)));
	connect(m_spiLineWidth,SIGNAL(valueChanged(int)),this,SLOT(slotLineWidthChange(int)));
	connect(m_btnColor,SIGNAL(clicked(bool)),this,SLOT(slotColorChange()));
	connect(m_btnFinish,SIGNAL(clicked(bool)),this,SLOT(accept()));
}

ByyJBSetDialog::~ByyJBSetDialog()
{

}

//设置军变显示到军标widget上
void ByyJBSetDialog::setJBGroup(QT_GROUP* jbGroup)
{
    m_pGroup = NULL;
	m_jbGroup = jbGroup;
	QColor color;
	JB_UNIT_HEAD* pJBUH = (JB_UNIT_HEAD*)m_jbGroup->byteInfo.data();
	LINE_ATTR* LA = (LINE_ATTR *)(pJBUH + 1);

	color = MakeColor(LA->sLineC);

	m_btnColor->setPalette(QColor(color));

	widget->setPixmap(CJBPixmapHelper::Instance()->getPixmap(
                          *jbGroup,m_spiSize->value(),m_spiSize->value(),QColor(),m_spiLineWidth->value()));
}

void ByyJBSetDialog::setJBGroup(JB_GROUP *jbGroup)
{
    m_pGroup = jbGroup;
    m_jbGroup = NULL;
    if(!m_color.isValid())
    {
        m_color = Qt::red;
    }
    m_btnColor->setPalette(m_color);
    widget->setPixmap(CJBPixmapHelper::Instance()->getPixmap(*m_pGroup,QSize(m_spiSize->value(),m_spiSize->value()),m_color,-1));
}
#include <qdebug.h>
//改变军标大小
void ByyJBSetDialog::slotSizeChange(int nValue)
{
    m_nSize = nValue;
     updateData();
    qDebug() << nValue;
    qDebug() << m_nLineWidth;

//    if(!m_jbGroup)
//    {
//        widget->setPixmap(CJBPixmapHelper::Instance()->getPixmap(*m_pGroup,QSize(m_nSize,m_nSize),
//                                                                 m_btnColor->palette().color(QPalette::Background),
//                                                                 m_nLineWidth));
//    }
//    else if(!m_pGroup)
//    {
//        widget->setPixmap(
//            CJBPixmapHelper::Instance()->getPixmap(*m_jbGroup,
//           m_nSize,m_nSize,m_btnColor->palette().color(QPalette::Background),m_spiLineWidth->value()));
//    }
}

//改变军标颜色
void ByyJBSetDialog::slotColorChange()
{
	QColor color = QColorDialog::getColor();
	if(color.isValid())
	{
           m_btnColor->setPalette(color);
//        if(!m_jbGroup)
//        {
//            widget->setPixmap(CJBPixmapHelper::Instance()->getPixmap(*m_pGroup,QSize(m_spiSize->value(),m_spiSize->value()),m_color,m_spiLineWidth->value()));
//        }
//        else if(!m_pGroup)
//        {
//            widget->setPixmap(
//                CJBPixmapHelper::Instance()->getPixmap(*m_jbGroup,
//                m_spiSize->value(),m_spiSize->value(),color,m_spiLineWidth->value()));
//        }
        m_color = color;
        updateData();
	}
}

//设置军标线宽
void ByyJBSetDialog::slotLineWidthChange(int nValue)
{
    m_nLineWidth = nValue;
    updateData();
//    if(!m_jbGroup)
//    {
//        widget->setPixmap(CJBPixmapHelper::Instance()->getPixmap(*m_pGroup,QSize(m_nSize,m_nSize),m_color,m_nLineWidth));
//    }
//    else if(!m_pGroup)
//    {
//        widget->setPixmap(
//            CJBPixmapHelper::Instance()->getPixmap(*m_jbGroup,
//           m_nSize,m_nSize,m_color,m_nLineWidth));
//    }
}

void ByyJBSetDialog::createUI()
{
    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("Dialog"));
    this->resize(443, 200);
    this->setMinimumSize(QSize(443, 200));
    this->setMaximumSize(QSize(443, 218));
    horizontalLayout_5 = new QHBoxLayout(this);
    horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
    widget = new ByyDrawJBWidget(this);
    widget->setObjectName(QString::fromUtf8("widget"));
    widget->setMinimumSize(QSize(200, 200));
    widget->setMaximumSize(QSize(200, 200));

    horizontalLayout_5->addWidget(widget);

    verticalLayout = new QVBoxLayout();
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    label = new QLabel(this);
    label->setObjectName(QString::fromUtf8("label"));

    horizontalLayout->addWidget(label);

    m_spiSize = new QSpinBox(this);
    m_spiSize->setObjectName(QString::fromUtf8("m_spiSize"));
    m_spiSize->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_spiSize->setMinimum(30);
    m_spiSize->setMaximum(300);
    m_spiSize->setValue(200);

    horizontalLayout->addWidget(m_spiSize);


    verticalLayout->addLayout(horizontalLayout);

    horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
    label_3 = new QLabel(this);
    label_3->setObjectName(QString::fromUtf8("label_3"));

    horizontalLayout_2->addWidget(label_3);

    m_spiLineWidth = new QSpinBox(this);
    m_spiLineWidth->setObjectName(QString::fromUtf8("m_spiLineWidth"));
    m_spiLineWidth->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_spiLineWidth->setMinimum(1);
    m_spiLineWidth->setMaximum(99);
    m_spiLineWidth->setValue(1);

    horizontalLayout_2->addWidget(m_spiLineWidth);


    verticalLayout->addLayout(horizontalLayout_2);

    horizontalLayout_3 = new QHBoxLayout();
    horizontalLayout_3->setSpacing(0);
    horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
    label_4 = new QLabel(this);
    label_4->setObjectName(QString::fromUtf8("label_4"));

    horizontalLayout_3->addWidget(label_4);

    m_btnColor = new QToolButton(this);
    m_btnColor->setObjectName(QString::fromUtf8("m_btnColor"));
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(m_btnColor->sizePolicy().hasHeightForWidth());
    m_btnColor->setSizePolicy(sizePolicy);

    horizontalLayout_3->addWidget(m_btnColor);

    horizontalLayout_3->setStretch(0, 1);
    horizontalLayout_3->setStretch(1, 1);

    verticalLayout->addLayout(horizontalLayout_3);

    horizontalLayout_4 = new QHBoxLayout();
    horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
    m_btnFinish = new QPushButton(this);
    m_btnFinish->setObjectName(QString::fromUtf8("m_btnFinish"));

    horizontalLayout_4->addWidget(m_btnFinish);

    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_4->addItem(horizontalSpacer);


    verticalLayout->addLayout(horizontalLayout_4);


    horizontalLayout_5->addLayout(verticalLayout);




    QMetaObject::connectSlotsByName(this);


    this->setWindowTitle(QString::fromLocal8Bit("设置军标"));
    //label->setText(QApplication::translate("Dialog", "\345\244\247\345\260\217\357\274\232", 0, QApplication::UnicodeUTF8));
   //label_3->setText(QApplication::translate("Dialog", "\347\272\277\345\256\275\357\274\232", 0, QApplication::UnicodeUTF8));
    //label_4->setText(QApplication::translate("Dialog", "\351\242\234\350\211\262\357\274\232", 0, QApplication::UnicodeUTF8));
    m_btnColor->setText(QString());
   // m_btnFinish->setText(QApplication::translate("Dialog", "\345\256\214\346\210\220", 0, QApplication::UnicodeUTF8));

    m_btnColor->setAutoFillBackground(true);
}

void ByyJBSetDialog::updateData()
{
        if(!m_jbGroup)
        {
            widget->setPixmap(CJBPixmapHelper::Instance()->getPixmap(*m_pGroup,QSize(m_nSize,m_nSize),m_color,m_nLineWidth));
        }
        else if(!m_pGroup)
        {
            widget->setPixmap(
                CJBPixmapHelper::Instance()->getPixmap(*m_jbGroup,
               m_nSize,m_nSize,m_color,m_nLineWidth));
        }
}
