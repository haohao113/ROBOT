#include <stdlib.h>
#include <math.h>
#include "main.h"

vs16 IMU;

//=================================================
//--------------- Cac hang so ---------------------
//=================================================
int robotLimitSpeed = 254;  // Toc do toi da 
int i100 = 100, khoangCachGocFix = 200;
float fHeSoChinhHuong = 0.25, f05 = 0.5, f1 = 1.0, f2 = 2.0, f4 = 4.0, f5 = 5.0, f6 = 6.0, f10 = 10.0, f20 = 20.0, f30 = 30.0, f60 = 60.0, f120 = 120.0;

//fHeSoChinhHuong tang dan se giup robot tang luc chinh huong


//===================== Su dung cho ham gia toc bac 1 tuyen tinh ( PT tang toc ) ======================

// 2 tham so _robotY v� _robotX su dung cho ham gia toc Tang toc de tinh gia tri bien Y cua ham giaToc

// neu Y Cao thi toc do khoi dong ban dau se NHANH CHONG dat den van toc thiet lap
// neu Y thap thi toc do khoi dong ban dau se DAN DAN dat den van toc thiet lap 

//Tai X = 35
//_robotY = 20 va _robotX = 140 th� Y = 15 
//_robotY = 50 va _robotX = 150 th� Y = 38
//_robotY =  va _robotX =  th� Y = 
//_robotY =  va _robotX =  th� Y = 
//_robotY =  va _robotX =  th� Y = 

float _robotY = 50;
float _robotX = 150; 

//float _robotY = 50;		
//float _robotX = 150;	
											 
//====================================================================
//De giam toc DAN DAN

// Tang Dan so trong ham robotStop(0) => robotStop(10)
// Di kem khi tang so la Tang delay di kem de co du thoi gian giam toc.

// Vidu1 :	robotStop(0);
//			    delay_ms(100);  //dung ngay lap tuc

// Vidu2 :	robotStop(10);
//			    delay_ms(210);  //dung lai DAN DAN


// them vao do dieu chinh tham so _robotDelayGiamToc = 7; tang DAN de keo dai thoi gian giam toc

// Trong h�m robotAnalytics(); dieu chinh tang hoac giam 2 tham so de co the tang toc hoac giam toc ngay lap tuc.
//====================================================================

int motorLock = 2;
int motorLockRepeat = 1000;
float radian = 3.141592653589793238462643/180;
//=================================================
//------ Bien toan cuc de dieu khien ------
//=================================================
int _robotChange=0, _robotRotatePoint=0;
int _robotCurrentSpeed=0, _robotRunSpeed=0, _robotMaxSpeed=0;
int _robotAccStop=0, _robotCounter=0;
int _robotAngle=30000,_robotRunAngle=30000, _robotRotateAngle=0, _robotIMUInit=0;
float _robotRotate=0, _robotCurve=0;
int _robotAngleCounterFix=0, _robotAngleToFix=30000;

int _robotIMUAngle=0, _IMUAngle_Before=0, _robotDelayGiamToc = 7;
float _robotRun0h=0, _robotRun4h=0, _robotRun8h=0;

//==========================================================================================
//==========================================================================================
//==========================================================================================


