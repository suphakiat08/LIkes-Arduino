#include <GxEPD.h>
#include <GxGDEW027C44/GxGDEW027C44.cpp>
#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>

#include <Fonts/AGENCYR32pt7b.h> // for price
#include <Fonts/AGENCYR21pt7b.h> // for price small
#include <Fonts/AGENCYR18pt7b.h> // for product name
#include <Fonts/AGENCYR17pt7b.h> // for serial number
#include <Fonts/AGENCYR16pt7b.h> // for likes, share
#include <Fonts/AGENCYR12pt7b.h> // for string footer
#include <Fonts/AGENCYR10pt7b.h> // for sale promotion
#include <Fonts/AGENCYR7pt7b.h> // for Offer ends and discounts
#include "image.c"

GxIO_Class io(SPI, 15, 4, 5); // arbitrary selection of D3(=0), D4(=2), selected for default of GxEPD_Class
GxEPD_Class display(io, 5, 16); // default selection of D4(=2), D2(=4)

// show serial number
void display_init(char* serial_char) {
  // init display
  display.init();

  // set background
  display.fillScreen(GxEPD_WHITE);
  display.setRotation(3);

  // serial number
  display.setTextColor(GxEPD_RED);
  display.setFont(&AGENCYR17pt7b);
  display.setCursor(25, 100);
  display.println(serial_char);

  // string footer
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&AGENCYR12pt7b);
  display.setCursor(25, 125);
  display.println("SERIAL NUMBER");

  display.update();
}

