#include "XColorTable.h"

#define xUS 0x1F
#define xRS 0x1E

#define  CIE_x_r                0.640            // nominal CRT primaries
#define  CIE_y_r                0.330
#define  CIE_x_g                0.290
#define  CIE_y_g                0.600
#define  CIE_x_b                0.150
#define  CIE_y_b                0.060
#define  CIE_x_w                0.295 //0.3333333333          // monitor white point
#define  CIE_y_w                0.315// 0.3333333333
#define CIE_D           (       CIE_x_r*(CIE_y_g - CIE_y_b) + \
            CIE_x_g*(CIE_y_b - CIE_y_r) + \
            CIE_x_b*(CIE_y_r - CIE_y_g)     )
#define CIE_C_rD        ( (1./CIE_y_w) * \
            ( CIE_x_w*(CIE_y_g - CIE_y_b) - \
            CIE_y_w*(CIE_x_g - CIE_x_b) + \
            CIE_x_g*CIE_y_b - CIE_x_b*CIE_y_g     ) )
#define CIE_C_gD        ( (1./CIE_y_w) * \
            ( CIE_x_w*(CIE_y_b - CIE_y_r) - \
            CIE_y_w*(CIE_x_b - CIE_x_r) - \
            CIE_x_r*CIE_y_b + CIE_x_b*CIE_y_r     ) )
#define CIE_C_bD        ( (1./CIE_y_w) * \
            ( CIE_x_w*(CIE_y_r - CIE_y_g) - \
            CIE_y_w*(CIE_x_r - CIE_x_g) + \
            CIE_x_r*CIE_y_g - CIE_x_g*CIE_y_r     ) )

#define CIE_rf          (CIE_y_r*CIE_C_rD/CIE_D)
#define CIE_gf          (CIE_y_g*CIE_C_gD/CIE_D)
#define CIE_bf          (CIE_y_b*CIE_C_bD/CIE_D)
static double tmat[3][3] =       //XYZ to RGB
{ { (CIE_y_g - CIE_y_b - CIE_x_b * CIE_y_g + CIE_y_b * CIE_x_g) / CIE_C_rD, (CIE_x_b
- CIE_x_g - CIE_x_b * CIE_y_g + CIE_x_g * CIE_y_b) / CIE_C_rD, (CIE_x_g * CIE_y_b
- CIE_x_b * CIE_y_g) / CIE_C_rD }, { (CIE_y_b - CIE_y_r - CIE_y_b * CIE_x_r
+ CIE_y_r * CIE_x_b) / CIE_C_gD, (CIE_x_r - CIE_x_b - CIE_x_r * CIE_y_b
+ CIE_x_b * CIE_y_r) / CIE_C_gD, (CIE_x_b * CIE_y_r - CIE_x_r * CIE_y_b)
/ CIE_C_gD }, { (CIE_y_r - CIE_y_g - CIE_y_r * CIE_x_g + CIE_y_g * CIE_x_r)
/ CIE_C_bD,
(CIE_x_g - CIE_x_r - CIE_x_g * CIE_y_r + CIE_x_r * CIE_y_g) / CIE_C_bD, (CIE_x_r
* CIE_y_g - CIE_x_g * CIE_y_r) / CIE_C_bD } };

static double c_gamma = 2.20;

namespace XSpace_Plugins
{
	XColorTable::XColorTable()
	{
		m_unID = 0;
		m_strName = QString();
		m_strObjType = QString();
		m_strRecord = QStringList();
	}

	XColorTable::~XColorTable()
	{
		m_unID = 0;
		m_strName = QString();
		m_strObjType = QString();
		m_strRecord = QStringList();
	}