void setMotor(int speed_0h, int speed_4h, int speed_8h)
{
	if(!speed_0h) mor_0h = motorLock;
 	else if(speed_0h > 0)
 	{
	 	mor_0h = speed_0h;
		mor_0h_next;
 	}else{
		mor_0h = -speed_0h;
		mor_0h_back;
 	}

	if(!speed_4h) mor_4h = motorLock;
 	else if(speed_4h > 0)
 	{
	 	mor_4h = speed_4h;
		mor_4h_next;
 	}else{
		mor_4h = -speed_4h;
		mor_4h_back;
 	}
 	
	if(!speed_8h) mor_8h = motorLock;
 	else if(speed_8h > 0)
 	{
	 	mor_8h = speed_8h;
		mor_8h_next;
 	}else{
		mor_8h = -speed_8h;
		mor_8h_back;
 	}
}
//------------------------------------------------------------------------------
void setAllMotor(int speed)
{
	mor_0h = speed;
	mor_4h = speed;
	mor_8h = speed;
}
//------------------------------------------------------------------------------
int robotIsStop()
{
    return (mor_0h < 3 && mor_4h < 3 && mor_8h < 3);
}
//------------------------------------------------------------------------------
int robotIsRun()
{
	return (_robotAngle == 30000)? 0: 1;
}
//------------------------------------------------------------------------------
void robotResetIMU(void)
{
	run_read_gyro_uart2();
}
//------------------------------------------------------------------------------
vs32 robotAngle(void)
{
	return -IMU;
}
//------------------------------------------------------------------------------
//==========================================================================================
//------------------------------------------------------------------------------
int roundF(float num)
{
	if(num < 0) return num - f05;
	else return num + f05;
}
//------------------------------------------------------------------------------
float fixMotorValue(float num)
{
	if(num == 0) 			return 0;
	else 	if(num < 0) return (num - f05)/_robotCurrentSpeed;
				else 				return (num + f05)/_robotCurrentSpeed;
}
//------------------------------------------------------------------------------
float absF(float num)
{
	if(num >= 0) return num;
	else return -num;
}
//------------------------------------------------------------------------------
int absI(int num)
{
	if(num >= 0) return num;
	else return -num;
}
//------------------------------------------------------------------------------
int giaToc(int x)
{
	// ------ gia toc bac 1 -----------
	float y = (-_robotY/_robotX)*x + _robotY;
	return (int)y;

}
//------------------------------------------------------------------------------
float maxF3(float N1, float N2, float N3)
{
	float _maxNum = 0.001;
	if(N1<0) N1 = -N1;
	if(N2<0) N2 = -N2;
	if(N3<0) N3 = -N3;

	if(N1>_maxNum) _maxNum = N1;
	if(N2>_maxNum) _maxNum = N2;
	if(N3>_maxNum) _maxNum = N3;

	return _maxNum;
}
//------------------------------------------------------------------------------
int robotSpeed()
{
    return _robotCurrentSpeed;
}
//------------------------------------------------------------------------------
int robotFixAngle()
{
		return _robotAngleCounterFix;
}
//------------------------------------------------------------------------------
//==========================================================================================
//------------------------------------------------------------------------------
void calculateMotor(float rotate)
{
		float angle;

		if(_robotAngle == 30000)// khi robot quay tai cho
		{
			_robotRun0h = _robotRun4h = _robotRun8h = 0;
		}
		else // khi robot vua chay vua quay
		{
			angle = (float)(_robotAngle - (_robotIMUAngle - _robotIMUInit) - _robotRotate * f30)/f10;
			
			_robotRun0h = -sin(radian *  angle        )*2 - _robotCurve;
			_robotRun4h =  sin(radian * (angle + f60) )*2;
			_robotRun8h = -sin(radian * (angle + f120))*2;
		}
		//----------------------------------------------
		if(rotate){
			switch(_robotRotatePoint)
			{
				case 0:
						_robotRun0h -= rotate;
						_robotRun4h -= rotate;
						_robotRun8h -= rotate;
						break;
				case 1:
						_robotRun4h -= rotate;
						_robotRun8h -= rotate;
						break;
				case 2:
						_robotRun0h -= rotate;
						_robotRun8h -= rotate;
						break;
				case 3:
						_robotRun0h -= rotate;
						_robotRun4h -= rotate;
						break;
			}
		}

		//----------- tinh toc do toi da ----------------
		_robotMaxSpeed = (int)(robotLimitSpeed/maxF3(_robotRun0h, _robotRun4h, _robotRun8h));
			if(_robotCurrentSpeed > _robotMaxSpeed) _robotCurrentSpeed = _robotMaxSpeed;
		//------------------------------------------------
		_robotChange = 1;
}
//------------------------------------------------------------------------------
void robotStop(unsigned int acc) // gia tri acc cang tang kem theo do la 1 khoang delay_ms vua du robot se dung cham dan
{																	// gia tri acc = 0 tuc robot se ngat xung ngay lap tuc khien banh bi tha troi g�y giat khuc
		_robotRunSpeed = 0;
		_robotAccStop = acc;

		if(!acc){
			_robotCurrentSpeed = 0;
			_robotCurve = 0;
			_robotAngle = 30000;

			_robotRotate = 0;
			_robotRotatePoint = 0;
			_robotRotateAngle = 0;
			_robotAngleCounterFix = 0;
			
			calculateMotor(0);
		}
}
//------------------------------------------------------------------------------
void robotRotateStop()
{
	if(_robotRotate || _robotRotatePoint || _robotRotateAngle)
    {
        _robotRotate = 0;
        _robotRotatePoint = 0;
        _robotRotateAngle = 0;

        _robotIMUInit = _robotIMUAngle;

        calculateMotor(_robotRotate);
    }
}
//------------------------------------------------------------------------------
void robotSetACC(float fX, float fY)
{
    _robotY = fY;
		_robotX = fX;
}
//------------------------------------------------------------------------------
void robotCurve(int angle, int maxSpeed, float curve)
{
    if(_robotAngle != angle || _robotRunSpeed != maxSpeed || _robotCurve != curve)
    {
        _robotRunSpeed = maxSpeed;
				if(_robotCurrentSpeed == 0) _robotCurrentSpeed = 3;
        _robotIMUInit = _robotIMUAngle;
        _robotCurve = curve;
        _robotAngle = angle;
        _robotCounter = 0;

        calculateMotor(_robotRotate);
    }
}
//------------------------------------------------------------------------------
void robotRotate(int rotateAngle, float rotate, int rotatePoint)
{
    if(_robotRotateAngle != rotateAngle || _robotRotate != rotate || _robotRotatePoint != rotatePoint)
    {
        if(_robotAngle == 30000)
        {
            _robotRunSpeed = 77;
            _robotCurrentSpeed = 10;

            if(rotateAngle < 30000){// Quay theo goc no do
							_robotAngleCounterFix = 1;
							if((rotate > 0 && _robotIMUAngle > rotateAngle) || (rotate < 0 && _robotIMUAngle < rotateAngle )) rotate = - rotate;        
						}
				}
        _robotRotateAngle = rotateAngle;
        _robotRotatePoint = rotatePoint;
        _robotRotate = rotate;
        _robotCounter = 0;
        

        calculateMotor(_robotRotate);
    }
}
//------------------------------------------------------------------------------
void robotRotateFree(float rotate, int rotatePoint)
{
	robotRotate(30000, rotate, rotatePoint);
}
//------------------------------------------------------------------------------		
void robotRun(int angle, int maxSpeed)
{
	robotCurve(angle, maxSpeed, 0);
}
//------------------------------------------------------------------------------
void robotRunAngle(float angle, int maxSpeed, float robotAngle, float rotate)
{
	if (_robotRunAngle != angle || _robotRunSpeed != maxSpeed || _robotRotateAngle != robotAngle)
	{
		_robotRunAngle = angle;
		robotCurve(angle - _robotIMUAngle, maxSpeed, 0);
		robotRotate(robotAngle, (abs(robotAngle - _robotIMUAngle) > 100 ? (robotAngle > _robotIMUAngle ? rotate : -rotate) : (robotAngle - _robotIMUAngle)*0.003), 0);
	}
}

