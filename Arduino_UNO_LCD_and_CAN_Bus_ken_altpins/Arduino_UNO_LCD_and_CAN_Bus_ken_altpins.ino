// Works

// Prints text in assorted sizes and places to work out best screen layouts.


//#define LCD_ST7789 1 // Use this 240x240 kind of display, not the SSD1306 one

/*

    Pro Mini pins...

        PORTB 0 = D8        PORTC 0 = A0    PORTD 0 = D0(RX)
        PORTB 1 = D9        PORTC 1 = A1    PORTD 1 = D1(TX)
        PORTB 2 = D10(ss)   PORTC 2 = A2    PORTD 2 = D2
        PORTB 3 = D11(mosi) PORTC 3 = A3    PORTD 3 = D3
        PORTB 4 = D12(miso) PORTC 4 = A4    PORTD 4 = D4
        PORTB 5 = D13(sck)  PORTC 5 = A5    PORTD 5 = D5
        PORTB 6 = nc        PORTC 6 = RST   PORTD 6 = D6
        PORTB 7 = nc        PORTC 7 = A0    PORTD 7 = D7

  See http://www.rjhcoding.com/avrc-spi.php
 */

#include <SerialID.h>	// See https://github.com/gitcnd/SerialID
SerialIDset("\n#\tv2.06 " __FILE__ "\t" __DATE__ " " __TIME__);// So we know what code and version is running inside our MCUs


#include <Canbus.h>
#include <defaults.h>
#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>


#ifdef LCD_ST7789 // 240x240 TFT
  #include <Adafruit_GFX.h>    // Core graphics library by Adafruit
  #include <Arduino_ST7789.h> // Hardware-specific library for ST7789 (with or without CS pin)
  #define TFT_DC    10 // was 8
  #define TFT_RST   9 
  //#define TFT_CS    10 // only for displays with CS pin
  #define TFT_MOSI  11   // for hardware SPI data pin (all of available pins) - goes to SDA
  #define TFT_SCLK  13   // for hardware SPI sclk pin (all of available pins) - got to SCK
  //Arduino_ST7789 tft = Arduino_ST7789(TFT_DC, TFT_RST); //for display without CS pin
  //Arduino_ST7789 tft = Arduino_ST7789(TFT_DC, TFT_RST, TFT_CS); //for display with CS pin
  Arduino_ST7789 mylcd = Arduino_ST7789(TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK); //for display without CS pin
  //Arduino_ST7789 tft = Arduino_ST7789(TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_CS); //for display with CS pin
  #define max_x 240
  #define max_y 240
  #define min_sz 2	// text scale factor

#else // SSD1283A 130x130 transflective LCD display
  #include "LCDWIKI_GUI.h" //Core graphics library
  #include "LCDWIKI_SPI.h" //Hardware-specific library
  #define MODEL SSD1283A
  #define CS   8 // was 2
  #define CD   7 // Data command pin
  #define SDA  3 // A2   //if you use the hardware spi,this pin is no need to set - MOSI
  #define MOSI SDA
  #define MISO 7	// unused
  #define SCK  5 // A1   //if you use the hardware spi,this pin is no need to set
  #define RST  4 	// needed (does no work without)//was9
  #define LED  -1   //if you don't need to control the LED pin,you should set it to -1 and set it to 3.3V
  #define CND_LED 6 // pwmable backlight

//#define CND_SPI_HW 1 // SEE ALSO LCDWIKI_SPI and the main .INO file
#ifdef CND_SPI_HW
  // hardware SPI: 
  #define SPI_DDR     DDRB  
  #define SPI_CS      PINB2 
  #define SPI_MOSI    PINB3
  #define SPI_MISO    PINB4
  #define SPI_SCK     PINB5
  #define SPI_INIT    SPI_DDR |= (1 << SPI_CS) | (1 << SPI_MOSI) | (1 << SPI_SCK); // set CS, MOSI and SCK to output
  #define SPI_INIT2   SPCR = (1 << SPE) | (1 << MSTR) | (0 << SPR1) | (0 << SPR0); SPSR|=(1<<SPI2X); 
  #define SPI_SEND(data) while(!(SPSR & (1 << SPIF))); SPDR = data;