	XColorTable* XColorTable::fromString(const QStringList& _strColorStr)
	{
		XColorTable* pColorTAB = NULL;
		foreach(QString strLine, _strColorStr)
		{
			if (strLine == "0001" || strLine == "****")
				continue;

			qint32 nIdx = strLine.indexOf(" ");
			if (nIdx < 0){
				if (pColorTAB)
					delete pColorTAB;
				return NULL;
			}
			QString strTypeLine = strLine.left(nIdx);
			strLine.remove(0, nIdx);
			while (!strLine.isEmpty() && strLine.at(0).cell() == 32)
				strLine.remove(0, 1);

			QString strLenLine = QString();
			quint32 unLenLine = 0;
			while (!strLine.isEmpty() && strLine.at(0).isDigit())
			{
				strLenLine.push_back(strLine.at(0));
				strLine.remove(0, 1);
				if (strLenLine.toUInt() == strLine.length()){
					unLenLine = strLenLine.toUInt();
					break;
				}
			}
			if (unLenLine == 0){
				if (pColorTAB)
					delete pColorTAB;
				return NULL;
			}

			if (strTypeLine == "COLS" && unLenLine > 11)	// CS00002NILDAY_BRIGHTUS
			{
				nIdx = strLine.indexOf(xUS);
				if (nIdx < 0){
					if (pColorTAB)
						delete pColorTAB;
					return NULL;
				}

				pColorTAB = new XColorTable();
				pColorTAB->m_strObjType = strLine.left(2);
				pColorTAB->m_unID = strLine.mid(2, 5).toUInt();
				pColorTAB->m_strName = strLine.mid(10, nIdx-10);
				pColorTAB->m_strRecord = _strColorStr;
			}
			else if (strTypeLine == "CCIE" && unLenLine > 12 && pColorTAB)
			{
				QString strKey = strLine.left(5);
				strLine.remove(0, 6);
				nIdx = strLine.indexOf(xUS);
				if (nIdx < 0){
					if (pColorTAB)
						delete pColorTAB;
					return NULL;
				}
				double dx = strLine.left(nIdx).toDouble();
				strLine.remove(0, nIdx+1);
				nIdx = strLine.indexOf(xUS);
				if (nIdx < 0){
					if (pColorTAB)
						delete pColorTAB;
					return NULL;
				}
				double dy = strLine.left(nIdx).toDouble();
				strLine.remove(0, nIdx+1);
				nIdx = strLine.indexOf(xUS);
				if (nIdx < 0){
					if (pColorTAB)
						delete pColorTAB;
					return NULL;
				}
				double dY = strLine.left(nIdx).toDouble();
				strLine.remove(0, nIdx+1);
				nIdx = strLine.indexOf(xUS);
				if (nIdx < 0){
					if (pColorTAB)
						delete pColorTAB;
					return NULL;
				}
				QString strName = strLine.left(nIdx);
				int md = 0;

				XColor xColor;
				xColor.strKey = strKey;
				xColor.strName = strName;
				xColor.xCIEYxy = QVector3D(dY, dx, dy);
				xColor.sRGB = XColorTable::Yxy2sRGB(xColor.xCIEYxy);
				pColorTAB->m_xColorVec.insert(xColor.strKey, xColor);
			}
			else
			{
				if (pColorTAB)
					delete pColorTAB;
				pColorTAB = NULL;
			}
		}

		if (pColorTAB->m_strName == "DAY_BRIGHT" || pColorTAB->m_strName == "DAY_BLACKBACK" || pColorTAB->m_strName == "DAY_WHITEBACK"){
			pColorTAB->m_xColorVec.insert("BASEB", XColor("BASEB", "Back", QColor(170, 195, 240)));
			pColorTAB->m_xColorVec.insert("BASEF", XColor("BASEF", "Front", QColor(170, 175, 80)));
		}else if (pColorTAB->m_strName == "DUSK"){
			pColorTAB->m_xColorVec.insert("BASEB", XColor("BASEB", "Back", QColor(85, 97, 120)));
			pColorTAB->m_xColorVec.insert("BASEF", XColor("BASEF", "Front", QColor(85, 87, 40)));
		}else if (pColorTAB->m_strName == "NIGHT"){
			pColorTAB->m_xColorVec.insert("BASEB", XColor("BASEB", "Back", QColor(42, 48, 60)));
			pColorTAB->m_xColorVec.insert("BASEF", XColor("BASEF", "Front", QColor(42, 43, 20)));
		}
		return pColorTAB;
	}

	QVector3D XColorTable::Yxy2XYZ(const QVector3D& _xCIEColorYxy)
	{
		// X = x*(Y/y)
		// Y = Y;
		// Z = (1-x-y)*(Y/y)
		double dY = _xCIEColorYxy.x();
		double dx = _xCIEColorYxy.y();
		double dy = _xCIEColorYxy.z();

		QVector3D verXYZ(0,0,0);
		if (dY != 0){
			verXYZ.setX(dx*dY / dy);
			verXYZ.setY(dY);
			verXYZ.setZ(((1 - dx - dy)*dY) / dy);
		}
		
		return verXYZ;
	}