// display 1 no option
void display_page(char* prod_name, char* price, char* likes, char* shares, String* promotion, boolean* option) {
  // set background
  display.fillScreen(GxEPD_RED);
  display.fillRect(0, 105, 264, 75, GxEPD_WHITE);
  display.setRotation(3);

  // product name
  display.setTextColor(GxEPD_WHITE);
  display.setFont(&AGENCYR18pt7b);
  display.setCursor(15, 35);
  display.println(prod_name);

  display.setFont(&AGENCYR16pt7b);
  
  if (!option[0] && !option[1] && !option[2]) {
    // box left
    display.drawBitmap(box, 15, 48, 114, 48, GxEPD_RED);
    // box right  
    display.drawBitmap(box, 135, 48, 114, 48, GxEPD_RED);
    // likes icon
    display.drawBitmap(likes_icons, 30, 59, 27, 27, GxEPD_BLACK);
    // likes count
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(75, 84);
    display.println(likes);
  
    // share icon
    display.drawBitmap(share_icons, 150, 59, 27, 27, GxEPD_BLACK);
    // share count
    display.setCursor(195, 84);
    display.println(shares);
    
  } else {
    int count_option = 0;
    int position[2][2] = {
      {18, 48},
      {75, 48}
    };
    display.setTextColor(GxEPD_RED);

    if((option[0] && !option[1] && !option[2]) || (!option[0] && option[1] && !option[2]) || 
          (!option[0] && !option[1] && option[2])) {
      if(option[0]) {
        display.drawBitmap(hot_sale, position[0][0], position[0][1], 48, 48, GxEPD_RED);
      } else if(option[1]) {
        display.drawBitmap(limited_icons, position[0][0], position[0][1], 48, 48, GxEPD_RED);
      } else {
        display.drawBitmap(cleance_icons, position[0][0], position[0][1], 48, 48, GxEPD_RED);
      }

      // box right  
      display.drawBitmap(box, 72, 48, 114, 48, GxEPD_RED);
      display.drawBitmap(box, 134, 48, 114, 48, GxEPD_RED);
      display.fillRect(130, 48, 5, 48, GxEPD_WHITE);
      // likes icon
      display.drawBitmap(likes_icons, 117, 59, 27, 27, GxEPD_BLACK);
      // likes count
      display.setCursor(162, 84);
      
    } else {
      if(option[0]) {
        display.drawBitmap(hot_sale, position[1][0], position[1][1], 48, 48, GxEPD_RED);
        count_option++;
      }
      
      if(option[1]) {
        if(count_option == 0) {
          display.drawBitmap(limited_icons, position[1][0], position[1][1], 48, 48, GxEPD_RED);
        } else if(count_option == 1) {
          display.drawBitmap(limited_icons, position[0][0], position[0][1], 48, 48, GxEPD_RED);
        }
        count_option++;
      }
  
      if(option[2]) {
        if(count_option == 0) {
          display.drawBitmap(cleance_icons, position[1][0], position[1][1], 48, 48, GxEPD_RED);
        } else if(count_option == 1) {
          display.drawBitmap(cleance_icons, position[0][0], position[0][1], 48, 48, GxEPD_RED);
        }
        count_option++;
      }

      // box right  
      display.drawBitmap(box, 132, 48, 114, 48, GxEPD_RED);
      // likes icon
      display.drawBitmap(likes_icons, 147, 59, 27, 27, GxEPD_BLACK);
      // likes count
      display.setCursor(192, 84);
    }
    display.setTextColor(GxEPD_BLACK);
    display.println(likes);
  }

  if (!option[3]) {
    // line from Baht symbol
    display.fillRect(22, 112, 3, 57, GxEPD_BLACK);
    display.fillRect(22, 117, 3, 48, GxEPD_WHITE);
  
    // B from Baht symbol
    display.setFont(&AGENCYR32pt7b);
    display.setCursor(10, 164);
    display.println("B");
  
    // price
    display.setTextColor(GxEPD_RED);
    display.setCursor(45, 164);
    display.println(price);
    
  } else {
    // line from Baht symbol 1
    display.fillRect(17, 120, 2, 40, GxEPD_BLACK);
    display.fillRect(17, 126, 2, 29, GxEPD_WHITE);
    
    // B from Baht symbol 1
    display.setFont(&AGENCYR21pt7b);
    display.setCursor(10, 155);
    display.println("B");

    // convert discounts to char*
    char discounts[promotion[3].length() + 1];
    promotion[3].toCharArray(discounts, sizeof(discounts));
    
    // price
    display.setTextColor(GxEPD_RED);
    display.setCursor(33, 155);
    display.println(discounts);

    // Offer ends
    display.setFont(&AGENCYR7pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(179, 148);
    display.println("Offer ends");
    
    // convert date to char*
    char date[promotion[2].length() + 1];
    promotion[2].toCharArray(date, sizeof(date));
    
    // date
    display.setCursor(179, 165);
    display.println(date);

    // line from Baht symbol 2
    display.fillRect(183, 114, 1, 19, GxEPD_BLACK);
    display.fillRect(183, 117, 1, 13, GxEPD_WHITE);
    
    // line width
    int width = 17;
    for(int i = 0; i < strlen(price); i++) {
      if(String(price[i]) != ",") {
        width += (i < 4) ? 8: 10;
      }
    }
    display.fillRect(177, 123, width, 1, GxEPD_BLACK);
    
    // B from Baht symbol 2
    display.setFont(&AGENCYR10pt7b);
    display.setCursor(179, 130);
    display.println("B");

    // promotion
    display.setCursor(191, 130);
    display.println(price);
  }

  display.update();
}

// display 1 refresh likes
void display_refresh_likes(char* likes, boolean* option) {
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&AGENCYR16pt7b);

  if((option[0] && !option[1] && !option[2]) || (!option[0] && option[1] && !option[2]) || 
          (!option[0] && !option[1] && option[2])) {
    display.fillRect(150, 52, 67, 37, GxEPD_WHITE);
    display.setCursor(162, 84);
    display.println(likes);
    display.updateWindow(71, 48, 176, 48);
  } else {
    display.fillRect(180, 52, 67, 37, GxEPD_WHITE);
    display.setCursor(192, 84);
    display.println(likes);
    display.updateWindow(131, 48, 114, 48);
  }
}

// display 1 refresh likes or share
void display_refresh_likes_share(int type, char* content) {
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&AGENCYR16pt7b);

  if(type == 0) {
    display.fillRect(60, 52, 70, 40, GxEPD_WHITE);
    display.setCursor(75, 84);
    display.println(content);
    display.updateWindow(14, 48, 114, 48);
  } else {
    display.fillRect(180, 52, 70, 40, GxEPD_WHITE);
    display.setCursor(195, 84);
    display.println(content);
    display.updateWindow(134, 48, 114, 48);
  }
}