#endif

  
//ok 
LCDWIKI_SPI mylcd(MODEL,CS,CD, MISO, MOSI, RST, SCK, LED); //LCDWIKI_SPI mylcd(MODEL,CS,CD,RST,LED); 
//ng  LCDWIKI_SPI mylcd(MODEL,CS,CD, RST, LED); //LCDWIKI_SPI mylcd(MODEL,CS,CD,RST,LED); 
//LCDWIKI_SPI::LCDWIKI_SPI(uint16_t model,int8_t cs, int8_t cd, int8_t reset, int8_t led)

//LCDWIKI_SPI::LCDWIKI_SPI(uint16_t model,int8_t cs, int8_t cd, int8_t reset, int8_t led)
//LCDWIKI_SPI::LCDWIKI_SPI(int16_t wid,int16_t heg,int8_t cs, int8_t cd, int8_t miso, int8_t mosi, int8_t reset, int8_t clk, int8_t led)
//LCDWIKI_SPI::LCDWIKI_SPI(uint16_t model,int8_t cs, int8_t cd, int8_t miso, int8_t mosi, int8_t reset, int8_t clk, int8_t led)
//LCDWIKI_SPI::LCDWIKI_SPI(int16_t wid,int16_t heg,int8_t cs, int8_t cd, int8_t reset,int8_t led)

//ng LCDWIKI_SPI mylcd(130,130,CS, CD, RST,LED)
//ng LCDWIKI_SPI mylcd(130,130,CS, CD, MISO, MOSI, RST, SCK, LED);
  
  #define fillScreen Fill_Screen
  //Set_Text_Back_colour
  #define setTextColor Set_Text_colour
  #define setTextSize Set_Text_Size
  //mylcd.Print_String("Hello World!", 0, 0);
  //mylcd.Print_Number_Float(01234.56789, 2, 0, 8, '.', 0, ' ');
  //mylcd.Print_Number_Int(0xDEADBEF, 0, 16, 0, ' ',16);
  //mylcd.Set_Rotation(rotation);
  #define max_x 130
  #define max_y 130
  #define min_sz 1	// text scale factor
#endif

#include <SPI.h>
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define FONT_W	6
#define FONT_H	8
#define FONT_SP 1

// #ifdef TEENSYDUINO
// #ifdef ARDUINO_ESP8266_NODEMCU // Wemos_D1_ESP8266_NodeMCU_1
// #ifdef ARDUINO_AVR_PRO	// Arduino_Pro_Mini33

void textPrint(int x, int y, int colfg, int colbg, int tsz, char *text) {
  mylcd.setTextColor(colfg);
  mylcd.setTextSize(tsz);
#ifdef LCD_ST7789
  mylcd.setCursor(x, y);
  mylcd.setTextWrap(true);
  mylcd.print(text);
#else
  mylcd.Set_Text_Back_colour(colbg);
  mylcd.Print_String(text, x, y);
#endif
} // textPrint

void textPrint(int x, int y, int colfg, int colbg, int tsz, int text) { // Font is 6px wide x 8px high; and is multpilied by the "size" (eg tsz 3=18x24)
  mylcd.setTextColor(colfg);
  mylcd.setTextSize(tsz);
#ifdef LCD_ST7789
  mylcd.setCursor(x, y);
  mylcd.setTextWrap(true);
  mylcd.print(text);
#else
  mylcd.Set_Text_Back_colour(colbg);
  mylcd.Print_Number_Int(text, x, y, 0, ' ',10);
#endif
} // textPrint