	QVector3D XColorTable::XYZ2sRGB(const QVector3D& _xColorXYZ)
	{
		// Ëã·¨
		{
			//X, Y and Z input refer to a D65/2¡ã standard illuminant.
			//sR, sG and sB (standard RGB) output range = 0 ¡Â 255

			/*var_X = X / 100
			var_Y = Y / 100
			var_Z = Z / 100

			var_R = var_X *  3.2406 + var_Y * -1.5372 + var_Z * -0.4986
			var_G = var_X * -0.9689 + var_Y *  1.8758 + var_Z *  0.0415
			var_B = var_X *  0.0557 + var_Y * -0.2040 + var_Z *  1.0570

			if (var_R > 0.0031308) var_R = 1.055 * (var_R ^ (1 / 2.4)) - 0.055
			else                     var_R = 12.92 * var_R
			if (var_G > 0.0031308) var_G = 1.055 * (var_G ^ (1 / 2.4)) - 0.055
			else                     var_G = 12.92 * var_G
			if (var_B > 0.0031308) var_B = 1.055 * (var_B ^ (1 / 2.4)) - 0.055
			else                     var_B = 12.92 * var_B

			sR = var_R * 255
			sG = var_G * 255
			sB = var_B * 255*/
		}

		double X = _xColorXYZ.x();
		double Y = _xColorXYZ.y();
		double Z = _xColorXYZ.z();
		double dR = (X * tmat[0][0]) + (Y * tmat[0][1]) + (Z * tmat[0][2]);
		double dG = (X * tmat[1][0]) + (Y * tmat[1][1]) + (Z * tmat[1][2]);
		double dB = (X * tmat[2][0]) + (Y * tmat[2][1]) + (Z * tmat[2][2]);

		//       Arbitrarily clip the luminance values to 100
		if (dR > 100) dR = 100;
		if (dG > 100) dB = 100;
		if (dB > 100) dB = 100;

		//       And scale
		dR /= 100;
		dG /= 100;
		dB /= 100;

		dR = pow(dR, 1.0 / c_gamma);
		dG = pow(dG, 1.0 / c_gamma);
		dB = pow(dB, 1.0 / c_gamma);

		double R = (int)(dR * 255);
		double G = (int)(dG * 255);
		double B = (int)(dB * 255);

		if ((R == 0) && (G == 0) && (B == 0)) {
			R = (unsigned char)7;
			G = (unsigned char)7;
			B = (unsigned char)7;
		}

		return QVector3D(R,G,B);

		double dVarX = _xColorXYZ.x() / 100.0;
		double dVarY = _xColorXYZ.y() / 100.0;
		double dVarZ = _xColorXYZ.z() / 100.0;
		double dVarR = dVarX*3.2406 + dVarY*-1.5372 + dVarZ*-0.4986;
		double dVarG = dVarX*-0.9689 + dVarY*1.8758 + dVarZ*0.0415;
		double dVarB = dVarX*0.0557 + dVarY*-0.2040 + dVarZ*1.0570;

		if (dVarR > 0.0031308)
			dVarR = 1.055 * qPow(dVarR, (1 / 2.4)) - 0.055;
		else
			dVarR = 12.92 * dVarR;
		if (dVarG > 0.0031308)
			dVarG = 1.055 * qPow(dVarG, (1 / 2.4)) - 0.055;
		else
			dVarG = 12.92 * dVarG;
		if (dVarB > 0.0031308)
			dVarB = 1.055 * qPow(dVarB, (1 / 2.4)) - 0.055;
		else
			dVarB = 12.92 * dVarB;

		return QVector3D(dVarR * 255, dVarG * 255, dVarB * 255);
	}

	QVector3D XColorTable::Yxy2sRGB(const QVector3D& _xCIEColorYxy)
	{
		QVector3D xColorXYZ = XColorTable::Yxy2XYZ(_xCIEColorYxy);
		return XColorTable::XYZ2sRGB(xColorXYZ);
	}

	const bool XColorTable::GetColor(const QString& _strKey, QColor& _xRefCol)
	{
		XColor xColor = XColor();
		if (m_xColorVec.contains(_strKey))
			xColor = m_xColorVec.value(_strKey);
		else
			return false;

		_xRefCol = QColor(xColor.sRGB.x(), xColor.sRGB.y(), xColor.sRGB.z());
		return true;
	}

}