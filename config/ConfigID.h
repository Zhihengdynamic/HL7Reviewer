#ifndef CONFIGID_H
#define CONFIGID_H

#include <QVariant>

struct  STUCfgItem
{
	quint32         cfgID;         //ID
	QVariant::Type  cfgValType;    //值数据类型
	QVariant        cfgDefVal;     //默认值
	QVariant        cfgVal;        //值
};

#define  ID_MAX_RENDER_SECONDS            0//绘制区域，最大绘制的波形秒数
#define  ID_LAST_OPEN_FILE_PATH           1//保存最后一次，打开文件的路径
#define  ID_INTELIIGENT_CALCULATE_SCALE   2//是否启用智能标尺
#define  ID_HASH_NAME_CHAR_COUNT          3//智能标尺根据前N个字母来计算波形的最大值和最小值
#define  ID_RENDER_AREA_HEIGHT            4//波形绘制区域的高度（像素）。界面不提供更改,可以更改注册表
#define  ID_RENDER_MEDICAL_ECG_WAVE       5//ECG波形采用医用标准。1厘米对应1mV.
#define  ID_HOR_PIXELS_PER_CM             6//水平方向，多少像素1厘米
#define  ID_VER_PIXELS_PER_CM             7//竖直方向，多少像素1厘米
#define  ID_RENDER_WAVE_SCALE_TEXT        8//是否绘制标尺文字。
#define  ID_WAVE_SPEED                    9//波形走速，每秒多少个毫米.默认25mm/s。界面不提供更改，可以更改注册表

static STUCfgItem s_cfgList[] =
{
	{ID_MAX_RENDER_SECONDS,					QVariant::Int,			32,			32		},
	{ID_LAST_OPEN_FILE_PATH,				QVariant::String,		".",		"."		},
	{ID_INTELIIGENT_CALCULATE_SCALE,		QVariant::Bool,         true,       true	},
	{ID_HASH_NAME_CHAR_COUNT,				QVariant::Int,			3,          3		},
	{ID_RENDER_AREA_HEIGHT,					QVariant::Int,			120,        120		},
	{ID_RENDER_MEDICAL_ECG_WAVE,			QVariant::Bool,			true,		true	},
	{ID_HOR_PIXELS_PER_CM,					QVariant::Double,		44,			44		},
	{ID_VER_PIXELS_PER_CM,					QVariant::Double,		44,			44		},
	{ID_RENDER_WAVE_SCALE_TEXT,				QVariant::Bool,			true,		true	},
	{ID_WAVE_SPEED,							QVariant::Double,		25,			25		},
};


#endif //CONFIGID_H