// H and W are text lines, not pixels, below:
void textPrintL(int x, int y, int colfg, int colbg, int tsz, char *text) {
  tsz+=(min_sz-1);				// Scale to match the resolution
  x=x*FONT_W*tsz; y=y*(FONT_H+FONT_SP)*tsz;	// Convert text location into pixel location
  mylcd.setTextColor(colfg);
  mylcd.setTextSize(tsz);
#ifdef LCD_ST7789
  mylcd.setCursor(x, y);
  mylcd.setTextWrap(true);
  mylcd.print(text);
#else
  mylcd.Set_Text_Back_colour(colbg);
  mylcd.Print_String(text, x, y);
#endif
} // textPrint

void textPrintL(int x, int y, int colfg, int colbg, int tsz, int text) {
  tsz+=(min_sz-1);				// Scale to match the resolution
  x=x*FONT_W*tsz; y=y*(FONT_H+FONT_SP)*tsz;	// Convert text location into pixel location
  mylcd.setTextColor(colfg);
  mylcd.setTextSize(tsz);
#ifdef LCD_ST7789
  mylcd.setCursor(x, y);
  mylcd.setTextWrap(true);
  mylcd.print(text);
#else
  mylcd.Set_Text_Back_colour(colbg);
  mylcd.Print_Number_Int(text, x, y, 0, ' ',10);
#endif
} // textPrint

int h=0;
int w=0;
int backl=0;
int line=0;
  uint16_t time;

void setup(void) {
  int canok;
  SerialIDshow(115200); // starts Serial.
  
  pinMode(CND_LED,OUTPUT); // Backlight LED
  digitalWrite(CND_LED,HIGH); // Turn backlight full on (can PWM this pin for less brightness)

#ifdef CND_SPI_HW
  SPI_INIT
  SPI_INIT2
#endif

   pinMode(CS,OUTPUT);pinMode(SDA,OUTPUT);pinMode(SCK,OUTPUT);pinMode(RST,OUTPUT);pinMode(CND_LED,OUTPUT);pinMode(CD,OUTPUT);

  canok=Canbus.init(CANSPEED_250);

#ifdef LCD_ST7789
  mylcd.init(240, 240);   // initialize a ST7789 chip, 240x240 pixels
#else
  mylcd.Init_LCD();
#endif

  Serial.println("Initialized");
  textPrint(10,50,BLUE,BLACK,4,"Ready");

  time = millis();
  mylcd.fillScreen(BLACK);
  time = millis() - time;

  Serial.print("Clear screen took: ");
  Serial.println(time, DEC);
  //delay(500);
  time = millis();

  textPrint(0,30,WHITE,BLACK,3,"CAN Init");
  if(canok) textPrint(20,60,GREEN,BLACK,3,"OK");
  else      textPrint(20,60,RED,BLACK,3,"Err");

/*
  if(Canbus.init(CANSPEED_125)) { //Initialise MCP2515 CAN controller at the specified speed
    //Serial.println("CAN Init ok");
    //textPrint(1,1,GREEN,BLACK,2,"CAN OK"); // print
  } else {
    //Serial.println("Can't init CAN");
    //textPrint(1,1,RED,BLACK,2,"CAN ERR"); // print
  }  
*/


  // large block of text
//  testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", WHITE);
//  delay(1000);

  // tft print function
  // tftPrintTest();
//  delay(4000);

  w=6;h=8;
  textPrint(0, line,YELLOW,BLACK,1,"1M/4567890123456789012345678901234567890"); line+=h;	//1 does 21.5 x 16.25 chars on 130px, 40chrs on 240px = 6pxW,8pxH
  textPrint(0, line,BLUE,BLACK,1,"0y       1         2         3         4"); line+=h;
  int ol=line;

  w=12;h=16;
  textPrint(0,line,RED,BLACK,2,"0y       1         2"); line+=h;
  textPrint(0,line,RED,BLACK,2,"1M/45678901234567890"); line+=h;				//2 does ? chars on 130px, 20chrs on 240px = 12pxW,16pxH

  w=18;h=24;
  textPrint(0,line, YELLOW,BLACK,3,"0y       1   "); line+=h;
  textPrint(0,line,YELLOW,BLACK,3,"1M/4567890123"); line+=h;				//3 does ? chars on 130px, 13chrs on 240px = ?pxW,?pxH

  w=24;h=32;
  textPrint(0,line,CYAN,BLACK,4,"0y       1"); line+=h;
  textPrint(0,line,CYAN,BLACK,4,"1M/4567890"); line+=h; 					//4 does ? chars on 130px, 10chrs on 240px = 24pxW,32pxH

  w=35;h=40;
  textPrint(0,line,MAGENTA,BLACK,5,"12345678"); line+=h;					//5 does ? chars on 130px, 8chrs on 240px = ?pxW,?pxH
  textPrint(0,line,WHITE,BLACK,6,"12345678"); line+=h;					//5 does ? chars on 130px, 8chrs on 240px = ?pxW,?pxH

  w=6;h=8;line=ol;
  textPrint(0, line,YELLOW,BLACK,1,"3."); line+=h;
  textPrint(0, line,YELLOW,BLACK,1,"4."); line+=h;
  textPrint(0, line,YELLOW,BLACK,1,"5."); line+=h;
  textPrint(0, line,YELLOW,BLACK,1,"6."); line+=h;
  textPrint(0, line,YELLOW,BLACK,1,"7."); line+=h;
  textPrint(0, line,YELLOW,BLACK,1,"8."); line+=h;
  textPrint(0, line,YELLOW,BLACK,1,"9."); line+=h;
  textPrint(0, line,YELLOW,BLACK,1,"01."); line+=h;
  textPrint(0, line,YELLOW,BLACK,1,"11."); line+=h;
  textPrint(0, line,YELLOW,BLACK,1,"21."); line+=h;
  textPrint(0, line,YELLOW,BLACK,1,"31."); line+=h;
  textPrint(0, line,YELLOW,BLACK,1,"41."); line+=h;
  textPrint(0, line,YELLOW,BLACK,1,"51."); line+=h;
  textPrint(0, line,YELLOW,BLACK,1,"61."); line+=h;

}

