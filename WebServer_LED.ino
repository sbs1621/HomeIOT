#include <SPI.h>
#include <Ethernet.h>
#include<Servo.h>
//맥 주소와 IP 주소 입력 (IP주소는 로컬 네트워크 환경에 따라 다를 수 있음)
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192,168,0,71);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress myDns (8, 8, 8, 8);

//사용하려는 IP 주소와 포트 초기화 (HTTP 포트 기본 값 : 80)
EthernetServer server(80);
EthernetClient client;
//LED 변수 선언 및 핀 번호 저장
Servo myServo;
void setup() {
  //LED 핀 모드 설정
  myServo.attach(6,500,2300);
  pinMode(7,OUTPUT);//GREEN
  pinMode(8,OUTPUT);//BLUE
  pinMode(2,OUTPUT);//RED
  // 시리얼 모니터 설정
  pinMode(4,OUTPUT);
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Ethernet WebServer Example");

  // 이더넷 연결 및 서버 시작
  Ethernet.begin(mac, ip, myDns, gateway, subnet);

  // 이더넷 하드웨어 확인 (없을 경우 아래 조건문 실행)
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // 서버 시작
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  // 클라이언트 객체 선언 및 수신
  client = server.available();
  if (client) {
    Serial.println("new client");
    // http 요청은 빈줄로 끝남
    boolean currentLineIsBlank = true;
    String buffer = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        buffer += c;
        Serial.write(c);
        // 클라이언트에서 읽은 내용이 끝나면 개행문자 \n을 받아서 해당 줄이 비어 있으면
        // http 요청이 끝났으므로 응답을 보낼 수 있음.
        if (c == '\n' && currentLineIsBlank) {
          setHtml();
          break;
        }
        if (c == '\n') {
          // 새로운 라인 시작
          currentLineIsBlank = true;
        } else if (c != '\r') {
          if(buffer.indexOf("GET /led?pinD8=on") >= 0){
            digitalWrite(2,HIGH);
            digitalWrite(7,HIGH);
            digitalWrite(8,HIGH);
          }else if(buffer.indexOf("GET /led?pinD8=off") >= 0){
            digitalWrite(2,LOW);
            digitalWrite(7,LOW);
            digitalWrite(8,LOW);
          }/*else if(buffer.indexOf("GET /servo?pinD6=on") >= 0){
            myServo.write(180);
          }else if(buffer.indexOf("GET /servo?pinD6=off") >= 0){
            myServo.write(0);
          }*/else if(buffer.indexOf("GET /ac?pinD4=on") >=0){
            digitalWrite(4,HIGH);
          }else if(buffer.indexOf("GET /ac?pinD4=off") >=0){
            digitalWrite(4,LOW);
          }
          currentLineIsBlank = false;
        }
      }
    }
    
    // 웹 브라우저가 데이터를 받을 시간을 부여
    delay(1);
    // 연결 종료
    client.stop();
    Serial.println("client disconnected");
  }
  
}

void makeLed(){
  client.print("<center>");
  client.print("<form action='/led' method='GET'>");
  client.print("<input type='hidden' name='pinD8' value='on'>");
  client.print("<input class='onBtn' type='submit' value='on'/>");
  client.print("</form>");
  client.print("<form action='/led' method='GET'>");
  client.print("<input type='hidden' name='pinD8' value='off'>");
  client.print("<input class='offBtn' type='submit' value='off'/>");
  client.print("</center>"); 
  client.print("</form>"); 
}
void makeServo(){
  client.print("<center>");
  client.print("<form action='/servo' method='GET'>");
  client.print("<input type='hidden' name='pinD6' value='on'>");
  client.print("<input class='onBtn' type='submit' value='on'/>");
  client.print("</form>");
  client.print("<form action='/servo' method='GET'>");
  client.print("<input type='hidden' name='pinD6' value='off'>");
  client.print("<input class='offBtn' type='submit' value='off'/>"); 
  client.print("</center>"); 
  client.print("</form>");
}
void makeAirConditioner(){
  client.print("<center>");
  client.print("<form action='/ac' method='GET'>");
  client.print("<input type='hidden' name='pinD4' value='on'>");
  client.print("<input class='onBtn' type='submit' value='on'/>");
  client.print("</form>");
  client.print("<form action='/ac' method='GET'>");
  client.print("<input type='hidden' name='pinD4' value='off'>");
  client.print("<input class='offBtn' type='submit' value='off'/>"); 
  client.print("</center>"); 
  client.print("</form>");
}
void setHtml(){
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println();
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<center>");
  client.println("<title>Arduino for IoT</title>");
  client.println("<head>");
  client.println("<style>");
  client.println("h1{color:#17A1A5; font-size:48px;}");
  client.println("h2{color:#17A1A5; font-size:30px;}");
  client.println(".box{border:solid 5px #4db7bb; width:200px; height: 300px;}");
  client.println("input{width:40%; height: 100px; font-size:40px; color:#fff; border:none; background: #17A1A5;}");
  client.println(".onBtn{margin:40px 0 20px 0;}");
  client.println("</style>");
  client.println("</head>");
  client.println("<body>");
  client.println("");
  client.println("<h1>Home Arduino</h1>");
  client.println("<br><br><br>");
  client.println("<h2>LED</h2>");
  makeLed(); 
  client.println("<h2>Curtain</h2>");        
  makeServo();
  client.println("<h2>AirConditioner</h2>");
  makeAirConditioner();
  client.println("<br />");
  client.println("</center>");
  client.println("</body>");
  client.println("</html>");
}