//------------------------------------------------------------------------------

void robotAngleAnalytics(void)
{
    if(_robotAngleCounterFix > 1)
    {
    	if(absI(_robotRotateAngle - _robotIMUAngle) < 5){
	    	if(_robotAngleCounterFix++ > 500)
				{
						robotStop(0);
						return;
				}
	    }else{
				
					if(_robotAngleCounterFix > 250){
						_robotRunSpeed = 4;
						_robotCurrentSpeed = 4;
						_robotAngleCounterFix = 251;
					}else{
						_robotRunSpeed = 5;
						_robotCurrentSpeed = 5;
					}
					
	        if(_robotIMUAngle > _robotRotateAngle) _robotRotate = -1;
	        else _robotRotate = 1;
				
					calculateMotor(_robotRotate);
    	}
    }
}

//-----------------------------------------------------------------

void robotDirectionAnalytics()
{
    // Khi chay duong cong hoac khong chay hoac quay tu do thi thoat
    if(_robotCurve || !_robotCurrentSpeed || _robotRotateAngle >= 30000) return;
    // Khi co lenh xoay
    if(_robotRotate)
    {
        // Khi chi xoay tai cho
        if(_robotAngle == 30000)
        {
            if((_robotRotate > 0 && _robotIMUAngle > _robotRotateAngle - khoangCachGocFix) ||
               (_robotRotate < 0 && _robotIMUAngle < _robotRotateAngle + khoangCachGocFix)) 
						{
									if(_robotAngleCounterFix < 2){
												// Khi dung goc thi dung robot, khoa banh xe, va bat dau Fix Angle
												if(absI(_robotRotateAngle - _robotIMUAngle) < 10)
												{
														_robotAngleToFix = _robotRotateAngle;
														_robotCurrentSpeed = 0;
														_robotRunSpeed = 0;
														_robotRotate = 0;
														_robotAngleCounterFix = 2;

														calculateMotor(_robotRotate);

														return;
												}else{
													if(_robotRotateAngle > _robotIMUAngle)
														calculateMotor((float)(f6 + absF(_robotCurrentSpeed*_robotRotate/khoangCachGocFix)*absI(_robotRotateAngle - _robotIMUAngle))/_robotCurrentSpeed);
													else
														calculateMotor(-(float)(f6 + absF(_robotCurrentSpeed*_robotRotate/khoangCachGocFix)*absI(_robotRotateAngle - _robotIMUAngle))/_robotCurrentSpeed);
												}
									}
            }else calculateMotor(_robotRotate);
        }
        else
        {// Vua chay thang vua xoay
            if((_robotRotate > 0 && _robotIMUAngle > _robotRotateAngle - _robotCurrentSpeed*_robotRotate - i100) ||
               (_robotRotate < 0 && _robotIMUAngle < _robotRotateAngle - _robotCurrentSpeed*_robotRotate + i100))
            {
								calculateMotor(fixMotorValue(0.2 * (_robotRotateAngle - _robotIMUAngle)));
            }
            else calculateMotor(_robotRotate);//---- Neu chua dat den goc can quay ----------
        }
        
        return;
    }

    // Khi chay thang	
    calculateMotor(fixMotorValue(fHeSoChinhHuong * (_robotIMUInit - _robotIMUAngle)));
}

