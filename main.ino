#include<Servo.h>
#include <LiquidCrystal.h>
Servo myservo;
int sensor=5;//红外传感器的输入端，感应到人会输出HIGH，否则LOW
int t=0;//记录没有LOW的次数
int pos=90;//舵机的位置
int duoji=13;//控制舵机的引脚
int v=1;//舵机的转速
int LED=4;//LED灯用来显示人是否已经找到
int sensource=11;//舵机电源
int mv;//风扇转速
int motor=6;//控制电机的引脚
int button1=2;
int button2=3;
const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);//定义液晶屏对象
bool flag;//check状态
void setup() {
lcd.begin(16, 2);
  // put your setup code here, to run once:
pinMode(sensor,INPUT);
pinMode(button1,INPUT);
pinMode(button1,INPUT);
lcd.print("--auto running--");
myservo.attach(duoji);
myservo.write(45);
delay(5000);
myservo.write(pos);
delay(500);
find();//启动后，开始寻找人体
//attachInterrupt(0,change,RISING);
}
void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(button2)==LOW) {
    if (digitalRead(button1)==LOW) autorun();//三种模式 自动运行 默认状态
    else controlangle();
  }
  //手动控制舵机角度
  if (digitalRead(button2)==HIGH) controlspeed();//手动控制风扇转速
  
}
void find(){
  //analogWrite(sensource,255);
  //flag=check(pos);//在原先的pos基础上开始查找
  int pos2=pos;
  int sum=0;
  while (digitalRead(sensor)==LOW){//如果没有检测到人就让它一直寻找
    if (pos2==135||pos2==45)v=-v;
    pos2+=v;
    if (sum==19) allfind();//绕小角度后都没有人,开始全局查找
    sum=(sum+1)%20; 
    myservo.write(pos2);
    //Serial.print("body missed ");Serial.println(t);t++;
    delay(50);
    if (flag!=0) return;
  }
  pos=(pos+pos2)/2;
  myservo.write(pos);
  delay(500);
  v=-v;//速度反向方便下一次查找
  //analogWrite(sensource,0);
}
void allfind(){//全局查找
  bool a[91]={0};
    int save_pos=0;
    int high=0;
    myservo.write(45);
    delay(1000);
    for(int i=45;i<=135;++i){
      myservo.write(i);
      delay(100);//缓慢寻找，避免误差
      if(digitalRead(sensor)==HIGH){
        a[i-45]=1;
      }
    }
    for(int j=0;j<=90;++j){
      if(a[j]){
         save_pos+=(j+45);//记录位置
         high++;//记录high的次数
      }
    }
    pos=save_pos/high;//将平均值作为人所在位置
    myservo.write(pos);
}
void autorun(){
  lcd.setCursor(0,0);
  lcd.print("--auto running--");
  if (digitalRead(sensor)==HIGH){ 
  t=0;
  digitalWrite(LED,HIGH);
  int tmp=analogRead(A0);//热敏电阻读数大概在400-500 
  int tmp2=500-tmp;
  mv=map(tmp2,0,110,100,255);
  lcd.setCursor(0,1);
  lcd.print("speed:");
  lcd.print(mv);
  lcd.print("     ");
  analogWrite(motor,mv);
  delay(2000);
  }
  else {
    t++; digitalWrite(LED,LOW);
    if(t>5){
    //因为发现人体传感器检测的是人体的动作，因而长期不动无法HIGH
    //所以此处LOW一段时间后再次进入寻找状态，让舵机转动虚拟人体的运动
    //这样子还可以避免对着吹太久而导致人觉得冷
      analogWrite(motor,0);//进入查找前先让风扇停止
      find();
    }
  }
  delay(500);
}
void controlangle(){
   
    lcd.setCursor(0,0);
    lcd.print("--control angle- ");//手动控制舵机角度
    int tmp=analogRead(A1);
    int pos2=map(tmp,0,1023,0,180);//角度转换
    myservo.write(pos2);
    if (abs(pos2-pos)>30) delay(500);
    else delay(50);
    pos=pos2;
    lcd.setCursor(0,1);
    lcd.print("position:");
    lcd.print(pos2);//显示角度
     lcd.print("   ");
    
    tmp=analogRead(A0); 
    int tmp2=500-tmp;
    mv=map(tmp2,0,150,50,255);
    analogWrite(motor,mv);
}
void controlspeed(){
   lcd.setCursor(0,0);
   lcd.print("--control speed-");
   int tmp=analogRead(A1);
   mv=map(tmp,0,1023,0,255);
    lcd.setCursor(0,1);
    lcd.print("speed:");
    lcd.print(mv);//显示速度
    lcd.print("   ");
    analogWrite(motor,mv);
    delay(100);
}