int i=0;
void loop() {
  tCAN message;
  char buf[50]; // to assemble a big message
  char one[2];  // to output it one-byte-a-a-time
  int X=0;
  int Y=0;

#ifdef LCD_ST7789 // 240x240 TFT
  mylcd.invertDisplay(true);
  delay(500);
  mylcd.invertDisplay(false);
  delay(500);
#endif

//   textPrintL((max_x/(FONT_W*((mysz-1)+min_sz)))/2, (max_y/((FONT_H+FONT_SP)*((mysz-1)+min_sz)))/2,BLACK,BLACK,mysz,i); // erase

  textPrint(max_x/2-(max_x/2*0.1),max_y/2-(max_y/2*0.1),BLACK,BLACK,2,i); // erase
  i++;
  textPrint(max_x/2-(max_x/2*0.1),max_y/2-(max_y/2*0.1),GREEN,BLACK,2,i); // print
  if(i==100) {
    time = millis() - time;
    Serial.print("100 took: ");
    Serial.println(time, DEC);
    /*
    i=0;  time = millis();
    backl++;
    if(backl%3==0) {
       pinMode(CND_LED,OUTPUT);
       digitalWrite(CND_LED,LOW); // Turns off backlight
       Serial.println("Low LED");
    } else if(backl%3==1) {
       pinMode(CND_LED,OUTPUT);
       digitalWrite(CND_LED,HIGH);
       Serial.println("HIGH LED");
    } else {
       pinMode(CND_LED,INPUT); // disconnect
       Serial.println("Open LED");
  } */
  //delay(500);

/*
    // Test screen writing: OK
    while(1) { // We now start printing CAN to LCD forever...
          sprintf(buf,"ID:%4X L:%d ", 1234,42);
          one[1]=0;
          for(int i=0;i<strlen(buf);i++) { // Send data to LCD, wrapping as needed
            one[0]=" "; textPrint(X,Y,BLACK,BLACK,1,one);     // erase
            one[0]=buf[i]; textPrint(X,Y,YELLOW,BLACK,1,one); // write char
            X=X+6;  if(X>=130) { X=0; Y=Y+8; if(Y>=130)Y=0;}  // advance
          }  
    }
*/

    while(1) { // We now start printing CAN to LCD forever...
      if (mcp2515_check_message())   {
        if (mcp2515_get_message(&message)) {
          //if(message.id == 0x620 and message.data[2] == 0xFF) { //uncomment when you want to filter
          sprintf(buf,"ID:%4X L:%d ", message.id, message.header.length);
               
               Serial.print("ID: ");
               Serial.print(message.id,HEX);
               Serial.print(", ");
               Serial.print("Data: ");
               Serial.print(message.header.length,DEC);
               for(int i=0;i<message.header.length;i++) 
                { 
		  if(strlen(buf)<46) sprintf(&buf[strlen(buf)],"%2X ", message.data[i]);
                  Serial.print(message.data[i],HEX);
                  Serial.print(" ");
                }
               Serial.println(buf);
             //}

	  one[1]=0;
	  for(int i=0;i<strlen(buf);i++) { // Send data to LCD, wrapping as needed
	    one[0]=" ";    textPrint(X,Y,BLACK,BLACK,1,one);     // erase
	    one[0]=buf[i]; textPrint(X,Y,YELLOW,BLACK,1,one); // write char
            X=X+6;  if(X>=130) { X=0; Y=Y+8; if(Y>=130)Y=0;}  // advance
	  }	  

        }
      } else { //mcp2515_check_message
        textPrint(max_x/2-(max_x/2*0.1),max_y/2-(max_y/2*0.1),BLACK,BLACK,2,i); // erase
        i++;
        textPrint(max_x/2-(max_x/2*0.1),max_y/2-(max_y/2*0.1),WHITE,BLACK,2,i); // print

      }
    } // while forever

  } // i=400
  
} //loop