//==========================================================================================
void ac_Stop(void)  // giam toc de dung
{
		if(_robotAngleCounterFix < 2){
			_robotCurve = 0;
			_robotRotate = 0;
			_robotRotatePoint = 0;
			_robotAngle = 30000;
			_robotRotateAngle = 0;
    }
		//-------------------------------------
		if(++_robotCounter < motorLockRepeat) setAllMotor(motorLock);
		else {
			_robotCounter = motorLockRepeat;
			setAllMotor(0);
		}
}
//------------------------------------------------------------------------------
void ac_SpeedACC(int acc, int n)  // Gia toc tang toc. n c�ng tang thi gia toc ban dau cang nhanh.
{																	// n am thi la giam toc, gia toc giam dan, do am cang lon giam toc cang nhanh.
	if(++_robotCounter > acc)
    {
        _robotCurrentSpeed += n; // mac dinh

        _robotCounter = 0;
    }
}
//------------------------------------------------------------------------------
void robotAnalytics(void)//-------- Dat trong Interup Timer ---------
{
		_robotIMUAngle = robotAngle();
	
    //======= Fix goc robot ==========
		robotAngleAnalytics();
	
    //======= Gia toc di chuyen ==========
    if(_robotRunSpeed > 0)// Robot dang duoc ra lenh chay
    {
        if(_robotCurrentSpeed != _robotRunSpeed)
        {
            if(_robotCurrentSpeed < _robotRunSpeed)
            {
            	if(_robotCurrentSpeed < _robotMaxSpeed) ac_SpeedACC(giaToc(_robotCurrentSpeed), 1); //--- Phuong trinh Gia toc --------	
            }
						else ac_SpeedACC(_robotDelayGiamToc, -2); //--- Phuong trinh Giam toc --------
        }
    }
    else  //-------- Robot duoc lenh Stop -----------
        if(_robotCurrentSpeed > 0) ac_SpeedACC(_robotAccStop, -1); //-------- Giam toc do -----------
				else{
					ac_Stop();
					return;
				}
		//======= Fix robot theo IMU ==========
		if(_IMUAngle_Before != _robotIMUAngle){		
				robotDirectionAnalytics();
				_IMUAngle_Before = _robotIMUAngle;
		}
		
    //======= Thiet lap thong so dong co ==========
    if(_robotChange) setMotor(roundF(_robotCurrentSpeed * _robotRun0h), roundF(_robotCurrentSpeed * _robotRun4h), roundF(_robotCurrentSpeed * _robotRun8h));
}

//==========================================================================================
//==========================================================================================
//==========================================================================================