void testdrawtext(char *text, uint16_t color) {
  // mylcd.setCursor(0, 0);
  mylcd.setTextColor(color);
  //mylcd.setTextWrap(true);
  //mylcd.print(text);
}


/*
void tftPrintTest() {
  mylcd.setTextWrap(false);
  mylcd.fillScreen(BLACK);
  // mylcd.setCursor(0, 30);
  mylcd.setTextColor(RED);
  mylcd.setTextSize(1);
  mylcd.println("Hello World!");
  mylcd.setTextColor(YELLOW);
  mylcd.setTextSize(2);
  mylcd.println("Hello World!");
  mylcd.setTextColor(GREEN);
  mylcd.setTextSize(3);
  mylcd.println("Hello World!");
  mylcd.setTextColor(BLUE);
  mylcd.setTextSize(4);
  mylcd.print(1234.567);
  delay(1500);
  // mylcd.setCursor(0, 0);
  mylcd.fillScreen(BLACK);
  mylcd.setTextColor(WHITE);
  mylcd.setTextSize(0);
  mylcd.println("Hello World!");
  mylcd.setTextSize(1);
  mylcd.setTextColor(GREEN);
  //mylcd.print(p, 6);
  mylcd.println(" Want pi?");
  mylcd.println(" ");
  mylcd.print(8675309, HEX); // print 8,675,309 out in HEX!
  mylcd.println(" Print HEX!");
  mylcd.println(" ");
  mylcd.setTextColor(WHITE);
  mylcd.println("Sketch has been");
  mylcd.println("running for: ");
  mylcd.setTextColor(MAGENTA);
  mylcd.print(millis() / 1000);
  mylcd.setTextColor(WHITE);
  mylcd.print(" seconds.");
}
*/
